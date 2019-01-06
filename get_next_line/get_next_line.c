/*
** EPITECH PROJECT, 2019
** CPE_getnextline_2018
** File description:
** main
*/
 
#include "get_next_line.h"
#ifndef READ_SIZE
    #define READ_SIZE 512
#endif /*READ_SIZE*/
 
#define BUFFER_SIZE 512
void copy_buffer(char *new_buffer, char *buffer, int length)
{
    int index = 0;
    while (index < length){
        new_buffer[index] = buffer[index];
        index++;
    }
}

int my_read(int fd, char *dest){
    static char buffer[READ_SIZE];
    static int index = 0;
    static int length = 0;

    if (index == length){
        length = read(fd, buffer, READ_SIZE);
        if (length <= 0){
            return (length);
        }
        index = 0;
    }
    *dest = buffer[index++];
    return 1;
}

char *get_next_line(int fd)
{
    int read_return_value = 0;
    int index = 0;
    char c;
    int length = BUFFER_SIZE;
    char *buffer = malloc(length+1);
    if (!buffer){
        return NULL;
    }
    while ((read_return_value = my_read(fd, &c) > 0) && c != '\n'){
        buffer[index++] = c;
        if (index == length){
            length += BUFFER_SIZE;
            char *new_buffer = malloc(length + 1);
            if (!new_buffer){
                return NULL;
            }
            copy_buffer(new_buffer, buffer, index);
            free(buffer);
            buffer = new_buffer;
        }
    }
    if (index != 0 && read_return_value >= 0){
        buffer[index] = 0;
        return (buffer);
    }
    free(buffer);
    return NULL;
}
