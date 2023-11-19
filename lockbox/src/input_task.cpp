#include "input_task.h"
#include "lockbox_common.h"
#include "BluetoothSerial.h"

#include <Arduino.h>
#include <WiFi.h>
#include <CommandParser.h>

extern bool gEnableMOSFET;

extern BluetoothSerial SerialBT;

typedef CommandParser<> MyCommandParser;
MyCommandParser parser;

void cmd_test(MyCommandParser::Argument *args, char *response) {
    strlcpy(response, "success", MyCommandParser::MAX_RESPONSE_SIZE);
    gEnableMOSFET = true;
    delay(750);
    gEnableMOSFET = false;
}

void input_task(void * pvParams)
{
    parser.registerCommand("UNLOCK", "s", &cmd_test);
    Serial.println("registered command: TEST <string> <double> <int64> <uint64>");
    Serial.println("example: TEST \"\\x41bc\\ndef\" -1.234e5 -123 123");

    while(1)
    {
        if(SerialBT.available())
        {
            char buffer[128];
            size_t line_length = SerialBT.readBytesUntil('\n', buffer, 127);
            buffer[line_length] = '\0';

            char response[MyCommandParser::MAX_RESPONSE_SIZE];
            parser.processCommand(buffer, response);
            SerialBT.println(response);
        }
        vTaskDelay(MS(100));
    }
}