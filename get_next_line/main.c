#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#include "get_next_line.h"

int main(int argc, char **argv)
{
    int fd = open("test", O_RDONLY);
    char *s = get_next_line(fd);

    while (s){
        printf("%s\n", s);
        free(s);
        s = get_next_line(fd);
    }
    return (0);
}
