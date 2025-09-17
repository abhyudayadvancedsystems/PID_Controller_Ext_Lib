#ifndef CANRPM_H
#define CANRPM_H

#include <Arduino.h>
#include <M5Unified.h>
#include "PIDController.h"
#include "M5DAC2.h"
#include "DisplayManager.h"

// ---------- Constants ----------
constexpr uint32_t CAN_RPM_ID = 0x106;    // CAN ID for RPM
constexpr float INITIAL_TARGET_RPM = 750.0f; // Idle RPM
constexpr float TARGET_STEP = 5.0f;
constexpr float TARGET_MIN  = 400.0f;
constexpr float TARGET_MAX  = 6000.0f;

// Soft ramp parameters
constexpr float SOFT_RAMP_STEP = 0.5f; // increment in volts per loop

// ---------- Objects ----------
CanRPM rpmReader(CAN_RPM_ID);
M5DAC2 dac;                 // Handles CH0/CH1 outputs
PIDController pid;          // PID + autotune
DisplayManager display;     // LVGL display manager

// ---------- Runtime Variables ----------
float targetRPM = INITIAL_TARGET_RPM;
float rpmFilt = 0.0f;
const float rpmAlpha = 0.2f;  // EMA filter
float dacVoltage = 0.0f;       // Current DAC voltage
bool autotuneDone = false;

void setup() {
    Serial.begin(115200);
    auto cfg = M5.config();
    M5.begin(cfg);
    M5.Lcd.setRotation(1);
    M5.Lcd.fillScreen(TFT_BLACK);

    // Init DAC
    dac.begin();
    dac.setFromCH1Voltage(0.36f); // Initial safe voltage

    // Init PID
    pid.begin(0.0003, 0.0001, 0.0); // conservative starting Kp, Ki, Kd
    pid.setOutputLimits(dac.CH1_MIN_VOLT, dac.CH1_MAX_VOLT);

    // Init display
    display.begin();
    display.showMessage("Initializing...");
}

void loop() {
    M5.update();

    // Adjust target RPM via buttons
    if (M5.BtnA.wasPressed()) targetRPM = min(TARGET_MAX, targetRPM + TARGET_STEP);
    if (M5.BtnB.wasPressed()) targetRPM = max(TARGET_MIN, targetRPM - TARGET_STEP);

    // Read RPM
    bool commOk = rpmReader.update();
    if (commOk) {
        float rpmMeas = float(rpmReader.rpm());
        rpmFilt = (1.0f - rpmAlpha) * rpmFilt + rpmAlpha * rpmMeas;
    }

    // Trigger autotune once when idle reached
    if (!autotuneDone && rpmFilt >= INITIAL_TARGET_RPM) {
        pid.runAutoTune(targetRPM);
        autotuneDone = true;
    }

    // PID calculation
    if (commOk) {
        float pidOut = pid.compute(targetRPM, rpmFilt);

        // Soft ramp
        if (pidOut > dacVoltage + SOFT_RAMP_STEP)
            dacVoltage += SOFT_RAMP_STEP;
        else if (pidOut < dacVoltage - SOFT_RAMP_STEP)
            dacVoltage -= SOFT_RAMP_STEP;
        else
            dacVoltage = pidOut;

        // Apply to DAC channels
        dac.setFromCH1Voltage(dacVoltage);
    }

    // Update LVGL display
    display.update(rpmFilt, dac.CH0_mv / 1000.0f, dac.CH1_mv / 1000.0f, targetRPM);

    delay(5);
}

#endif // CANRPM_H