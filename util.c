#include "find.h"

// this function finds an occurrence of needle in haystack
// needle must be matched with a whole word in haystack...
static char *strstr_fully_matched(char *haystack, char *needle) {
    char *rv;
    char padded_needle[strlen(needle) + 3];
    padded_needle[0] = ' ';
    strcpy(padded_needle + 1, needle);
    padded_needle[strlen(needle) + 1] = ' ';
    padded_needle[strlen(needle) + 2] = '\0'; // pad the needle w/ spaces before and after
    if(!strcmp(needle, haystack))
        return haystack; // needle matches the whole haystack
    if (!strncmp(haystack, padded_needle + 1, strlen(needle) + 1)) {
        return haystack; // needle is at the beginning of haystack
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

// lowers the case of each character in its input string
static void to_lower(char* input) {
    for(int i = 0; i < strlen(input); i++)
        input[i] = tolower(input[i]);
}

// subroutine used in qsort
static void swap(void** v, int i, int j) {
    void* temp = v[i];
    v[i] = v[j];
    v[j] = temp;
}

// quick sort on array of strings
void quicksort(char*str[], int left, int right) {
    int i, last;
    if(left >= right)
        return;
    swap(str, left, left + (right-left)/2);
    last = left;
    for(i = left + 1; i <= right; i++)
        if(strcmp(str[i], str[left]) < 0)
            swap(str, ++last, i);
    swap(str, left, last);
    quicksort(str, left, last-1);
    quicksort(str, last+1, right);
}
