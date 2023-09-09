#include "led_task.h"
#include "rtos_macros.h"

#include <Adafruit_NeoPixel.h>

void led_task(void *pvParams)
{
    Adafruit_NeoPixel strip(N_LEDS, LED_PIN, NEO_GRB);
    strip.begin();

    while(1)
    {
        for(int i=0; i<N_LEDS; i++) {
            strip.clear();
            strip.setPixelColor(i, strip.Color(128, 0, 0));
            strip.show();
            vTaskDelay(MS(50));
        }
    }
}