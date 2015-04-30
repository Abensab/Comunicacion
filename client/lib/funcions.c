#include "./../include/funcions.h"

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

void error(const char *msg) {
    perror(msg);
    exit(1);
}

long long current_timestamp() {
    struct      timeval te;
    long long   milliseconds;

    gettimeofday(&te, NULL); // get current time
    milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // caculate milliseconds

    return milliseconds;
}

int changeFlag(int flag){
    if(flag)
        return 0;
    else
        return 1;
}

long long timeToStartInSeconds(int s){
    return current_timestamp() + (s*1000);
}

long long timeToStartInMilisecons(int m){
    return current_timestamp() + (m);
}

void* playSuperWav(void *arguments) {

    Arg_thread *args = (Arg_thread *) arguments;
    /*printf("Time to start: %lld\n", args->timeToStart);
    printf("Flag: %d\n", args->flag);
    printf("Finish playing: %d\n", args->finishPlaying);*/

    int sleepTime = ( (args->timeToStart - current_timestamp())/1000 ) -1;

    printf("Sleep Time: %d\n",sleepTime);

    while (TRUE){
        sleep(sleepTime);
        if( (args->timeToStart - current_timestamp()) <= 0 ) {
            superWav(&(args->flag),&(args->finishPlaying));
            break;
        }/*else esperando*/
    }
    return NULL;
}
