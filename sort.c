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


void sortById(StudentRecord records[], int count) {
    // (Bubble Sort for simplicity):
    // Can try other sorting methods like merge sort etc

    // IF count is less than or equal to 1, THEN RETURN (nothing to sort).

    // Outer FOR loop from i = 0 up to (count - 2): // Controls passes
        // Inner FOR loop from j = 0 up to (count - i - 2): // Controls comparisons
            // IF records[j].id is GREATER THAN records[j + 1].id, THEN:
                // Call swapRecords(&records[j], &records[j + 1]);

    // Print success message ("CMS: Records sorted by ID.").
}


void sortByMark(StudentRecord records[], int count) {
    // (Bubble Sort, modified for descending order):
    // Can try other sorting methods like merge sort etc

    // IF count is less than or equal to 1, THEN RETURN (nothing to sort).

    // Outer FOR loop from i = 0 up to (count - 2):
        // Inner FOR loop from j = 0 up to (count - i - 2):
            // IF records[j].mark is LESS THAN records[j + 1].mark, THEN: // Comparison flipped for DESCENDING
                // Call swapRecords(&records[j], &records[j + 1]);

    // Print success message ("CMS: Records sorted by Mark (High to Low).").
}