#include "config.h"
#include "fuse.h"
#include "settings.h"
#include "z80/z80.h"
#include "debugger/debugger.h"
#include "machine.h"
#include "sound.h"
#include "timer/timer.h"
#include "event.h"
#include "unittests/unittests.h"

#include <stdio.h>
#include <unistd.h>
#include <termios.h>


char getch(int block) {
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0)
            perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = block;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
            perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
            perror ("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
            perror ("tcsetattr ~ICANON");
    return (buf);
}
