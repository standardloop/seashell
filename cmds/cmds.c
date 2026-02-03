#include "./cmds.h"

extern SeashellFunction *FunctionStringToFunction(char *function_name)
{
    char *seashell_functions_list[] = {
        "exit",
        "cd",
        "env",
        "help",
        "pwd",
        "ls",
    };
    const size_t seashell_functions_list_length = sizeof(seashell_functions_list) / sizeof(seashell_functions_list[0]);
    for (size_t i = 0; i < seashell_functions_list_length; i++)
    {
        if (strcmp(seashell_functions_list[i], function_name) == 0)
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
            case 5:
                return SeashellLS;
            }
        }
    }
    return NULL;
}
