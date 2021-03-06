#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET    -1 // This display does not have a reset pin accessible
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define FAN_CTRL PB1

#define FAN_PWM PA_0

#define FAN1_RPM PA12
#define FAN2_RPM PA15
#define FAN3_RPM PB3
#define FAN4_RPM PB4

// A little faster than 1 second so the display updates more often.
// Don't set it too fast or the PWM will be reset too often.
constexpr uint32_t LOOP_TIME_MS = 500;

// The fan ticks 2 times per revolution.
constexpr uint32_t RPM_SCALAR =  1000 / LOOP_TIME_MS / 2 * 60;

// ==== GLOBAL VARIABLES =====
uint32_t prevLoopTime_ms = 0;

uint32_t fan1_tick = 0;
uint32_t fan2_tick = 0;
uint32_t fan3_tick = 0;
uint32_t fan4_tick = 0;


uint32_t fan1_RPM = 0;
uint32_t fan2_RPM = 0;
uint32_t fan3_RPM = 0;
uint32_t fan4_RPM = 0;

uint32_t fan_speed_setpoint = 0;
uint32_t fan_PWM_setpoint = 0;

// ====== HELPER FUNCTIONS ======

void update_display(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("*** Fan Speed RPM ***");
  display.println("Fan 1: " + String(fan1_RPM));
  display.println("Fan 2: " + String(fan2_RPM));
  display.println("Fan 3: " + String(fan3_RPM));
  display.println("Fan 4: " + String(fan4_RPM));
  display.println();
  //display.println("Fan Setpoint (%): " + String(fan_speed_setpoint));
  display.println("Fan Setpoint (%): " + String(100-fan_PWM_setpoint));
  display.display();
}

void fan1_IRQ(){
  fan1_tick += 1;
}

void fan2_IRQ(){
  fan2_tick += 1;
}

void fan3_IRQ(){
  fan3_tick += 1;
}

void fan4_IRQ(){
  fan4_tick += 1;
}

// === MAIN CODE ====

void setup() {
  //Initialize IRQ for fan RPM measurements
  pinMode(FAN1_RPM, INPUT_PULLUP);
  pinMode(FAN2_RPM, INPUT_PULLUP);
  pinMode(FAN3_RPM, INPUT_PULLUP);
  pinMode(FAN4_RPM, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(FAN1_RPM), fan1_IRQ, RISING);
  attachInterrupt(digitalPinToInterrupt(FAN2_RPM), fan2_IRQ, RISING);
  attachInterrupt(digitalPinToInterrupt(FAN3_RPM), fan3_IRQ, RISING);
  attachInterrupt(digitalPinToInterrupt(FAN4_RPM), fan4_IRQ, RISING);

  //Initialize PWM for fan control
  pinMode(FAN_PWM, OUTPUT);

  //Initialize ADC for fan RPM setpoint
  pinMode(FAN_CTRL, INPUT);

  //Initialize OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  delay(2000);
  update_display();
}

void loop() {
  //Update RPM setpoint value
  fan_speed_setpoint = analogRead(FAN_CTRL);
  fan_PWM_setpoint = (float)fan_speed_setpoint * 100.0 / 1023.0;

  if (fan_PWM_setpoint > 98) {
    fan_PWM_setpoint = 100;
  }
  
  if (fan_PWM_setpoint < 10) {
    fan_PWM_setpoint = 0;
  }

  //Update PWM output
  pwm_start(FAN_PWM, 25000, fan_PWM_setpoint, 
            TimerCompareFormat_t::PERCENT_COMPARE_FORMAT);

  //Calculate fan RPM 1-3
  fan1_RPM = fan1_tick * RPM_SCALAR;
  fan2_RPM = fan2_tick * RPM_SCALAR;
  fan3_RPM = fan3_tick * RPM_SCALAR;
  fan4_RPM = fan4_tick * RPM_SCALAR;

  //Reset fan ticks for this time period.
  fan1_tick = 0;
  fan2_tick = 0;
  fan3_tick = 0;
  fan4_tick = 0;

  //Update display
  update_display();

  //Wait time
  while (millis() - prevLoopTime_ms < LOOP_TIME_MS){}
  prevLoopTime_ms = millis();
}