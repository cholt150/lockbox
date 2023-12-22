#include "keypad_task.h"
#include "led_task.h"
#include "lockbox_common.h"
#include "oled.h"
#include "puzzles.h"
#include "switches.h"
#include <Adafruit_MMC56x3.h>

static bool solved_state[4] = {0};

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
  static bool solved = false;
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
  char str[23];
  sprintf(str, "<INFO> 0x%02X", output);
  set_oled_body(3, str);
  solved = (output == 0b11111);
  return solved;
}

bool compass_puzzle(bool &restart)
{
  static bool solved = false;
  static uint16_t n_times_on_target = 0;

  int targets[3] = {0, -50, 50};
  static rgb_color target_colors[4] = {ORANGE, CYAN, GREEN, GREEN};
  static int targets_hit = 0;
  static rgb_color background_color = OFF;
  static rgb_color foreground_color = target_colors[targets_hit];

  sensors_event_t event;
  mmc.getEvent(&event);
  auto heading = (atan2(event.magnetic.y, event.magnetic.x) * 180) / PI;

  if(!solved)
  {
    if(restart)
    {
      restart = false;
      n_times_on_target = 0;
      targets_hit = 0;
      background_color = OFF;
      foreground_color = target_colors[targets_hit];
    }

    int target = targets[targets_hit];

    if(heading > (target-25) && heading <= (target-15))
    {
      n_times_on_target = 0;
      set_strip(background_color);
      set_led(0, foreground_color);
    }
    else if(heading > (target-15) && heading <= (target-5))
    {
      n_times_on_target = 0;
      set_strip(background_color);
      set_led(1, foreground_color);
    }
    else if(heading > (target-5) && heading <= (target+5))
    {
      auto target_color = foreground_color;
      (n_times_on_target % 2 == 0) ? target_color = foreground_color : target_color = background_color;
      n_times_on_target++;
      set_strip(background_color);
      set_led(2, target_color);
      if(n_times_on_target >= 4)
      {
        set_led(1, target_color);
        set_led(3, target_color);
      }
      if(n_times_on_target >= 8)
      {
        set_led(0, target_color);
        set_led(4, target_color);
        targets_hit++;
        n_times_on_target = 0;
        background_color = foreground_color;
        foreground_color = target_colors[targets_hit];
        solved = (targets_hit == 3);
      }
    }
    else if(heading > (target+5) && heading <= (target+15))
    {
      n_times_on_target = 0;
      set_strip(background_color);
      set_led(3, foreground_color);
    }
    else if(heading > (target+15) && heading <= (target+25))
    {
      n_times_on_target = 0;
      set_strip(background_color);
      set_led(4, foreground_color);
    }
    else
    {
      n_times_on_target = 0;
      set_strip(background_color);
    }
  }
  else
  {
    set_strip(foreground_color);
  }

  vTaskDelay(MS(230));
  return solved;
}

bool combo_puzzle(bool &restart)
{
  static_assert(PASSCODE_LENGTH == strlen(PASSCODE));
  static bool solved = false;
  static String passcode = "";
  static String correct_passcode = PASSCODE;
  char digit;
  static int n_digits = 0;
  auto digit_read = xQueueReceive(keypad_queue_handle, &digit, 0);

  if(restart)
  {
    restart = false;
    passcode = "";
    n_digits = 0;
  }

  if(pdTRUE == digit_read && !solved)
  {
    if(n_digits == 0)
    {
      set_strip(OFF);
    }
    if(digit != '*' && digit != '#')
    {
      passcode += digit;
      if(digit == correct_passcode[n_digits])
      {
        set_led(n_digits, GREEN);
      }
      else if(correct_passcode.indexOf(digit) != -1)
      {
        set_led(n_digits, ORANGE);
      }
      else
      {
        set_led(n_digits, RED);
      }
      n_digits++;
    }
  }
  if(passcode.length() == PASSCODE_LENGTH && !solved)
  {
    if(passcode == correct_passcode)
    {
      solved = true;
    }
    else
    {
      passcode = "";
      n_digits = 0;
    }
  }
  if(solved)
  {
    set_strip(GREEN);
  }
  return solved;
}

bool main_puzzle(void)
{
  static char selected_puzzle = '\0';
  static char prev_selected_puzzle = 'F';

  static bool restart_puzzle;

  selected_puzzle = get_selected_puzzle();
  bool all_solved = solved_state[0] && solved_state[1] && solved_state[2] && solved_state[3];
  int n_solved = 0;

  for(auto i = 0; i<4; i++)
  {
    if(solved_state[i])
      n_solved++;
  }

  if(!all_solved)
  {
    switch(selected_puzzle)
    {
      case '\0':
        if(prev_selected_puzzle != selected_puzzle)
        {
          add_to_led_sequence(4, OFF, 500);
          add_to_led_sequence(4, CYAN, 500);
          add_to_led_sequence(3, OFF, 1000);
          add_to_led_sequence(3, CYAN, 500);
          add_to_led_sequence(2, OFF, 1500);
          add_to_led_sequence(2, CYAN, 500);
          add_to_led_sequence(1, OFF, 2000);
          add_to_led_sequence(1, CYAN, 500);
          add_to_led_sequence(0, OFF, 2500);
          add_to_led_sequence(0, CYAN, 500);
          vTaskDelay(MS(2500));
          clear_oled_body();
          set_oled_header("READY?");
          set_oled_body(1, "> Solve puzzles A-D");
          set_oled_body(2, "  to unlock.");
          set_oled_body(3, "> Select a letter");
          set_oled_body(4, "  to begin.");
          set_oled_body(5, "> '*' to view status.");
        }
        prev_selected_puzzle = selected_puzzle;
        break;
      case 'A':
        if(prev_selected_puzzle != selected_puzzle)
        {
          set_strip(OFF);
          clear_oled_body();
          set_oled_header("A:sw_seq");
          set_oled_body(1, "<INPUT> SWITCH ARRAY");
          set_oled_body(2, "<OBJECTIVE> 5 GREEN");
          if(solved_state[0])
          {
            set_oled_body(5, "<< ==== DONE! ==== >>");
          }
        }
        if(switch_puzzle() && !solved_state[0])
        {
          solved_state[0] = true;
          prev_selected_puzzle = 'F';
        }
        else
        {
          prev_selected_puzzle = selected_puzzle;
        }
        break;
      case 'B':
        if(prev_selected_puzzle != selected_puzzle)
        {
          set_strip(OFF);
          clear_oled_body();
          set_oled_header("B:skutnik");
          set_oled_body(1, "<INPUT> 1-5");
          set_oled_body(2, "<OBJECTIVE> 5 GREEN");
          if(solved_state[1])
          {
            set_oled_body(5, "<< ==== DONE! ==== >>");
          }
        }
        if(skutnik_puzzle() && !solved_state[1])
        {
          solved_state[1] = true;
          prev_selected_puzzle = 'F';
        }
        else
        {
          prev_selected_puzzle = selected_puzzle;
        }
        break;
      case 'C':
        if(prev_selected_puzzle != selected_puzzle)
        {
          set_strip(OFF);
          clear_oled_body();
          set_oled_header("C:KOMPAS");
          set_oled_body(1, "<INPUT> ???");
          set_oled_body(2, "<OBJECTIVE> 5Y 5B 5G");
          restart_puzzle = true;
          if(solved_state[2])
          {
            set_oled_body(5, "<< ==== DONE! ==== >>");
          }
        }
        if(compass_puzzle(restart_puzzle) && !solved_state[2])
        {
          solved_state[2] = true;
          prev_selected_puzzle = 'F';
        }
        else
        {
          prev_selected_puzzle = selected_puzzle;
        }
        break;
      case 'D':
        if(prev_selected_puzzle != selected_puzzle)
        {
          set_strip(OFF);
          clear_oled_body();
          set_oled_header("D:cod-le");
          set_oled_body(1, "<INPUT> 0-9");
          set_oled_body(2, "<OBJECTIVE> 5-digit");
          set_oled_body(3, "  passcode");
          restart_puzzle = true;
          if(solved_state[3])
          {
            set_oled_body(5, "<< ==== DONE! ==== >>");
          }
        }
        if(combo_puzzle(restart_puzzle) && !solved_state[3])
        {
          solved_state[3] = true;
          prev_selected_puzzle = 'F';
        }
        else
        {
          prev_selected_puzzle = selected_puzzle;
        }
        break;
      case '*':
        if(prev_selected_puzzle != selected_puzzle)
          {
            set_strip(OFF);
            clear_oled_body();
            set_oled_header("STATUS");
            char str[23];
            sprintf(str, "<SOLVED> %i/4", n_solved);
            set_oled_body(1, str);
            set_oled_body(2, "<LOCK> ENGAGED");
            restart_puzzle = true;
          }
        prev_selected_puzzle = selected_puzzle;
        break;
    }
  }
  return all_solved;
}