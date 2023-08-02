#ifndef OLED_TASK_H
#define OLED_TASK_H

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <list>

#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDRESS 0x3C

#define N_TEXT_ROWS 7
#define N_CHARS 21

void oled_task(void *);

#endif