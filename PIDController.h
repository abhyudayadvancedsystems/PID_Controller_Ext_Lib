#pragma once
#include <PID_v1.h>
#include <PID_AutoTune_v0.h>

class PIDController {
public:
    PIDController(double* input, double* output, double* setpoint);
    void begin();
    void update();
    void startAutoTune();
    bool isAutoTuneFinished(); // <-- removed 'const' here
    double getOutput() const;

private:
    double* input;
    double* output;
    double* setpoint;
    PID pid;
    PID_ATune tuner;
    bool autoTuneDone = false;
};