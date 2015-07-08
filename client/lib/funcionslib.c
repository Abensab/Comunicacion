#include "../include/functionlib.h"

long long current_timestamp() {
    struct      timeval te;
    long long   milliseconds;

    gettimeofday(&te, NULL); // get current time
    milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // caculate milliseconds

    return milliseconds;
}

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int kbhit(void)
{
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if(ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}

int getMaxInt(int a[], int size) {
    int c, max;
    max = a[0];
    for (c = 1; c < size; c++) {
        if (a[c] > max) {
            max = a[c];
        }
    }
    return max;
}

float getMaxFloat(float a[], int size) {
    int c;
    float max;
    max = a[0];
    for (c = 1; c < size; c++) {
        if (a[c] > max) {
            max = a[c];
        }
    }
    return max;
}