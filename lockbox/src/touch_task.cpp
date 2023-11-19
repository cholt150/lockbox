#include "Arduino.h"
#include "touch_task.h"
#include "debug.h"
#include "lockbox_common.h"
#include "BluetoothSerial.h"

extern bool gEnableMOSFET;
extern BluetoothSerial SerialBT;

void touch_sensor_init()
{

}

void touch_sensor_task(void *pvParams)
{
  bool touch_state = false;
  char touch_msg[] =  "TOUCH";
  char no_touch_msg[] =  "...";
  while(1)
  {
    auto val = touchRead(4);
    if(val < 20)
    {
      sprintf(touch_msg, "TOUCH %i", val);
      // Serial.println(touch_msg);
      if(!touch_state)
      {
        gEnableMOSFET = !gEnableMOSFET;
        // Notify BT terminal of state change
        SerialBT.printf("MOSFET %i\n", gEnableMOSFET);
      }
      touch_state = true;
    }
    else
    {
      touch_state = false;
      sprintf(no_touch_msg, "----- %i", val);
      // Serial.println(no_touch_msg);
    }
    vTaskDelay(MS(100));
  }
  vTaskDelete(NULL);
}