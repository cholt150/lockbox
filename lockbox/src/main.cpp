#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <Adafruit_MMC56x3.h>

#include "lockbox_common.h"
#include "i2c_task.h"
#include "touch_task.h"
#include "led_task.h"
#include "input_task.h"
#include "keypad_task.h"
#include "switches.h"
#include "puzzles.h"

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

  i2c_init();

  // Initialize magnetic sensor
  if (!mmc.begin(MMC56X3_DEFAULT_ADDRESS, &Wire))
  {
    Serial << "Unable to connect to MMC6503\n";
  }

  Serial.println("====== Creating Tasks ======");
  create_and_log_task(blink_task, "blink_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
  create_and_log_task(led_task, "led_task", 2000, NULL, 5, NULL);
  create_and_log_task(input_task, "command_task", 2000, NULL, 7, NULL);
  create_and_log_task(keypad_task, "keypad_task", 2000, NULL, 7, NULL);
  create_and_log_task(touch_sensor_task, "touch_sensor_task", 2000, NULL, 5, NULL);
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

static void unlock(void)
{
  gCloseRelay = true;
  vTaskDelay(MS(750));
  gCloseRelay = false;
}

lock_state state = RESET;

void loop()
{
  // This fn is required by the arduino framework, but we are not using it. 
  // So it will simply delete itself (for now)
  float heading;
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
  
  while (1)
  {
    main_puzzle();
    vTaskDelay(MS(20));
  }
  

  String passcode = "";
  char c = '\0';

  while(1) {
    switch(state) {
      case RESET:
        run_startup_colors();
        state = STAGE_1;
        xQueueReset(keypad_queue_handle);
        break;
      case STAGE_1:
        xQueueReceive(keypad_queue_handle, &c, portMAX_DELAY);
        SerialBT.printf("Pressed: %c\n", c);
        passcode += c;
        if(c == '#')
        {
          SerialBT << passcode << '\n';
          Serial << passcode << '\n';
          if(passcode == "1234#")
          {
            state = STAGE_5;
          }
          passcode = "";
        }
        break;
      case STAGE_2:
        break;
      case STAGE_3:
        break;
      case STAGE_4:
        break;
      case STAGE_5:
        unlock();
        state = DONE;
        break;
      case DONE:
        break;
      default:
        state = RESET;
        break;
    }

    SerialBT << "switch_state: " << read_switches() << "\n";

    sensors_event_t event;
    mmc.getEvent(&event);
    heading = (atan2(event.magnetic.y, event.magnetic.x) * 180) / PI;

    Serial << "X: " << event.magnetic.x << " Y: " << event.magnetic.y << " Z: " << event.magnetic.z << '\n';
    Serial << heading << '\n';
    Serial << event.magnetic.heading << '\n';

    vTaskDelay(MS(100));
  }
  vTaskDelete(NULL);
}