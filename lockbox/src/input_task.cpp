#include "input_task.h"
#include "led_task.h"
#include "lockbox_common.h"
#include "BluetoothSerial.h"

#include <Arduino.h>
#include <WiFi.h>
#include <Cmd.h>

extern bool gEnableMOSFET;

extern BluetoothSerial SerialBT;

extern lock_state state;

void cmd_get_status(int argc, char **argv) {
  auto stream = cmdGetStream();
  stream->printf("Lock Status: %i\n", state);
}

void cmd_unlock(int argc, char **argv) {
  int pulse = 750;
  if(argc > 1) {
    pulse = atoi(argv[1]);
  }
  for(auto i = 0; i < 5; i-=-1) {
    set_strip(GREEN);
    delay(200);
    set_strip(OFF);
    delay(200);
  }
  gEnableMOSFET = true;
  delay(pulse);
  gEnableMOSFET = false;
}

void input_task(void * pvParams)
{
  // Link up the command parser to the BT Serial stream
  cmdInit(&SerialBT);
  cmdAdd("unlock", cmd_unlock);
  cmdAdd("status", cmd_get_status);

  while(1)
  {
    cmdPoll();
    vTaskDelay(MS(100));
  }
}