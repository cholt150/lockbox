#include <Arduino.h>

#include "lockbox_common.h"
#include "Adafruit_Keypad.h"

#define KEYPAD_PID3844

#define C4    14
#define C3    27
#define C2    26
#define C1    25
#define R4    33
#define R3    32
#define R2    35
#define R1    34
// leave this import after the above configuration
#include "keypad_config.h"

void keypad_task(void *pvParams)
{
    Adafruit_Keypad customKeypad = Adafruit_Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS);

    while(1)
    {
        customKeypad.tick();

        while(customKeypad.available())
        {
            keypadEvent e = customKeypad.read();
            Serial.print((char)e.bit.KEY);
            if(e.bit.EVENT == KEY_JUST_PRESSED) Serial.println(" pressed");
            else if(e.bit.EVENT == KEY_JUST_RELEASED) Serial.println(" released");
        }

        vTaskDelay(MS(10));
    }
}