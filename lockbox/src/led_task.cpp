#include "led_task.h"
#include "lockbox_common.h"

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// Create A queue for setting 
QueueHandle_t led_queue = xQueueCreate(N_LEDS, sizeof(led_state));

// Create the LED strip manager object
Adafruit_NeoPixel strip(N_LEDS, LED_PIN, NEO_GRB);  

rgb_triple colors[NUM_COLORS] = {
    rgb_triple{0,0,0},     // OFF
    rgb_triple{128,0,0},   // RED
    rgb_triple{0,128,0},   // GREEN
    rgb_triple{0,0,128},   // BLUE
    rgb_triple{128,128,0}  // YELLOW
};

void set_led(uint8_t led_number, uint8_t color)
{
    led_state state_to_send = {
        led_number,
        color
    };
    xQueueSendToBack(led_queue, &state_to_send, portMAX_DELAY);
};

void test_leds(void)
{
    static uint8_t color;
    set_strip(color);
    ++color %= NUM_COLORS;
    vTaskDelay(MS(1000));
}

void set_strip(uint8_t color)
{
    for (auto i = 0; i < N_LEDS; i++)
    {
        set_led(i, color);
    }
}

void led_task(void *pvParams)
{
    strip.begin();

    static led_state led_to_process;
    
    while(1)
    {
        xQueueReceive(led_queue, &led_to_process, portMAX_DELAY);
        rgb_triple color = colors[led_to_process.color];
        strip.setPixelColor(led_to_process.led_number, color.red, color.green, color.blue);
        strip.show();
    }
}