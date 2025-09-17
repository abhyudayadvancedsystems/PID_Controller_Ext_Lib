#include "PIDController.h"

PIDController::PIDController(double* input_, double* output_, double* setpoint_)
  : input(input_), output(output_), setpoint(setpoint_),
    pid(input_, output_, setpoint_, 0.1, 0.05, 0.0, DIRECT),  // Initial guesses
    tuner(input_, output_) {}

void PIDController::begin() {
    pid.SetMode(AUTOMATIC);
    pid.SetOutputLimits(0, 5000); // mV range for DAC
}

void PIDController::update() {
    pid.Compute();
}

void PIDController::startAutoTune() {
    tuner.SetOutputStep(50);    // mV step for autotune
    tuner.SetControlType(1);    // PID control
    tuner.SetNoiseBand(5);      // RPM fluctuation tolerance
    autoTuneDone = false;
}

bool PIDController::isAutoTuneFinished() const {
    return autoTuneDone || tuner.Runtime(); // tuner.Runtime returns true when finished
}

double PIDController::getOutput() const {
    return *output;
}
