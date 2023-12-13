#include "lockbox_common.h"
#include "keypad_task.h"
#include "switches.h"

QueueHandle_t keypad_queue_handle = xQueueCreate(5, sizeof(char));
QueueHandle_t puzzle_select_queue_handle = xQueueCreate(5, sizeof(char));

template<class T> inline Print &operator <<(Print &obj, T arg) { obj.print(arg); return obj; }

void create_and_log_task(
  TaskFunction_t pvTaskCode,
  const char * const pcName,
  const uint32_t usStackDepth,
  void * const pvParameters,
  UBaseType_t uxPriority,
  TaskHandle_t * const pxCreatedTask)
{
  Serial << "Creating task: " << pcName << " Stack size: " << usStackDepth;
  xTaskCreate(pvTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pxCreatedTask);
  Serial << " Done!\n";
}

void configure_gpio()
{
  // initialize LED digital pin as an output.
  gpio_pad_select_gpio(LED_BUILTIN);
  gpio_set_direction(LED_BUILTIN, GPIO_MODE_OUTPUT);

  // initialize MOSFET digital pin as an output.
  gpio_pad_select_gpio(MOSFET_PIN);
  gpio_set_direction(MOSFET_PIN, GPIO_MODE_OUTPUT);

  switches_init();

  keypad_init();
}