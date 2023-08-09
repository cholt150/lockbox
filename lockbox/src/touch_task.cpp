#include "Arduino.h"
#include "touch_task.h"
#include "debug.h"
#include "rtos_macros.h"

void touch_sensor_init()
{

}

void touch_sensor_task(void *pvParams)
{
  while(1)
  {
    oled_debug(static_cast<int>(touchRead(4)));
    vTaskDelay(MS(500));
  }
  vTaskDelete(NULL);
}