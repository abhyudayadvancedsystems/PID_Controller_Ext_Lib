#include <Arduino.h>
#include "lv_conf.h"       // Project-local LVGL configuration
#include <lvgl.h>
#include "M5Unified.h"
#include "CanRPM.h"
#include "M5DAC2.h"
#include "PIDController.h"
#include "DisplayManager.h"
#include <DFRobot_GP8XXX.h>

// ---------- Global variables ----------
DFRobot_GP8XXX_IIC dac_i2c;
M5DAC2 dac(&dac_i2c);  // M5DAC2 expects a pointer to the IIC DAC

double rpm_input = 0;
double dac_output = 0;
double rpm_setpoint = INITIAL_TARGET_RPM;
PIDController pid(&rpm_input, &dac_output, &rpm_setpoint);

CanRPM canBus(CAN_RPM_ID); // CAN ID defined in CanRPM.h
DisplayManager display;

void setup() {
  Serial.begin(115200);

  // Initialize M5Stack
  auto cfg = M5.config();
  M5.begin(cfg);
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.setRotation(1);

  // Initialize modules
  dac.begin();
  pid.begin();
  display.begin();

  // No explicit begin() for CanRPM, assuming constructor does setup
}

void loop() {
  M5.update();

  // 1. Read RPM from CAN
  bool commOk = canBus.update();
  if (commOk) {
    rpm_input = (double)canBus.rpm();
  }

  // 2. PID computation
  pid.update();

  // 3. Set DAC output (raw, both channels to same value for demo)
  dac.setVoltageRaw((uint16_t)dac_output, (uint16_t)dac_output);

  // 4. Update display with current RPM, DAC output, and autotune status
  display.update(rpm_input, dac_output, pid.isAutoTuneFinished());

  // 5. LVGL task handler
  lv_timer_handler();
  delay(5);
}