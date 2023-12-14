#include "lockbox_common.h"
#include "switches.h"

void switches_init(void)
{
  // Switch 1
  gpio_pad_select_gpio(SWITCH_1_PIN);
  gpio_set_direction(SWITCH_1_PIN, GPIO_MODE_INPUT);
  gpio_pullup_en(SWITCH_1_PIN);

  // Switch 2
  gpio_pad_select_gpio(SWITCH_2_PIN);
  gpio_set_direction(SWITCH_2_PIN, GPIO_MODE_INPUT);
  gpio_pullup_en(SWITCH_2_PIN);

  // Switch 3
  gpio_pad_select_gpio(SWITCH_3_PIN);
  gpio_set_direction(SWITCH_3_PIN, GPIO_MODE_INPUT);
  gpio_pullup_en(SWITCH_3_PIN);

  // Switch 4
  gpio_pad_select_gpio(SWITCH_4_PIN);
  gpio_set_direction(SWITCH_4_PIN, GPIO_MODE_INPUT);
  gpio_pullup_en(SWITCH_4_PIN);

  // Switch 5
  gpio_pad_select_gpio(SWITCH_5_PIN);
  gpio_set_direction(SWITCH_5_PIN, GPIO_MODE_INPUT);
  gpio_pullup_en(SWITCH_5_PIN);
}

uint16_t read_switches(void)
{
  uint16_t result = 0;
  static uint16_t last_result = 0;
  // Switches use active-low logic, so invert the gpio read.
  result |= (!gpio_get_level(SWITCH_1_PIN)) << SWITCH_1_BIT_LOCATION;
  result |= (!gpio_get_level(SWITCH_2_PIN)) << SWITCH_2_BIT_LOCATION;
  result |= (!gpio_get_level(SWITCH_3_PIN)) << SWITCH_3_BIT_LOCATION;
  result |= (!gpio_get_level(SWITCH_4_PIN)) << SWITCH_4_BIT_LOCATION;
  result |= (!gpio_get_level(SWITCH_5_PIN)) << SWITCH_5_BIT_LOCATION;

  if(last_result != result)
  {
    last_result = result;
    Serial.println(result);
  }
  return result;
}