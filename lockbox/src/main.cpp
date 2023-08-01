/*
 * Blink
 * Turns on an LED on for one second,
 * then off for one second, repeatedly.
 */

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "oled_task.h"

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

void setup()
{
  // initialize LED digital pin as an output.
  gpio_pad_select_gpio(LED_BUILTIN);
  gpio_set_direction(LED_BUILTIN, GPIO_MODE_OUTPUT);

  //Serial
  Serial.begin(9600);

  // I2C
  Wire.begin();

  byte error, address;
  int nDevices;
  Serial.println("Scanning...");
  nDevices = 0;
  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
      nDevices++;
    }
    else if (error==4) {
      Serial.print("Unknow error at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  }
  else {
    Serial.println("done\n");
  }

  Serial.println("\nCreating Tasks...\n");

  xTaskCreate(blink_task, "blink", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
  // xTaskCreate(oled_task, "oled", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
  Serial.println("Done!");
}

void loop()
{
  int FRC;
  FRC++;
  delay(1000);
}