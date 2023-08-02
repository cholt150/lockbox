#include "oled_task.h"

int oled_row_locations[N_TEXT_ROWS] = {
    0, 9, 18, 27, 36, 45, 54
};

void oled_task(void *pvParameters)
{
  QueueHandle_t msg_queue;

  msg_queue = *((QueueHandle_t *)pvParameters);

  Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET);

  char log_buffer[8][21] = {0};

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
  }

  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(1);     // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  int count;
  uint8_t curr_row = 0;
  uint8_t start_row = 1;
  
  while(1)
  {
    // block until we get a string to show
    xQueueReceive(msg_queue, log_buffer[curr_row++], portMAX_DELAY);
    curr_row %= N_TEXT_ROWS;

    uint8_t row_to_print = start_row++;
    start_row %= N_TEXT_ROWS;

    display.clearDisplay();
    for(auto i = 0; i < N_TEXT_ROWS; i++)
    {
      display.setCursor(0, i*9);
      display.print(log_buffer[row_to_print++]);
      row_to_print %= N_TEXT_ROWS;
    }
    display.display();
  }
}