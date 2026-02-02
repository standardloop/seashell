#include "./cmds.h"

extern SeashellFunction *FunctionStringToFunction(char *function_name)
{
    const int seashellFunctionListLength = 4;
    char *seashellFunctionList[] = {
        "exit",
        "cd",
        "env",
        "help",
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
                return NULL;
            case 2:
                return NULL;
            case 3:
                return SeashellHelp;
            }
        }
    }
    return NULL;
}
