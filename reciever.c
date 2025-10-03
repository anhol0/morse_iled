#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>

#define MAX_BUF 1024

#define CONTROL_LED "/sys/class/leds/tpacpi::lid_logo_dot/brightness"

int main() {
        int fd;
        char *myfifo = "/tmp/fifo";
        char buf[MAX_BUF];

        mkfifo(myfifo, 0666);

        fd = open(myfifo, O_RDONLY);
        
        read(fd, buf, MAX_BUF);
        printf("Recieved: %s\n", buf);
        close(fd);

        return 0;
}