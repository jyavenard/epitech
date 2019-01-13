#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <uuid/uuid.h>
#include "utils.h"

#define NAME_MAX 255  /* # chars in a file name */
#define PATH_MAX 4096 /* # chars in a path name including nul */

static const char *lookup[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                               "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

enum Flags {
    kLongView = 1 << 0,
    kRecursive = 1 << 1,
    kReverseSort = 1 << 2,
    kSortedByTime = 1 << 4,
    kShowHidden = 1 << 5,
};

/* single linked-list */
/* the lifetime of a node is always within an opendir session, so we can use pointers with it. */
struct list_entry_node {
    const char *name;
    struct stat sb;
    struct list_entry_node *next_entry_node;
};

typedef struct list_entry_node list_entry_node;
typedef struct dirent dirent;

list_entry_node *insert_entry_before(list_entry_node *entries, list_entry_node *new_entry) {
    new_entry->next_entry_node = entries;
    return new_entry;
}

list_entry_node *insert_entry_after(list_entry_node *entries, list_entry_node *new_entry) {
    new_entry->next_entry_node = entries->next_entry_node;
    entries->next_entry_node = new_entry;
    return entries;
}

// TODO: Super ugly sorting, in O(N^2), linked list not the best storage for storing stuff.
// Using an array would perform much better.

list_entry_node *insert_entry_by_name(list_entry_node *entries, const char *name, struct stat *sb) {
    int compare = 0;
    list_entry_node *first = entries;
    list_entry_node *previous = NULL;
    list_entry_node *new_entry = malloc(sizeof(list_entry_node));
    new_entry->name = name;
    my_memcpy(&new_entry->sb, sb, sizeof(struct stat));
    new_entry->next_entry_node = NULL;

    if (!entries) {
        return new_entry;
    }

    while (entries) {
        compare = my_strcmp(name, entries->name);
        if (compare < 0) {
            break;
        }
        previous = entries;
        entries = entries->next_entry_node;
    }

    if (!entries) {
        // put it last
        previous->next_entry_node = new_entry;
    } else if (!previous) {
        // put it first.
        first = new_entry;
        new_entry->next_entry_node = entries;
    } else {
        new_entry->next_entry_node = entries;
        previous->next_entry_node = new_entry;
    }
    return first;
}

int count_nodes(list_entry_node *entries) {
    int counter = 0;
    while (entries) {
        counter++;
        entries = entries->next_entry_node;
    }
    return counter;
}

// free all nodes of the linked list
void cleanup_list_nodes(list_entry_node *entries) {
    while (entries) {
        list_entry_node *next_node = entries->next_entry_node;
        free(entries);
        entries = next_node;
    };
}

char *my_group_from_gid(gid_t gid) {
    struct group *grgid = getgrgid(gid);
    if (grgid == NULL) {
        return ""; /* can't happen here */
    }
    return (grgid->gr_name);
}

char *my_user_from_uid(uid_t uid) {
    struct passwd *pwuid = getpwuid(uid);
    if (pwuid == NULL) {
        return ""; /* can't happen here */
    }
    return (pwuid->pw_name);
}

int set_flags(int argc, char *const *argv) {
    int flags = 0;
    int ch;
    while ((ch = my_getopt(argc, argv, "lRdrt")) != -1) {
        switch (ch) {
            case 'l':
                flags |= kLongView;
                break;
            case 'R':
                flags |= kRecursive;
                break;
            /* The -d option turns off the -R option. */
            case 'd':
                flags &= ~kRecursive;
                break;
            case 'r':
                flags |= kReverseSort;
                break;
            case 't':
                flags |= kSortedByTime;
                break;
            default:
                printf("ls: illegal option -- %c\n", ch);
                return (-1);
        }
    }
    return (flags);
}

void display_contents(const char *name, const struct stat *sb, int flags) {
    struct tm *t;
    char link_read[PATH_MAX] = {0};
    ssize_t bytes_read;

    if (flags & kLongView) {
        printf("%c",
               S_ISDIR(sb->st_mode)
                   ? 'd'
                   : S_ISFIFO(sb->st_mode) ? 'p' : S_ISLNK(sb->st_mode) ? 'l' : '-');
        printf("%c", (S_IRUSR & sb->st_mode) ? 'r' : '-');
        printf("%c", (S_IWUSR & sb->st_mode) ? 'w' : '-');
        printf("%c", (S_IXUSR & sb->st_mode) ? 'x' : '-');
        printf("%c", (S_IRGRP & sb->st_mode) ? 'r' : '-');
        printf("%c", (S_IWGRP & sb->st_mode) ? 'w' : '-');
        printf("%c", (S_IXGRP & sb->st_mode) ? 'x' : '-');
        printf("%c", (S_IROTH & sb->st_mode) ? 'r' : '-');
        printf("%c", (sb->st_mode & S_IWOTH) ? 'w' : '-');
        printf("%c  ", (S_IXOTH & sb->st_mode) ? 'x' : '-');
        printf("%d ", sb->st_nlink);
        printf("%s\t", my_user_from_uid(sb->st_uid));
        printf("%s\t", my_group_from_gid(sb->st_gid));
        printf("%5.0lu ", (unsigned long)sb->st_size);
        t = localtime(&sb->st_ctime);
        printf("%s ", lookup[t->tm_mon]);
        printf("%d %02d:%02d ", t->tm_mday, t->tm_hour, t->tm_min);
        if (S_ISLNK(sb->st_mode)) {
            printf("%s ", name);
            bytes_read = readlink(name, link_read, 254);
            link_read[bytes_read] = '\0';
            printf("-> %s\n", link_read);
        } else {
            printf("%s\n", name);
        }
    } else {
        printf("%s\n", name);
    }
}

void display_entries(list_entry_node *entries, int flags) {
    int counter = 0;
    while (entries) {
        display_contents(entries->name, &entries->sb, flags);
        counter++;
        entries = entries->next_entry_node;
    }
}

int get_contents(char *root_path, DIR *d, int flags) {
    char path[PATH_MAX] = {0};
    list_entry_node *entries = NULL;
    struct dirent *entry;
    struct stat s;
    int number_of_entries = 0;
    while ((entry = readdir(d)) != NULL) {
        path[0] = 0;
        strcat(path, root_path);
        strcat(path, "/");
        strcat(path, entry->d_name);
        if (!(flags & kShowHidden) && entry->d_name[0] == '.') {
            continue;
        }
        if (lstat(path, &s) < 0) {
            return (-1);
        }
        if ((flags & kRecursive) && S_ISDIR(s.st_mode)) {
            printf("%s:\n", path);
            DIR *subdir = opendir(path);
            if (subdir == NULL) {
                printf("can't read\n");
            } else {
                int return_value = get_contents(path, subdir, flags);
                closedir(subdir);
                if (return_value < 0) {
                    return (return_value);
                }
            }
        } else {
            entries = insert_entry_by_name(entries, entry->d_name, &s);
        }
        number_of_entries++;
    }

    display_entries(entries, flags);

    return (number_of_entries);
}

int main(int argc, char *argv[]) {
    int flags = 0;
    int index = 0;
    struct stat s;
    char buf[PATH_MAX] = {0};

    flags = set_flags(argc, argv);
    if (flags < 0) {
        return (-1);
    }
    argc -= my_optind;
    argv += my_optind;

    if (argc < 1) {
        static char *temp_argv[] = {"."};
        argv = temp_argv;
        argc = 1;
    }

    lstat(argv[0], &s);
    int arg_is_dir = S_ISDIR(s.st_mode); /* set to 1 if the argument provided is a directory, in which case we'll display its content */

    list_entry_node *entries = NULL;

    while (index < argc) {
        lstat(argv[index], &s);
        if (arg_is_dir || ((flags & kRecursive) && S_ISDIR(s.st_mode))) {
            DIR *subdir = opendir(argv[index]);
            if (subdir == NULL) {
                printf("can't read\n");
            } else {
                if (get_contents(argv[index], subdir, flags) < 0) {
                    return (-1);
                }
                closedir(subdir);
            }
            arg_is_dir = 0;
        } else {
            entries = insert_entry_by_name(entries, argv[index], &s);
        }
        index++;
    }

    display_entries(entries, flags);

    return (0);
}
