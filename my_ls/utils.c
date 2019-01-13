#include "utils.h"
#include "stddef.h"

void *my_memcpy(void *dest, const void *src, size_t n) {
    /* Typecast src and dest addresses to (char *) */
    char *csrc = (char *)src;
    char *cdest = (char *)dest;
    int index = 0;

    /* Copy contents of src[] to dest[] */
    while (index < n) {
        *cdest++ = *csrc++;
        index++;
    }
    return dest;
}

int my_strcmp(const char *s1, const char *s2) {
    while (*s1) {
        if (!*s2) {
            /* s1 is greater than s2 */
            return 1;
        }
        if (*s1 == *s2) {
            s1++;
            s2++;
            continue;
        }
        return *s1 > *s2 ? 1 : -1;
    }
    return 0;
}

char *my_strcat(char *s1, char *s2) {
    /* go to end of s1 */
    char *ptr = s1;
    while (*ptr) {
        ptr++;
    };
    while (*s2) {
        *ptr++ = *s2++;
    }
    *ptr = 0;
    return (s1);
}

char *my_strchr(char *s, int c) {
    while (*s) {
        if (*s == c) {
            return s;
        }
        s++;
    }
    return (NULL);
}

int my_optind = 0;
char *place = "";

int my_getopt(int argc, char *const *argv, const char *options) {
    if (options == NULL) {
        return (-1);
    }

    if (!*place) {
        my_optind++;
        if (my_optind >= argc) {
            return (-1);
        }
        if (*argv[my_optind] != '-') {
            return (-1);
        }
        place = argv[my_optind] + 1;
    }

    int value = *place;
    place++;
    return (value);
}

size_t my_strlen(const char *s) {
    size_t length = 0;
    while (s[length] != 0) {
        length++;
    }
    return length;
}

char *my_strdup(const char *s1) {
    size_t length = my_strlen(s1);
    char *new_str = malloc(length + 1);
    if (new_str == NULL) {
        return NULL;
    }
    int index = 0;
    while (index <= length) {
        new_str[index] = s1[index];
        index++;
    }
    return new_str;
}
