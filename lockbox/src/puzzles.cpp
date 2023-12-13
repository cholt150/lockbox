#include "puzzles.h"
#include "lockbox_common.h"
#include "switches.h"
#include "led_task.h"

static bool solved_state[4] = {0};
// This array maps each puzzle to its LED.
// Since the middle LED is used as an indicator, 2 is skipped.
static const uint8_t puzzle_to_led_map[4] = {
  0, 1, 3, 4
};

bool switch_puzzle(void)
{
  auto switch_state = read_switches();
  return (read_switches() == SWITCH_ANSWER);
}

bool keypad_puzzle(void)
{
  // Make sure our passcode is the proper length
  static_assert(PASSCODE_LENGTH == strlen(PASSCODE));
  static char input_sequence[PASSCODE_LENGTH + 1] {'\0'};
  char input;
  auto new_entry = xQueueReceive(keypad_queue_handle, &input, 0);
  if(new_entry)
  {

    auto i = 0;
    for(; i < PASSCODE_LENGTH; i++)
    {
      input_sequence[i] = input_sequence[i+1];
    }
    input_sequence[PASSCODE_LENGTH-1] = input;
    Serial.printf("%s\n", input_sequence);
  }
  return !(strcmp(input_sequence, PASSCODE));
}

static void set_puzzle_leds(char input)
{
  static char last_input = '\0';
  for(auto i = 0; i < 4; i++)
  {
    uint8_t this_puzzle_LED = puzzle_to_led_map[i];
    if(true == solved_state[i])
    {
      set_led(this_puzzle_LED, GREEN);
    }
    else
    {
      set_led(this_puzzle_LED, RED);
    }
  }
}

bool main_puzzle(void)
{
  static char selected_puzzle = '\0';
  auto button_received = xQueueReceive(puzzle_select_queue_handle, &selected_puzzle, 0);
  if(button_received || '\0' == selected_puzzle)
  {
    set_puzzle_leds(selected_puzzle);
  }
  switch(selected_puzzle)
  {
    case 'A':
      solved_state[0] = switch_puzzle();
      if(solved_state[0])
      {
        selected_puzzle = '\0';
      }
      else
      {
        set_led(0, YELLOW);
      }
      break;
    case 'B':
      solved_state[1] = keypad_puzzle();
      if(solved_state[1])
      {
        selected_puzzle = '\0';
      }
      else
      {
        set_led(1, YELLOW);
      }
      break;
    case 'C':
      set_led(3, YELLOW);
      break;
    case 'D':
      set_led(4, YELLOW);
      break;
  }
  return true;
}