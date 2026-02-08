#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include "./cmds.h"

// TODO
// flags/arguments
// - hidden files
// - stats
// - etc
// colors
extern int SeashellLS(StringArr *args)
{
    if (args == NULL)
    {
        return 1;
    }
    DIR *mydir;
    struct dirent *myfile;
    // struct stat mystat;

    char buf[512];
    mydir = opendir("."); // TODO support arguments
    while ((myfile = readdir(mydir)) != NULL)
    {
        snprintf(buf, sizeof(buf), "%s/%s", ".", myfile->d_name);
        // stat(buf, &mystat);
        // printf("%zu", mystat.st_size);
        printf(" %s ", myfile->d_name);
    }
    printf("\n");
    closedir(mydir);
    return 0;
}
