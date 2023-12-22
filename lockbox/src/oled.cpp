#include "lockbox_common.h"
#include "oled.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <Wire.h>
#include <list>

int oled_row_locations[N_TEXT_ROWS] = {
    0, 9, 18, 27, 36, 45, 54
};

Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET);

auto header_queue = xQueueCreate(4,16);
char header[16] = {'\0'};

auto body_1_queue = xQueueCreate(4,23);
char body_1[23] = {'\0'};

auto body_2_queue = xQueueCreate(4,23);
char body_2[23] = {'\0'};

auto body_3_queue = xQueueCreate(4,23);
char body_3[23] = {'\0'};

auto body_4_queue = xQueueCreate(4,23);
char body_4[23] = {'\0'};

auto body_5_queue = xQueueCreate(4,23);
char body_5[23] = {'\0'};

void set_oled_header(char *str)
{
  if(strlen(str) < 16)
  {
    xQueueSendToBack(header_queue, str, MS(1));
  }
}

void set_oled_body(int line, char *str)
{
  if(strlen(str) < 23)
  {
    switch(line)
    {
      case 1:
        xQueueSendToBack(body_1_queue, str, MS(1));
        break;
      case 2:
        xQueueSendToBack(body_2_queue, str, MS(1));
        break;
      case 3:
        xQueueSendToBack(body_3_queue, str, MS(1));
        break;
      case 4:
        xQueueSendToBack(body_4_queue, str, MS(1));
        break;
      case 5:
        xQueueSendToBack(body_5_queue, str, MS(1));
        break;
      default:
        break;
    }
    
  }
}

void clear_oled_body(void)
{
  set_oled_body(1,"");
  set_oled_body(2,"");
  set_oled_body(3,"");
  set_oled_body(4,"");
  set_oled_body(5,"");
}

void oled_init()
{
  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
  }

  display.setTextSize(2);      // Normal 1:1 pixel scale
  display.setTextColor(1);     // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  display.clearDisplay();
  display.display();
}

void oled_task(void *pvParameters)
{
  QueueHandle_t msg_queue;

  msg_queue = *((QueueHandle_t *)pvParameters);

  char log_buffer[8][21] = {0};

  int count;
  uint8_t curr_row = 0;
  uint8_t start_row = 1;
  
  while(1)
  {
    auto new_header = xQueueReceive(header_queue, header, 0);
    auto new_body_1 = xQueueReceive(body_1_queue, body_1, 0);
    auto new_body_2 = xQueueReceive(body_2_queue, body_2, 0);
    auto new_body_3 = xQueueReceive(body_3_queue, body_3, 0);
    auto new_body_4 = xQueueReceive(body_4_queue, body_4, 0);
    auto new_body_5 = xQueueReceive(body_5_queue, body_5, 0);
    bool update_needed = new_header || new_body_1 || new_body_2 || new_body_3 || new_body_4 || new_body_5;
    if(update_needed)
    {
      display.clearDisplay();
      display.setCursor(0,0);
    }
    // Draw header
    display.setTextSize(2);
    display.println(header);
    display.drawLine(0, display.getCursorY()+1, OLED_WIDTH-1, display.getCursorY()+1,1);
    // Draw other content
    display.setTextSize(1);
    display.setCursor(0, display.getCursorY() + 4);
    display.println(body_1);
    display.println(body_2);
    display.println(body_3);
    display.println(body_4);
    display.println(body_5);
    if(update_needed)
    {
      display.display();
    }
    vTaskDelay(MS(100));
  }
}