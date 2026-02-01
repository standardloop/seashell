#include <stdio.h>
#include <stdlib.h>
#include <standardloop/logger.h>
#include <standardloop/util.h>

int main(void)
{
    SetLogLevel(StringToLogLevel(GetEnv("LOG_LEVEL", "TRACE")));
    Log(TRACE, "Hello World!");
    return EXIT_SUCCESS;
}
