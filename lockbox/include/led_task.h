#ifndef LED_TASK_H
#define LED_TASK_H

#define LED_PIN 13
#define N_LEDS 5

#include "BluetoothSerial.h"

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

extern BluetoothSerial SerialBT;

enum {
    OFF,
    RED,
    GREEN,
    BLUE,
    YELLOW,
    NUM_COLORS
};

typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} rgb_triple;

typedef struct {
    uint8_t led_number;
    uint8_t color;
} led_state;

void led_task(void *pvParams);
void test_leds(void);
void set_strip(uint8_t color);
void set_led(uint8_t led_number, uint8_t color);

#endif // DEBUG