// summary.c contains functions for to show overall statistics
#include <stdio.h>
#include "records.h"

// compute average mark (returns 0.0 for empty input)
static float calculateAverageMark(const StudentRecord records[], int count) {
    if (!records || count <= 0) return 0.0f;
    double sum = 0.0;                 // use double to reduce rounding error
    for (int i = 0; i < count; ++i) sum += records[i].mark;
    return (float)(sum / count);
}

// round positive mark to hundredths as an integer (e.g. 85.127 -> 8513)
static int round_to_hundredths(float m) {
    return (int)(m * 100.0f + 0.5f);
}

void showSummary(const StudentRecord records[], int count) {
    if (!records) {
        printf("CMS: ERROR: Internal error (null records pointer).\n");
        return;
    }
    if (count <= 0) {
        printf("CMS: The database is empty. No summary available.\n");
        return;
    }

    // find max/min and count pass/fail in one pass
    int passed = 0, failed = 0;
    float max_mark = records[0].mark; // initialize from first entry (simpler)
    float min_mark = records[0].mark;
    for (int i = 0; i < count; ++i) {
        float m = records[i].mark;
        if (m > max_mark) max_mark = m;
        if (m < min_mark) min_mark = m;
        if (m >= 50.0f) ++passed;
        else ++failed;
    }

    float avg = calculateAverageMark(records, count);

    // print basic info
    printf("CMS: SUMMARY: %d record(s)\n", count);
    printf("  Total students: %d\n", count);
    printf("  Average mark : %.2f\n", (double)avg);

    // print highest mark and all names tied at two decimal places
    {
        int target = round_to_hundredths(max_mark);
        printf("  Highest mark  : %.2f (", (double)max_mark);
        int first = 1;
        for (int i = 0; i < count; ++i) {
            if (round_to_hundredths(records[i].mark) == target) {
                if (!first) printf(", ");
                printf("%s", records[i].name);
                first = 0;
            }
        }
        printf(")\n");
    }

    // print lowest mark and all names tied at two decimal places
    {
        int target = round_to_hundredths(min_mark);
        printf("  Lowest mark   : %.2f (", (double)min_mark);
        int first = 1;
        for (int i = 0; i < count; ++i) {
            if (round_to_hundredths(records[i].mark) == target) {
                if (!first) printf(", ");
                printf("%s", records[i].name);
                first = 0;
            }
        }
        printf(")\n");
    }

    // pass/fail counts
    printf("  Passed        : %d\n", passed);
    printf("  Failed        : %d\n", failed);
}