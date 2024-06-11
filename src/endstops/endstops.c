#include "main.c"
#include "src/steppers/steppers.c"

void moveIfEndstopTriggered(int tableSize, char axis, int* currentCoord, bool* endstop) {
    move(tableSize * tableSize, axis);
    if (*endstop) {
        *currentCoord = 0;
    }
}

bool isTriggered(char endstopName) {
    int buttonPin = 0[endstopName];
    
    return digitalRead(buttonPin) == HIGH;
}

void stopIfEndstopTriggered(char axis, bool endstop) {
    if (endstop) {
        stop(axis);
    }
}

int main() {
    if (ENDSTOP_SYS_ON == true) {
        moveIfEndstopTriggered(TABLE_SIZE_X, 'X', &CURRENT_COORD_X, &ENDSTOP_X);
        moveIfEndstopTriggered(TABLE_SIZE_Y, 'Y', &CURRENT_COORD_Y, &ENDSTOP_Y);
        moveIfEndstopTriggered(TABLE_SIZE_Z, 'Z', &CURRENT_COORD_Z, &ENDSTOP_Z);
    } else {
        stopIfEndstopTriggered("X", ENDSTOP_X);
        stopIfEndstopTriggered("Y", ENDSTOP_Y);
        stopIfEndstopTriggered("Z", ENDSTOP_Z);
    }
}