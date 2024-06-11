#include "src/steppers/steppers.c"
#include "src/endstops/endstops.c"
#include "main.c"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>


int main() {
    int mem_fd;
    void *gpio_map;

    while(1) {
        if (isTriggered("z")) {
            stop("z");
            
            if ((mem_fd = open("dev/mem", O_RDWR|O_SYNC) ) < 0) {
                printf("Can't open /dev/mem \n");
                exit(-1);
            }

            gpio_map = mmap(
                NULL,
                4096,
                PROT_READ|PROT_WRITE,
                MAP_SHARED,
                mem_fd,
                GPIO_BASE
            );

            volatile unsigned *gpio = (volatile unsigned *)gpio_map;
            gpio[0] = 1;

            munmap(gpio_map, 4096);
            close(mem_fd);

            return 0;
        }
    }
}