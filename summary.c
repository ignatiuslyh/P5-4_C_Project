// summary.c contains functions for data analysis and statistics.
#include <stdio.h>
#include <float.h>

#define STRING_LEN 50

#include "records.h"
#include "summary.h"

/* Simple helper to compute average mark. Returns 0.0f for empty/null input. */
static float calculateAverageMark(const StudentRecord records[], int count) {
    if (!records || count <= 0) return 0.0f;
    double total = 0.0;
    for (int i = 0; i < count; ++i) total += records[i].mark;
    return (float)(total / (double)count);
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

    int passed = 0, failed = 0;
    float max_mark = -FLT_MAX;
    float min_mark =  FLT_MAX;
    int max_idx = -1, min_idx = -1;

    for (int i = 0; i < count; ++i) {
        float m = records[i].mark;
        if (m > max_mark) { max_mark = m; max_idx = i; }
        if (m < min_mark) { min_mark = m; min_idx = i; }
        if (m >= 50.0f) ++passed;
        else ++failed;
    }

    float avg = calculateAverageMark(records, count);

    printf("CMS: SUMMARY: %d record(s)\n", count);
    printf("  Average mark : %.2f\n", (double)avg);

    if (max_idx >= 0) {
        printf("  Highest mark  : %.1f (ID %d, %s, %s)\n",
               records[max_idx].mark,
               records[max_idx].id,
               records[max_idx].name,
               records[max_idx].programme);
    }

    if (min_idx >= 0) {
        printf("  Lowest mark   : %.1f (ID %d, %s, %s)\n",
               records[min_idx].mark,
               records[min_idx].id,
               records[min_idx].name,
               records[min_idx].programme);
    }

    printf("  Passed        : %d\n", passed);
    printf("  Failed        : %d\n", failed);
}