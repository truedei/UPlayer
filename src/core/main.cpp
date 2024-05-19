#include "qt/App.h"
#include "stdio.h"

int main(int argc, char *argv[])
{
    //printf log
    setvbuf(stdout, 0, _IONBF, 0);

    return run_main(argc, argv);
}
