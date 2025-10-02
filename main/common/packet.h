#ifndef PACKET_H
#define PACKET_H

#include <stdint.h>

// Normalized control packet
// steering: -128 = full left, 0 = center, 127 = full right
// throttle: -128 = full reverse, 0 = stop, 127 = full forward
typedef struct __attribute__((packed)) {
    int8_t steering;
    int8_t throttle;
} control_packet_t;

#endif