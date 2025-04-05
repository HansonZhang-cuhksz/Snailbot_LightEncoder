#ifndef VLC_SEND_H
#define VLC_SEND_H

#include <stdint.h>

#define inverse(x) x == 0 ? 1 : 0
#define FREQ 50

extern void add_byte(uint8_t byte);
extern void set_bit(uint8_t bit);
extern void vlc_task(void);

#endif // VLC_SEND_H
