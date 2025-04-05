#include "vlc_send.h"
#include "main.h"

typedef struct
{
	uint8_t data[256];
	uint8_t head;
	uint8_t tail;
} queue_t;

#define queue_is_empty(queue) ((queue)->head == (queue)->tail)
#define queue_is_full(queue) (((queue)->head + 1) % 256 == (queue)->tail)

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

queue_t buffer;

uint8_t calculate_even_parity(uint8_t byte) {
	uint8_t parity = 0;
	for (uint8_t i = 0; i < 8; i++) {
	  parity ^= (byte >> i) & 1;
	}
	return parity;
  }

void set_bit(uint8_t bit)
{
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

void write_bit(uint8_t bit) {
	enqueue(&buffer, inverse(bit));
}

void add_byte(uint8_t byte)
{
	write_bit(0);
	for (uint8_t i = 0; i < 8; i++) {
		write_bit((byte >> i) & 1);
	}
	uint8_t even_parity = calculate_even_parity(byte);
	write_bit(even_parity);
	write_bit(1);
}

void vlc_task(void)
{
	if (!queue_is_empty(&buffer)) {
		set_bit(dequeue(&buffer));
	}
}
