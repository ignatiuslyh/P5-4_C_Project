// query.c handles searching for student records by ID
#include <stdio.h>
#include "query.h"
#include "records.h"

int queryRecord(const StudentRecord records[], int count, int id) {
    // Validate input
    if (!records) {
        printf("CMS: ERROR: Internal error (null records pointer).\n");
        return 0;
    }

    // Search for the record with matching ID
    for (int i = 0; i < count; i++) {
        if (records[i].id == id) {
            // Record found - display it
            printf("CMS: The record with ID=%d is found in the data table.\n", id);
            printf("%-8s %-20s %-24s %s\n", "ID", "Name", "Programme", "Mark");
            printf("%-8d %-20s %-24s %.1f\n",
                records[i].id,
                records[i].name,
                records[i].programme,
                records[i].mark);
            return 1;
        }
    }

    // Record not found
    printf("CMS: The record with ID=%d does not exist.\n", id);
    return 0;
}