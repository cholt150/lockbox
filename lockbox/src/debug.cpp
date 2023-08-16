#include "debug.h"

void oled_debug(int n)
{
  char string[21];
  sprintf(string, "%i", n);
  xQueueSendToBack(oled_msg_queue, string, portMAX_DELAY);
}

void oled_debug(char *s)
{
  xQueueSendToBack(oled_msg_queue, s, portMAX_DELAY);
}