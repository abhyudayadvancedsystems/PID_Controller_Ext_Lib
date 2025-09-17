#include <Arduino.h>
#include "lv_conf.h"       // Project-local LVGL configuration
#include <lvgl.h>
#include "M5Unified.h"
#include "CanRPM.h"
#include "M5DAC2.h"
#include "PIDController.h"
#include "DisplayManager.h"

// Global objects
CanBusRPM canBus;
M5DAC2 dac;
PIDController pid;
DisplayManager display;

void setup() {
  Serial.begin(115200);

  // Initialize M5Stack
  M5.begin();
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.setRotation(1);

  // Initialize modules
  canBus.begin();
  dac.begin();
  pid.begin();
  display.begin();
}

void loop() {
  M5.update();

  // 1. Read RPM from CAN
  canBus.update();

  // 2. Update PID with measured RPM
  pid.update(canBus.getRPM());

  // 3. Set DAC output
  dac.setVoltage(pid.getOutput());

  // 4. Update display
  display.update(canBus.getRPM(), pid.getSetpoint(), dac.getOutput());

  // 5. LVGL task handler (needed for animations & screens)
  lv_timer_handler();
  delay(5);
}
