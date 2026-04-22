#pragma once
#include <cstdint>

struct BotData {
    uint8_t throttle;  // 0.0 -> 100.0
    uint8_t battery;   // 0.0 -> 100.0
    uint8_t accX;      // 0.0 -> 100.0
    uint8_t accY;      // 0.0 -> 100.0
    uint8_t accZ;      // 0.0 -> 100.0
    uint16_t rpm;      // 0 -> 9999
    float radius;
    uint32_t interval_micros;
} __attribute__((packed));

void init_relay();
void relay_data(BotData data);