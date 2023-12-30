#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <Adafruit_MMC56x3.h>

#include "i2c_task.h"
#include "input_task.h"
#include "keypad_task.h"
#include "led_task.h"
#include "lockbox_common.h"
#include "oled.h"
#include "puzzles.h"
#include "switches.h"

// This global is used to close the relay
bool gCloseRelay = false;

// MOSFET logic lives here for now
void blink_task(void *pvParameter)
{
  static bool led_state;
  while(true)
  {
    gpio_set_level(LED_BUILTIN, gCloseRelay);
    gpio_set_level(MOSFET_PIN, gCloseRelay);
    vTaskDelay(MS(100));
  }
}

template<class T> inline Print &operator <<(Print &obj, T arg) { obj.print(arg); return obj; }

// Magnetometer object
Adafruit_MMC5603 mmc = Adafruit_MMC5603(12345);
BluetoothSerial SerialBT;

void setup()
{
  // Configure the input/output pins
  configure_gpio();

  //Serial
  Serial.begin(9600);
  SerialBT.begin("Lockbox");

  led_init();

  i2c_init();

  oled_init();

  // Initialize magnetic sensor
  if (!mmc.begin(MMC56X3_DEFAULT_ADDRESS, &Wire))
  {
    Serial << "Unable to connect to MMC6503\n";
  }

  Serial.println("====== Creating Tasks ======");
  create_and_log_task(blink_task, "blink_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
  create_and_log_task(led_task, "led_task", 2000, NULL, 4, NULL);
  create_and_log_task(input_task, "command_task", 2000, NULL, 7, NULL);
  create_and_log_task(keypad_task, "keypad_task", 2000, NULL, 7, NULL);
  create_and_log_task(oled_task, "oled_task", 2000, NULL, 5, NULL);
}

bool celebrate = false;

static void unlock(void)
{
  celebrate = true;
  gCloseRelay = true;
  vTaskDelay(MS(750));
  gCloseRelay = false;
}

void loop()
{
  bool unlocked = false;
  while (1)
  {
    bool solved = main_puzzle();
    if(solved && !unlocked)
    {
      unlock();
      unlocked = true;
      clear_oled_body();
      set_oled_header("Unlocked!");
    }
    vTaskDelay(MS(20));
  }

  vTaskDelete(NULL);
}