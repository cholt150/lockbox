#pragma once

#define SWITCHES_STATE_1 25
#define SWITCHES_STATE_2 24
#define SWITCHES_STATE_3 8
#define SWITCHES_STATE_4 12
#define SWITCHES_STATE_5 28

#define SKUTNIK_COMBO_1 0b10011
#define SKUTNIK_COMBO_2 0b01101
#define SKUTNIK_COMBO_3 0b10101
#define SKUTNIK_COMBO_4 0b11001
#define SKUTNIK_COMBO_5 0b10110

#define PASSCODE "41531"
#define PASSCODE_LENGTH 5

bool switch_puzzle(void);
bool main_puzzle(void);