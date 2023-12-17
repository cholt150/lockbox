#include "led_task.h"
#include "lockbox_common.h"

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <queue>

// Create A queue for setting 
QueueHandle_t led_queue = xQueueCreate(4, sizeof(led_seq_state));

TimerHandle_t led_timers[N_LEDS] {0};
std::queue<led_seq_state> led_queues[N_LEDS];

// Create the LED strip manager object
Adafruit_NeoPixel strip(N_LEDS, LED_PIN, NEO_GRB);

static void nop(TimerHandle_t xTimer)
{
  return;
}

void led_init(void)
{
  for(auto i = 0; i<N_LEDS; i++)
  {
    char name[16] {0};
    sprintf(name, "led_timer_%i", i);
    led_timers[i] = xTimerCreate(
      name,
      MS(10),
      pdFALSE,
      NULL,
      nop
    );
  }
}

void set_led(uint8_t led_number, rgb_color color)
{
  led_seq_state state_to_send = {
    led_number,
    color,
    0
  };
  xQueueSendToBack(led_queue, &state_to_send, portMAX_DELAY);
};

void set_strip(rgb_color color)
{
  for (auto i = 0; i < N_LEDS; i++)
  {
    set_led(i, color);
  }
}

void add_to_led_sequence(uint8_t led_num, rgb_color color, uint16_t duration)
{
  led_seq_state state_to_send = {
    led_num,
    color,
    duration
  };
  xQueueSendToBack(led_queue, &state_to_send, portMAX_DELAY);
}

void led_task(void *pvParams)
{
  strip.begin();
  strip.setBrightness(128);
  strip.clear();

  uint16_t frc;
  
  while(1)
  {
    frc += 32;
    if(!celebrate)
    {
      led_seq_state led_to_process {0};
      while(pdTRUE == xQueueReceive(led_queue, &led_to_process, 0))
      {
        led_queues[led_to_process.led_num].push(led_to_process);
      }

      for(auto i = 0; i < N_LEDS; i++)
      {
        if(
          pdFALSE == xTimerIsTimerActive(led_timers[i]) &&
          !led_queues[i].empty()
          )
        {
          // If this led queue has a new state and the previous timer has expired
          led_seq_state this_state;
          this_state = led_queues[i].front();
          led_queues[i].pop();
          strip.setPixelColor(
            this_state.led_num,
            this_state.color.r,
            this_state.color.g,
            this_state.color.b
          );
          if(this_state.duration != 0)
          {
            xTimerChangePeriod(led_timers[i], MS(this_state.duration), MS(1));
            xTimerStart(led_timers[i], MS(1));
          }
        }
      }
    }
    else
    {
      strip.rainbow(frc);
    }
    strip.show();
    vTaskDelay(MS(1));
  }
}