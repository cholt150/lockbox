#ifndef LED_TASK_H
#define LED_TASK_H

#define LED_PIN 13
#define N_LEDS 5

#include "BluetoothSerial.h"

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

extern BluetoothSerial SerialBT;

typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} rgb_color;

typedef struct {
  uint8_t led_num;
  rgb_color color;
  uint16_t duration;
} led_seq_state;

#define RED     rgb_color{255,0,0}
#define GREEN   rgb_color{0,255,0}
#define BLUE    rgb_color{0,0,255}
#define CYAN    rgb_color{0,255,255}
#define YELLOW  rgb_color{255,128,0}
#define OFF     rgb_color{0,0,0}

void led_init(void);
void led_task(void *pvParams);
void set_strip(rgb_color color);
void set_led(uint8_t led_number, rgb_color color);
void add_to_led_sequence(uint8_t led_num, rgb_color color, uint16_t duration);

#endif // DEBUG