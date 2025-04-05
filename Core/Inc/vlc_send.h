#ifndef VLC_SEND_H
#define VLC_SEND_H

#include <stdint.h>

extern void add_byte(uint8_t byte);
extern uint8_t get_bit(void);
extern void set_bit(uint8_t bit);

#endif // VLC_SEND_H
