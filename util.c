#include "find.h"

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
