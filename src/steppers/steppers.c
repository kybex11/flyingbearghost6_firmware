#include <AccelStepper.h>

AccelStepper stepper(1, STEP_PIN, DIR_PIN)

void setup()
{
    stepper.setMaxSpeed(STEP_MAX_SPEED);
    stepper.setAcceleration(STEP_ACCELERATION);
}

void move(float cord) {
    stepper.move(cord);
    stepper.runSpeedToPosition();
}

void stop() {
    stepper.stop()
}