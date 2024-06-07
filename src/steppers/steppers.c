#include <AccelStepper.h>

AccelStepper stepper(1, STEP_PIN, DIR_PIN)

void setup()
{
    stepper.setMaxSpeed(STEP_MAX_SPEED);
    stepper.setAcceleration(STEP_ACCELERATION);
}

void move(float cord, char stepper_name) {
    stepper[stepper_name].move(cord);
    stepper[stepper_name].runSpeedToPosition();
}

void stop(char stepper_name) {
    stepper[stepper_name].stop()
}