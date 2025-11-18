// sort.c is responsible for the sorting algos to order StudentRecords array

#include <stdio.h>
#include <string.h>

#include "records.h"
#include "sort.h"

void sort_and_print(const StudentRecord records[], int count, int by_id, int asc)
{
    if (!records) {
        printf("CMS: ERROR: Internal error (no records buffer).\n");
        return;
    }

    if (count <= 0) {
        //  call showAllRecords for header/message formatting
        showAllRecords(records, count);
        return;
    }

    // copy records 
    StudentRecord tmp[MAX_RECORDS];
    for (int i = 0; i < count; ++i) tmp[i] = records[i];

    // selection sort
    for (int i = 0; i < count - 1; ++i) {
        int best = i;
        for (int j = i + 1; j < count; ++j) {
            if (by_id) {
                if (asc) {
                    if (tmp[j].id < tmp[best].id) best = j;
                } else {
                    if (tmp[j].id > tmp[best].id) best = j;
                }
            } else { // sort by mark
                if (asc) {
                    if (tmp[j].mark < tmp[best].mark) best = j;
                } else {
                    if (tmp[j].mark > tmp[best].mark) best = j;
                }
            }
        }
        if (best != i) {
            StudentRecord t = tmp[i];
            tmp[i] = tmp[best];
            tmp[best] = t;
        }
    }

    showAllRecords(tmp, count);
}