/*
 * Blink
 * Turns on an LED on for one second,
 * then off for one second, repeatedly.
 */

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <Adafruit_MMC56x3.h>

#include "oled_task.h"
#include "i2c_task.h"
#include "debug.h"
#include "touch_task.h"

// Set LED_BUILTIN if it is not defined by Arduino framework
#ifndef LED_BUILTIN
    #define LED_BUILTIN GPIO_NUM_2
#endif

void blink_task(void *pvParameter)
{
  static bool led_state;
  while(true)
  {
    gpio_set_level(LED_BUILTIN, led_state);
    led_state = !led_state;
    vTaskDelay(ONE_SECOND);
  }
}

// Create a queue for sending debug strings to oled task.
QueueHandle_t oled_msg_queue = xQueueCreate(8, 21);

Adafruit_MMC5603 mmc = Adafruit_MMC5603(12345);

void setup()
{
  // initialize LED digital pin as an output.
  gpio_pad_select_gpio(LED_BUILTIN);
  gpio_set_direction(LED_BUILTIN, GPIO_MODE_OUTPUT);

  //Serial
  Serial.begin(9600);

  i2c_init();

  i2c_scan();

  if (!mmc.begin(MMC56X3_DEFAULT_ADDRESS, &Wire))
  {  // I2C mode
    /* There was a problem detecting the MMC5603 ... check your connections */
    Serial.println("Ooops, no MMC5603 detected ... Check your wiring!");
  }

  Serial.println("\nCreating Tasks...\n");

  xTaskCreate(blink_task, "blink", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
  xTaskCreate(oled_task, "oled", 2000, &oled_msg_queue, 5, NULL);
  xTaskCreate(touch_sensor_task, "touch", 2000, NULL, 5, NULL);
  Serial.println("Done!");
}

void loop()
{
  // This fn is required by the arduino framework, but we are not using it. 
  // So it will simply delete itself (for now)
  float heading;
  char str[21];
  while(1)
  {
    sensors_event_t event;
    mmc.getEvent(&event);
    heading = (atan2(event.magnetic.y, event.magnetic.x) * 180) / PI;

    sprintf(
      str, 
      "1:%i 2:%i", 
      static_cast<int>(heading), 
      static_cast<int>(event.magnetic.heading)
    );
    oled_debug(str);
    vTaskDelay(MS(250));
  }
  vTaskDelete(NULL);
}