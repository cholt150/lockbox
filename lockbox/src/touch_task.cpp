#include "Arduino.h"
#include "touch_task.h"
#include "debug.h"
#include "rtos_macros.h"

void touch_sensor_init()
{

}

void touch_sensor_task(void *pvParams)
{
  char touch_msg[] =  "TOUCH";
  char no_touch_msg[] =  "...";
  while(1)
  {
    auto val = touchRead(4);
    if(val < 20)
    {
      sprintf(touch_msg, "TOUCH %i", val);
      // oled_debug(touch_msg);
    }
    else
    {
      sprintf(no_touch_msg, "----- %i", val);
      // oled_debug(no_touch_msg);
    }
    vTaskDelay(MS(100));
  }
  vTaskDelete(NULL);
}