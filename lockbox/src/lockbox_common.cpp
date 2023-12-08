#include "lockbox_common.h"

QueueHandle_t keypad_queue_handle = xQueueCreate(5, sizeof(char));

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
