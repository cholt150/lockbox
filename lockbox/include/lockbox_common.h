#ifndef LOCKBOX_COMMON_H
#define LOCKBOX_COMMON_H

#include <Arduino.h>

#include "BluetoothSerial.h"

// Set LED_BUILTIN if it is not defined by Arduino framework
#ifndef LED_BUILTIN
    #define LED_BUILTIN GPIO_NUM_2
#endif
// Our latch MOSFET is tied to GPIO 12
#define MOSFET_PIN GPIO_NUM_12

#define ONE_SECOND 1000 / portTICK_PERIOD_MS
#define MS(x) x / portTICK_PERIOD_MS

#define vstr(s) str(s)
#define str(s) #s

#define GPIO_ON 1
#define GPIO_OFF 0

template<class T> inline Print &operator <<(Print &obj, T arg);

extern BluetoothSerial SerialBT;

enum lock_state
{
  RESET,
  STAGE_1,
  STAGE_2,
  STAGE_3,
  STAGE_4,
  STAGE_5,
  DONE
};

extern QueueHandle_t keypad_queue_handle;
extern QueueHandle_t puzzle_select_queue_handle;

void create_and_log_task(
  TaskFunction_t pvTaskCode,
  const char * const pcName,
  const uint32_t usStackDepth,
  void * const pvParameters,
  UBaseType_t uxPriority,
  TaskHandle_t * const pxCreatedTask
);

void configure_gpio(void);

#endif // !LOCKBOX_COMMON_H