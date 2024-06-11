#include "src/steppers/steppers.c"

int main() {
    float resonanceStress;

    for (int i = 0; i < 4000) {
        resonanceStress++;

        move(resonanceStress * resonanceStress - resonanceStress , "x");
        move(resonanceStress * resonanceStress - resonanceStress , "y");
        move(resonanceStress * resonanceStress - resonanceStress , "z");
    
    }
}