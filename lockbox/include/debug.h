#ifndef DEBUG_H
#define DEBUG_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include <stdio.h>
#include <cstring>

#include "lockbox_common.h"

extern QueueHandle_t oled_msg_queue;

void oled_debug(int n);
void oled_debug(char *s);

#endif