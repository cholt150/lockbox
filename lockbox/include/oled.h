#pragma once

#include <Adafruit_SSD1306.h>

extern Adafruit_SSD1306 display;

#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDRESS 0x3C

#define N_TEXT_ROWS 7
#define N_CHARS 21

void oled_init();
void set_oled_header(char *str);
void set_oled_body(int line, char *str);
void clear_oled_body(void);
void oled_task(void *);
