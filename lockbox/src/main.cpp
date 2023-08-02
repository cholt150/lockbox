/*
 * Blink
 * Turns on an LED on for one second,
 * then off for one second, repeatedly.
 */

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

#include "oled_task.h"
#include "i2c_task.h"

// Set LED_BUILTIN if it is not defined by Arduino framework
#ifndef LED_BUILTIN
    #define LED_BUILTIN GPIO_NUM_2
#endif

#define ONE_SECOND 1000 / portTICK_PERIOD_MS

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

void setup()
{
  // initialize LED digital pin as an output.
  gpio_pad_select_gpio(LED_BUILTIN);
  gpio_set_direction(LED_BUILTIN, GPIO_MODE_OUTPUT);

  //Serial
  Serial.begin(9600);

  i2c_init();

  i2c_scan();

  Serial.println("\nCreating Tasks...\n");

  xTaskCreate(blink_task, "blink", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
  xTaskCreate(oled_task, "oled", 2000, &oled_msg_queue, 5, NULL);
  Serial.println("Done!");
}

void loop()
{
  // This fn is required by the arduino framework, but we are not using it. 
  // So it will simply delete itself (for now)
  static int FRC;
  char string[21];
  sprintf(string, "%i", FRC++);
  xQueueSendToBack(oled_msg_queue, string, portMAX_DELAY);
  vTaskDelay(ONE_SECOND);
  // vTaskDelete(NULL);
}