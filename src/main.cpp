#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // This display does not have a reset pin accessible
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

uint32_t prevLoopTime_ms = 0;


void update_display(){
  // Displays "Hello world!" on the screen
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("Hello world!");
  display.display();
}

void setup() {
  //Initialize IRQ for RPM measurements
  //Initialize PWM for fan control
  //Initialize ADC for RPM control


  //Initialize OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
 
  // Displays Adafruit logo by default. call clearDisplay immediately if you don't want this.
  display.display();
  delay(2000);
  update_display();
}

void loop() {
  //Update RPM measurements
  //Update PWM output
  //Read RPM 1-3
  //Update display
  update_display();
  
  //Wait time
  while (millis() - prevLoopTime_ms < 100){}
  prevLoopTime_ms = millis();
}