#include "alphaos.h"

extern int main(int argc, char const *argv[]);

void c_start()
{
    struct process_arguments args;
    __process_get_arguments(&args);

    int res = main(args.argc, (const char **)args.argv);

    if (res == 0) {}
}
