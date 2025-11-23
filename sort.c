// sort.c - bubble sort algo for show all command 

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
        // reuse existing formatting for empty DB / header
        showAllRecords(records, count);
        return;
    }

    // Make a local copy so we don't change the original array.
    StudentRecord records_copy[MAX_RECORDS];
    for (int i = 0; i < count; ++i) {
        records_copy[i] = records[i];
    }

    // Bubble sort: repeatedly pass through the array and swap adjacent out-of-order items. We stop early if a pass makes no swaps.

    for (int pass_index = 0; pass_index < count - 1; ++pass_index) {
        int did_swap = 0; // track whether this pass performed any swaps

        // after pass_index passes, the last pass_index elements are in final position
        for (int pair_index = 0; pair_index < count - 1 - pass_index; ++pair_index) {
            StudentRecord *left = &records_copy[pair_index];
            StudentRecord *right = &records_copy[pair_index + 1];

            int out_of_order = 0;
            if (by_id) {
                // compare by ID
                if (asc) out_of_order = (left->id > right->id);
                else     out_of_order = (left->id < right->id);
            } else {
                // compare by mark
                if (asc) out_of_order = (left->mark > right->mark);
                else     out_of_order = (left->mark < right->mark);
            }

            if (out_of_order) {
                // swap adjacent elements
                StudentRecord tmp = *left;
                *left = *right;
                *right = tmp;
                did_swap = 1;
            }
        }

        // no swaps => array is already sorted
        if (!did_swap) break;
    }

    // Print header and sorted rows (same format as showAllRecords)
    printf("CMS: Here are all the records found in the table \"StudentRecords\".\n");
    printf("%-8s %-20s %-24s %s\n", "ID", "Name", "Programme", "Mark");
    for (int i = 0; i < count; ++i) {
        printf("%-8d %-20s %-24s %.1f\n",
               records_copy[i].id,
               records_copy[i].name,
               records_copy[i].programme,
               records_copy[i].mark);
    }
}