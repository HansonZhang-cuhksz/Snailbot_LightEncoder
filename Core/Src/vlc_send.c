#include "vlc_send.h"
#include "main.h"

uint8_t curr_bit;
uint32_t set_bit_activate_count = 0;

typedef struct
{
	uint8_t data[256];
	uint8_t head;
	uint8_t tail;
} queue_t;

#define queue_is_empty(queue) (queue->head == queue->tail)
#define queue_is_full(queue) ((queue->head + 1) % 256 == queue->tail)

void enqueue(queue_t *queue, uint8_t data)
{
	if (queue_is_full(queue))
		return;
	queue->data[queue->head] = data;
	queue->head = (queue->head + 1) % 256;
}

uint8_t dequeue(queue_t *queue)
{
	if (queue_is_empty(queue))
		return 0xFF;
	uint8_t data = queue->data[queue->tail];
	queue->tail = (queue->tail + 1) % 256;
	return data;
}

queue_t recv_buf_bits;

uint8_t calculate_even_parity(uint8_t byte)
{
    uint8_t parity = 0;
    uint8_t temp = byte;

    // Calculate the number of 1 bits in the byte
    while (temp)
    {
        parity ^= (temp & 1);
        temp >>= 1;
    }

    // If parity is 1, it means the number of 1 bits is odd, so we set the parity bit to 1
    // If parity is 0, it means the number of 1 bits is even, so we set the parity bit to 0
    return (byte & 0xFF) | (parity << 8);
}

void set_bit(uint8_t bit)
{
	curr_bit = bit;
	set_bit_activate_count++;
	if (bit)	// Low duty (bright) cycle for 1, high duty (dark) cycle for 0
	{
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 1999);
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 1999);
		__HAL_TIM_SET_COMPARE(&htim14, TIM_CHANNEL_1, 1999);
		__HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1, 1999);
	}
	else
	{
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 6999);
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 6999);
		__HAL_TIM_SET_COMPARE(&htim14, TIM_CHANNEL_1, 6999);
		__HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1, 6999);
	}
}

void add_byte(uint8_t byte)
{
	enqueue(&recv_buf_bits, 0);	// Start bit
	for (uint8_t i = 0; i < 8; i++)
	{
		enqueue(&recv_buf_bits, byte & 1);
		byte >>= 1;
	}
	enqueue(&recv_buf_bits, calculate_even_parity(byte));	// Parity bit
	enqueue(&recv_buf_bits, 1);	// Stop bit
}

uint8_t get_bit(void)
{
	return dequeue(&recv_buf_bits);
}
