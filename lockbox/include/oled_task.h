#ifndef OLED_TASK_H
#define OLED_TASK_H

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDRESS 0x3C

#define N_TEXT_ROWS 7
#define N_CHARS 21

// int oled_row_locations[N_TEXT_ROWS] = {
//     0, 9, 18, 27, 36, 45, 54
// };

void oled_task(void *);

#endif