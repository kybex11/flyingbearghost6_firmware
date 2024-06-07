#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#define FBDEV "/dev/fb1"

int main() {
    int fbfd = 0;
    struct fb_var_screeninfo vinfo;
    uint16_t *fbp;

    fbfd = open(FBDEV, O_RDWR);
    if (fbfd == -1) {
        perror("Error: cannot open framebuffer device");
        return 1;
    }

    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
        perror("Error reading variable information");
        close(fbfd);
        return 1;
    }

    fbp = (uint16_t *)mmap(0, vinfo.yres_virtual * vinfo.xres_virtual * 2, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if ((int)fbp == -1) {
        perror("Error: failed to map framebuffer device to memory");
        close(fbfd);
        return 1;
    }

    // Draw "Hello World" text on the screen
    int x = 100, y = 100;
    uint16_t color = 0xF800; // Red color
    for (int i = 0; i < 11; i++) {
        fbp[(y + 0) * vinfo.xres_virtual + x + i] = color;
    }

    munmap(fbp, vinfo.yres_virtual * vinfo.xres_virtual * 2);
    close(fbfd);

    return 0;
}