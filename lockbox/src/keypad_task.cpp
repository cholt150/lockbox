#include <Arduino.h>

#include "BluetoothSerial.h"
#include "lockbox_common.h"
#include "keypad_task.h"

extern BluetoothSerial SerialBT;

bool keypad_state[KEYPAD_ROWS][KEYPAD_COLUMNS] = {
    0,0,0,0,
    0,0,0,0,
    0,0,0,0,
    0,0,0,0
};

gpio_num_t row_pins[KEYPAD_ROWS] = {
    R1_PIN,
    R2_PIN,
    R3_PIN,
    R4_PIN
};

gpio_num_t col_pins[KEYPAD_COLUMNS] = {
    C1_PIN,
    C2_PIN,
    C3_PIN,
    C4_PIN
};

void keypad_init(void)
{
    gpio_pad_select_gpio(C1_PIN);
    gpio_set_direction(C1_PIN, GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(C2_PIN);
    gpio_set_direction(C2_PIN, GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(C3_PIN);
    gpio_set_direction(C3_PIN, GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(C4_PIN);
    gpio_set_direction(C4_PIN, GPIO_MODE_OUTPUT);
    
    gpio_pad_select_gpio(R1_PIN);
    gpio_set_direction(R1_PIN, GPIO_MODE_INPUT);
    gpio_pulldown_en(R1_PIN);
    gpio_pad_select_gpio(R2_PIN);
    gpio_set_direction(R2_PIN, GPIO_MODE_INPUT);
    gpio_pulldown_en(R2_PIN);
    gpio_pad_select_gpio(R3_PIN);
    gpio_set_direction(R3_PIN, GPIO_MODE_INPUT);
    gpio_pad_select_gpio(R4_PIN);
    gpio_set_direction(R4_PIN, GPIO_MODE_INPUT);
}

static void poll_keypad(void)
{
    for(auto col = 0; col < KEYPAD_COLUMNS; col++)
    {
        gpio_set_level(col_pins[col], GPIO_ON);
        for(auto row = 0; row < KEYPAD_ROWS; row++)
        {
            keypad_state[row][col] = gpio_get_level(row_pins[row]);
            if(keypad_state[row][col])
            {
                SerialBT.printf("Pressed: %i %i\n", row, col);
            }
        }
        gpio_set_level(col_pins[col], GPIO_OFF);
    }
}

void keypad_task(void *pvParams)
{
    while(1)
    {
        poll_keypad();

        vTaskDelay(MS(KEYPAD_POLL_RATE));
    }
}