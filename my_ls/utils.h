#ifndef _UTILS_H
#define _UTILS_H

#include <sys/types.h>

extern int my_optind;
extern char *place;

int my_getopt(int argc, char *const *argv, const char *options);
char *my_strchr(char *s, int c);
char *my_strcat(char *s1, char *s2);
int my_strcmp(const char *s1, const char *s2);
void *my_memcpy(void *dest, const void *src, size_t n);
size_t my_strlen(const char *s);
char *my_strdup(const char *s1);

#endif  // _UTILS_H