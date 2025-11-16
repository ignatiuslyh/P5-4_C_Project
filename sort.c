// sort.c is responsible for the sorting algos to order StudentRecords array

#include <stdio.h>
#include <string.h>

#include "records.h"
#include <stdlib.h>

static void swapRecords(StudentRecord *a, StudentRecord *b) {
    if (!a || !b) return;
    StudentRecord tmp = *a;
    *a = *b;
    *b = tmp;
}

void sort_and_print(const StudentRecord records[], int count, int by_id, int asc) {
    if (!records || count <= 0) return;

    StudentRecord *copy = malloc(sizeof(StudentRecord) * (size_t)count);
    if (!copy) {
        fprintf(stderr, "CMS: ERROR: Out of memory while sorting.\n");
        return;
    }
    memcpy(copy, records, sizeof(StudentRecord) * (size_t)count);

    // Simple bubble sort 
    for (int i = 0; i < count - 1; ++i) {
        int swapped = 0;
        for (int j = 0; j < count - 1 - i; ++j) {
            int should_swap = 0;
            if (by_id) {
                if (asc)
                    should_swap = (copy[j].id > copy[j+1].id);
                else
                    should_swap = (copy[j].id < copy[j+1].id);
            } else {
               
                if (copy[j].mark == copy[j+1].mark) {
                    
                    if (asc)
                        should_swap = (copy[j].id > copy[j+1].id);
                    else
                        should_swap = 0; 
                } else {
                    if (asc)
                        should_swap = (copy[j].mark > copy[j+1].mark);
                    else
                        should_swap = (copy[j].mark < copy[j+1].mark);
                }
            }
            if (should_swap) {
                swapRecords(&copy[j], &copy[j+1]);
                swapped = 1;
            }
        }
        if (!swapped) break;
    }

    for (int i = 0; i < count; ++i) {
        printf("%d %s %s %.1f\n", copy[i].id, copy[i].name, copy[i].programme, copy[i].mark);
    }

    free(copy);
}