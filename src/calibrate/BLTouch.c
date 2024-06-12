#include "main.c"
#include "src/steppers/steppers.c"
#include "src/endstops/endstops.c"

int main() {
    if (USE_BL_TOUCH) {
        analogWrite(BL_TOUCH_PIN_PWRD, 5.5);
    }
}