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

#include "lockbox_common.h"
#include "BluetoothSerial.h"
#include "debug.h"
#include "oled_task.h"
#include "i2c_task.h"
#include "touch_task.h"
#include "led_task.h"
#include "input_task.h"
#include "keypad_task.h"

// Set LED_BUILTIN if it is not defined by Arduino framework
#ifndef LED_BUILTIN
    #define LED_BUILTIN GPIO_NUM_2
#endif
// Our latch MOSFET is tied to GPIO 12
#define MOSFET_PIN GPIO_NUM_12

// This global is used to set the state of the MOSFET
bool gEnableMOSFET = false;

// MOSFET logic lives here for now
void blink_task(void *pvParameter)
{
  static bool led_state;
  while(true)
  {
    gpio_set_level(LED_BUILTIN, gEnableMOSFET);
    gpio_set_level(MOSFET_PIN, gEnableMOSFET);
    vTaskDelay(MS(100));
  }
}

template<class T> inline Print &operator <<(Print &obj, T arg) { obj.print(arg); return obj; }

// Create a queue for sending debug strings to oled task.
QueueHandle_t oled_msg_queue = xQueueCreate(8, 21);

// Magnetometer object
Adafruit_MMC5603 mmc = Adafruit_MMC5603(12345);
BluetoothSerial SerialBT;

void setup()
{
  // initialize LED digital pin as an output.
  gpio_pad_select_gpio(LED_BUILTIN);
  gpio_set_direction(LED_BUILTIN, GPIO_MODE_OUTPUT);

  // initialize MOSFET digital pin as an output.
  gpio_pad_select_gpio(MOSFET_PIN);
  gpio_set_direction(MOSFET_PIN, GPIO_MODE_OUTPUT);

  //Serial
  Serial.begin(9600);
  SerialBT.begin("Lockbox");

  keypad_init();

  i2c_init();

  i2c_scan();

  if (!mmc.begin(MMC56X3_DEFAULT_ADDRESS, &Wire))
  {  // I2C mode
    /* There was a problem detecting the MMC5603 ... check your connections */
    Serial.println("Ooops, no MMC5603 detected ... Check your wiring!");
  }

  Serial.println("\nCreating Tasks...\n");

  xTaskCreate(blink_task, "blink", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
  xTaskCreate(led_task, "leds", 2000, NULL, 5, NULL);
  xTaskCreate(input_task, "serial_input", 2000, NULL, 7, NULL);
  xTaskCreate(keypad_task, "keypad", 2000, NULL, 7, NULL);
  // xTaskCreate(oled_task, "oled", 2000, &oled_msg_queue, 5, NULL);
  xTaskCreate(touch_sensor_task, "touch", 2000, NULL, 5, NULL);
  Serial.println("Done!");
}

static void run_startup_colors(void)
{
  set_led(2, BLUE);
  delay(500);
  set_led(1, GREEN);
  set_led(3, GREEN);
  delay(500);
  set_led(0, RED);
  set_led(4, RED);
  delay(2000);
  set_strip(OFF);
}

lock_state state = RESET;

void loop()
{
  // This fn is required by the arduino framework, but we are not using it. 
  // So it will simply delete itself (for now)
  // float heading;
  // char str[21];
  // while(1)
  // {
  //   sensors_event_t event;
  //   mmc.getEvent(&event);
  //   heading = (atan2(event.magnetic.y, event.magnetic.x) * 180) / PI;

  //   sprintf(
  //     str, 
  //     "1:%i 2:%i", 
  //     static_cast<int>(heading), 
  //     static_cast<int>(event.magnetic.heading)
  //   );
  //   SerialBT.println(str);
  //   vTaskDelay(MS(1000));
  // }

  // Enumeration to describe lock states
  run_startup_colors();

  while(1) {
    switch(state) {
      case RESET:
        break;
      case STAGE_1:
        break;
      case STAGE_2:
        break;
      case STAGE_3:
        break;
      case STAGE_4:
        break;
      case STAGE_5:
        break;
      default:
        state = RESET;
        break;
    }

    vTaskDelay(MS(100));
  }
  vTaskDelete(NULL);
}