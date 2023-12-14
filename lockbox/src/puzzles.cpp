#include "puzzles.h"
#include "lockbox_common.h"
#include "switches.h"
#include "keypad_task.h"
#include "led_task.h"
#include <Adafruit_MMC56x3.h>

static bool solved_state[4] = {0};
// This array maps each puzzle to its LED.
// Since the middle LED is used as an indicator, 2 is skipped.
static const uint8_t puzzle_to_led_map[4] = {
  0, 1, 3, 4
};

template<class T> inline Print &operator <<(Print &obj, T arg) { obj.print(arg); return obj; }

// This state machine will run continuously while puzzle A is selected
bool switch_puzzle(void)
{
  static enum {
    RESET,
    NUM_1,
    NUM_2,
    NUM_3,
    NUM_4,
    NUM_5,
  } state = RESET;

  // Get our inputs
  uint16_t switch_state = read_switches();

  switch(state)
  {
    case RESET:
      set_strip(RED);
      (switch_state == SWITCHES_STATE_1) 
        ? state = NUM_1 
        : state = RESET;
      break;
    case NUM_1:
      set_led(0, GREEN);
      (switch_state == SWITCHES_STATE_2)
        ? state = NUM_2
        : (switch_state == SWITCHES_STATE_1) 
          ? // nothing
          : state = RESET;
      break;
    case NUM_2:
      set_led(1, GREEN);
      (switch_state == SWITCHES_STATE_3)
        ? state = NUM_3 
        : (switch_state == SWITCHES_STATE_2)
          ?
          : state = RESET;
      break;
    case NUM_3:
      set_led(2, GREEN);
      (switch_state == SWITCHES_STATE_4) 
        ? state = NUM_4 
        : (switch_state == SWITCHES_STATE_3)
          ?
          : state = RESET;
      break;
    case NUM_4:
      set_led(3, GREEN);
      (switch_state == SWITCHES_STATE_5) 
        ? state = NUM_5 
        : (switch_state == SWITCHES_STATE_4)
          ? 
          : state = RESET;
      break;
    case NUM_5:
      set_strip(GREEN);
      state = NUM_5;
      break;
  }

  return (state == NUM_5);
}

bool skutnik_puzzle(void)
{
  static uint16_t output = 0;
  static uint16_t combos[N_LEDS] = {
    SKUTNIK_COMBO_1,
    SKUTNIK_COMBO_2,
    SKUTNIK_COMBO_3,
    SKUTNIK_COMBO_4,
    SKUTNIK_COMBO_5
  };
  char input;

  auto new_input = xQueueReceive(keypad_queue_handle, &input, 0);
  if(new_input && (input >= '1' && input <= '5'))
  {
    int i = input - '0'; // Sneaky convert char to int
    output ^= combos[i - 1];
  }
  for(auto i = 0; i < N_LEDS; i++)
  {
    if(output & (1 << i))
    {
      set_led(i, GREEN);
    }
    else
    {
      set_led(i, RED);
    }
  }
  return (output == 0b11111);
}

bool compass_puzzle(void)
{
  sensors_event_t event;
  mmc.getEvent(&event);
  auto heading = (atan2(event.magnetic.y, event.magnetic.x) * 180) / PI;

  if(heading > -25 && heading <= -15)
  {
    set_strip(OFF);
    set_led(0, PURPLE);
  }
  else if(heading > -15 && heading <= -5)
  {
    set_strip(OFF);
    set_led(1, PURPLE);
  }
  else if(heading > -5 && heading <= 5)
  {
    set_strip(OFF);
    set_led(2, PURPLE);
  }
  else if(heading > 5 && heading <= 15)
  {
    set_strip(OFF);
    set_led(3, PURPLE);
  }
  else if(heading > 15 && heading <= 25)
  {
    set_strip(OFF);
    set_led(4, PURPLE);
  }
  else
  {
    set_strip(OFF);
  }

  vTaskDelay(MS(230));
  return false;
}

bool main_puzzle(void)
{
  static char selected_puzzle = '\0';
  // auto button_received = xQueueReceive(puzzle_select_queue_handle, &selected_puzzle, 0);
  selected_puzzle = get_selected_puzzle();
  // set_puzzle_leds(selected_puzzle);
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
        // set_led(0, CYAN);
      }
      break;
    case 'B':
      solved_state[1] = skutnik_puzzle();
      if(solved_state[1])
      {
        selected_puzzle = '\0';
      }
      else
      {
        // set_led(1, CYAN);
      }
      break;
    case 'C':
      solved_state[2] = compass_puzzle();
      break;
    case 'D':
      set_led(4, CYAN);
      break;
  }
  return true;
}