#pragma once

#define C1_PIN GPIO_NUM_14
#define C2_PIN GPIO_NUM_27
#define C3_PIN GPIO_NUM_26
#define C4_PIN GPIO_NUM_25

#define R1_PIN GPIO_NUM_33
#define R2_PIN GPIO_NUM_32
#define R3_PIN GPIO_NUM_35
#define R4_PIN GPIO_NUM_34

// Keypad poll rate milliseconds
#define KEYPAD_POLL_RATE 20

// Keypad debounce time ms
#define KEYPAD_DEBOUNCE 10

// Number of keypad columns
#define KEYPAD_COLUMNS 4

// Number of keypad rows
#define KEYPAD_ROWS 4

typedef enum
{
  FALLING_EDGE,
  RISING_EDGE,
  NO_EDGE
} Edge_Type;

void keypad_init(void);

char get_selected_puzzle(void);

void keypad_task(void *pvParams);