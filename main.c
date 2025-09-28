#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    for (int i = 1; i < (argc); i++) {
        FILE* fd = fopen("/sys/class/leds/tpacpi::lid_logo_dot/brightness", "w");
        if(fd == NULL) {
            printf("Error opening file. Run program with sudo or check system configuration\n");
            return 1;
        }
        fwrite(argv[i], 1, sizeof(argv[i]), fd);
        fclose(fd);
        usleep(200000);
    }
    
}