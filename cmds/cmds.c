#include "./cmds.h"

extern SeashellFunction *FunctionStringToFunction(char *function_name)
{
    const int seashellFunctionListLength = 5;
    char *seashellFunctionList[] = {
        "exit",
        "cd",
        "env",
        "help",
        "pwd",
    };
    for (int i = 0; i < seashellFunctionListLength; i++)
    {
        if (strcmp(seashellFunctionList[i], function_name) == 0)
        {
            switch (i)
            {
            case 0:
                return SeashellExit;
            case 1:
                return SeashellCD;
            case 2:
                return NULL;
            case 3:
                return SeashellHelp;
            case 4:
                return SeashellPWD;
            }
        }
    }
    return NULL;
}
