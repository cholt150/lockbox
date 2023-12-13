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

bool prev_keypad_state[KEYPAD_ROWS][KEYPAD_COLUMNS] = {
  0,0,0,0,
  0,0,0,0,
  0,0,0,0,
  0,0,0,0
};

static char keypad_lookup_table[KEYPAD_ROWS][KEYPAD_COLUMNS] = {
  '1','2','3','A',
  '4','5','6','B',
  '7','8','9','C',
  '*','0','#','D',
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
  // R3 and R4 do not have internal pull-downs. 1MOhm resistors are to be attached externally.
  gpio_pad_select_gpio(R3_PIN);
  gpio_set_direction(R3_PIN, GPIO_MODE_INPUT);
  gpio_pad_select_gpio(R4_PIN);
  gpio_set_direction(R4_PIN, GPIO_MODE_INPUT);
}

static char poll_keypad(void)
{
  char character_pressed = '\0';
  for(auto col = 0; col < KEYPAD_COLUMNS; col++)
  {
    bool input_detected = false;
    Edge_Type edge;
    // Turn on the voltage to this column
    gpio_set_level(col_pins[col], GPIO_ON);
    for(auto row = 0; row < KEYPAD_ROWS; row++)
    {
      bool prev_state = prev_keypad_state[row][col];
      // Read the row pin. If this is 1, we have a successful input
      input_detected = gpio_get_level(row_pins[row]);
      if(input_detected)
      {
        // Do a dumb debounce. I think if it's still pressed
        vTaskDelay(MS(KEYPAD_DEBOUNCE));
        input_detected = gpio_get_level(row_pins[row]);
      }
      keypad_state[row][col] = input_detected;

      if(input_detected && !prev_keypad_state[row][col])
      {
        edge = RISING_EDGE;
      }
      else if(!input_detected && prev_keypad_state[row][col])
      {
        edge = FALLING_EDGE;
      }
      else
      {
        edge = NO_EDGE;
      }
      prev_keypad_state[row][col] = input_detected;

      switch(edge)
      {
        case RISING_EDGE:
          character_pressed = keypad_lookup_table[row][col];
          break;
        case FALLING_EDGE:
        case NO_EDGE:
        default:
          character_pressed = '\0';
          break;
      }
      if(input_detected) 
      {
        break;
      }
    }
    // Turn the column voltage back off
    gpio_set_level(col_pins[col], GPIO_OFF);
    if(input_detected) 
    {
      break;
    }
  }
  return character_pressed;
}

void keypad_task(void *pvParams)
{
  while(1)
  {
    char button_read = poll_keypad();
    if('\0' != button_read){
      if((button_read >= 'A') && (button_read <= 'D'))
      {
        xQueueSendToBack(puzzle_select_queue_handle, &button_read, MS(1));
      }
      else
      {
        xQueueSendToBack(keypad_queue_handle, &button_read, MS(1));
      }
    }

    vTaskDelay(MS(KEYPAD_POLL_RATE));
  }
}