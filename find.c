#include "find.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define NUMBERED 0x01
#define EXCEPT   0x02
#define SORTED   0x04
#define REVERSE  0x08
#define MATCH    0x10
#define CASE     0x20
#define FIRST    0x40
#define PARTIAL  0x80

char **lineptr;
int line_count = 0;

char *strcasestr(const char *haystack, const char *needle) {
    if (!*needle)
        return (char *)haystack;
    for (; *haystack; haystack++) {
        if (toupper(*haystack) == toupper(*needle)) {
            const char *h, *n;
            for (h = haystack, n = needle; *h && *n; h++, n++) {
                if (toupper(*h) != toupper(*n)) {
                    break;
                }
            }
            if (!*n)
                return (char *)haystack;
        }
    }
    return NULL;
}

static char *strstr_fully_matched(const char *haystack, const char *needle) {
    char *rv;
    char padded_needle[strlen(needle) + 3];
    padded_needle[0] = ' ';
    strcpy(padded_needle + 1, needle);
    padded_needle[strlen(needle) + 1] = ' ';
    padded_needle[strlen(needle) + 2] = '\0'; // pad the needle w/ spaces before and after
    if(!strcmp(needle, haystack))
        return (char *)haystack; // needle matches the whole haystack
    if (!strncmp(haystack, padded_needle + 1, strlen(needle) + 1)) {
        return (char *)haystack; // needle is at the beginning of haystack
    }
    if ((rv = strstr(haystack, padded_needle)) != NULL) {
        return rv + 1; // needle is at the middle of haystack.
    }
    padded_needle[strlen(needle) + 1] = '\0'; // remove the last space
    if ((rv = strstr(haystack, padded_needle)) != NULL &&
        rv[strlen(padded_needle)] == '\0') {
        return rv + 1; // needle is at the end of haystack.
    }
    return NULL;
}

char *strstr_w_option(const char *haystack, const char *needle, flags option) {
    if (option & CASE) {
        return strcasestr(haystack, needle);
    } else if (option & MATCH) {
        return strstr_fully_matched(haystack, needle);
    } else {
        return strstr(haystack, needle);
    }
}

int compare(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

int reverse_compare(const void *a, const void *b) {
    return strcmp(*(const char **)b, *(const char **)a);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: \"find [-n] [-x] [-s] [-r] [-m] [-c] [-f] [-p] pattern\"\n");
        exit(1);
    }

    int option = 0;
    char *pattern = NULL;

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
                case 'n': option |= NUMBERED; break;
                case 'x': option |= EXCEPT; break;
                case 's': option |= SORTED; break;
                case 'r': option |= REVERSE; break;
                case 'm': option |= MATCH; break;
                case 'c': option |= CASE; break;
                case 'f': option |= FIRST; break;
                case 'p': option |= PARTIAL; break;
                default: 
                    fprintf(stderr, "Unknown option: %s\n", argv[i]);
                    exit(1);
            }
        } else {
            pattern = argv[i];
        }
    }

    if (!pattern) {
        fprintf(stderr, "Pattern not provided.\n");
        exit(1);
    }

    if ((option & SORTED) && (option & REVERSE)) {
        fprintf(stderr, "find: Illegal usage. -s and -r can't be used together.\n");
        exit(4);
    }

    if ((option & EXCEPT) && (option & FIRST)) {
        fprintf(stderr, "find: Illegal usage. -f and -x can't be used together.\n");
        exit(4);
    }

    if ((option & EXCEPT) && (option & PARTIAL)) {
        fprintf(stderr, "find: Illegal usage. -p and -x can't be used together.\n");
        exit(4);
    }

    size_t len = 0;
    ssize_t read;
    char *line = NULL;
    lineptr = malloc(sizeof(char *) * 1000);

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\n")] = 0;
        lineptr[line_count++] = strdup(line);
    }

    free(line);

    if (option & SORTED) {
        qsort(lineptr, line_count, sizeof(char *), compare);
    } else if (option & REVERSE && !(option & SORTED)) {
        qsort(lineptr, line_count, sizeof(char *), reverse_compare);
    }

    int start = 0, end = line_count, step = 1;
    if (option & REVERSE && !(option & SORTED)) {
        start = line_count - 1;
        end = -1;
        step = -1;
    }

    char initial[10] = "";

    for (int i = start; i != end; i += step) {
        if (option & NUMBERED)
            sprintf(initial, "%d. ", i + 1);
        char *first_occurrence = strstr_w_option(lineptr[i], pattern, option);
        if (((option & EXCEPT) != 0) != (first_occurrence != NULL)) {
            if (option & PARTIAL) {
                int index = first_occurrence - lineptr[i];
                if (index > 10) {
                    printf("%.10s...", lineptr[i]);
                } else {
                    printf("%.*s", index, lineptr[i]);
                }
                printf("%s", pattern);
                if (strlen(lineptr[i]) - index - strlen(pattern) > 5) {
                    printf("...%.5s\n", lineptr[i] + strlen(lineptr[i]) - 5);
                } else {
                    printf("%s\n", first_occurrence + strlen(pattern));
                }
            } else if (option & FIRST) {
                int index = first_occurrence - lineptr[i];
                printf("@%d: %s\n", index + 1, lineptr[i]);
            } else {
                printf("%s%s\n", initial, lineptr[i]);
            }
        }
    }
    return 0;
}
