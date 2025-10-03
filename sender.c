#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>


#define CONTROL_LED "/sys/class/leds/tpacpi::lid_logo_dot/brightness"

int main() {
        int fd;
        char *myfifo = "/tmp/fifo";
        mkfifo(myfifo, 0666);

        fd = open(myfifo, O_WRONLY);
        write(fd, "string", sizeof("string"));
        close(fd);

        unlink(myfifo);
        return 0;
}