#include <unistd.h>
#include "./cmds.h"

extern int SeashellCD(StringArr *args)
{
    if (args == NULL)
    {
        return -1;
    }
    else if (args->num_strings == 1)
    {
        printf("cd: not enough arguments\n");
        return -1;
    }
    else if (args->num_strings != 2)
    {
        printf("cd: too many arguments\n");
        return -1;
    }
    // TODO
    // get string length of new folder
    // get string length of current global pwd
    // write new folder into current global pwd and attempt to chdir
    // if it doesn't worked we need restore buffer.

    char current_pwd[PATH_MAX] = {};

    int global_pwd_iterator;
    for (global_pwd_iterator = 0; GLOBAL_pwd[global_pwd_iterator] != '\0' && global_pwd_iterator < PATH_MAX; global_pwd_iterator++)
    {
        current_pwd[global_pwd_iterator] = GLOBAL_pwd[global_pwd_iterator];
    }
    current_pwd[global_pwd_iterator] = '\0';

    int length_of_new_directory = strlen(args->strings[1]);
    int pwd_after_cd_len = global_pwd_iterator + length_of_new_directory;
    if (pwd_after_cd_len > PATH_MAX)
    {
        printf("cd: path larger than PATH_MAX value of %d\n", PATH_MAX);
        return 1;
    }

    current_pwd[global_pwd_iterator] = '/';
    global_pwd_iterator++;
    int new_full_path_iterator = global_pwd_iterator;

    for (int new_directory_itr = 0; new_directory_itr < length_of_new_directory; new_directory_itr++)
    {
        current_pwd[new_full_path_iterator] = args->strings[1][new_directory_itr];
        new_full_path_iterator++;
    }

    current_pwd[new_full_path_iterator] = '\0';
    printf("%s\n", current_pwd);

    // if (chdir(args->strings[1]) != 0)
    // {
    //     printf("cd: no such file or directory: %s\n", args->strings[1]);
    // }
    return 0;
}
