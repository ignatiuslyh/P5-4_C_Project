// sort.c is responsible for the sorting algos to order StudentRecords array

#include <stdio.h>
#include <string.h>


#define STRING_LEN 50

typedef struct {
    int id;
    char name[STRING_LEN];
    char programme[STRING_LEN];
    float mark;
} StudentRecord;


void swapRecords(StudentRecord *a, StudentRecord *b) {

    // StudentRecord temp = *a;
    // *a = *b;
    // *b = temp;
}

//refer to main.c and rmb to allow for asc and desc

void sortById(StudentRecord records[], int count) {
    // (Bubble Sort for simplicity):
    // Can try other sorting methods like merge sort quick sort etc

}


void sortByMark(StudentRecord records[], int count) {
    // (Bubble Sort, modified for descending order):
    
}