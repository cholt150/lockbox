#include "oled_task.h"

void oled_task(void *pvParameters)
{
  Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET);

  char log_buffer[8][21] = {0};

  Serial.println("Display object created");

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;) // Don't proceed, loop forever
    {
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
  }

  display.display();
  delay(1000);

  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(1);     // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  int count;
  display.clearDisplay();
  for(auto row = 0; row<=54; row+=9)
  {
    display.setCursor(0, row);
    display.print("XXXXXXXXXXXXXXXXXXXXX");
  }
  display.display();
  while(1)
  {
    // display.setCursor(OLED_WIDTH/2,OLED_HEIGHT/2);
    // display.clearDisplay();
    // display.print(count++);
    // display.display();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}