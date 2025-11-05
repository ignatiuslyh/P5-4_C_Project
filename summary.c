// summary.c contains functions for data analysis and statistics.

#include <stdio.h>
#include <string.h>
#include <math.h> // Needed for statistics (e.g., floor, round)


#define STRING_LEN 50

typedef struct {
    int id;
    char name[STRING_LEN];
    char programme[STRING_LEN];
    float mark;
} StudentRecord;


// UTILITY FUNCTION: calculateAverageMark
float calculateAverageMark(const StudentRecord records[], int count) {

    // IF count is 0, THEN RETURN 0.0.

    // Initialize total_marks to 0.0.

    // FOR loop from i = 0 up to (count - 1):
        // Add records[i].mark to total_marks.

    // RETURN total_marks divided by count.

    return 0.0; // Temporary placeholder return
}


// FUNCTION: showSummary
void showSummary(const StudentRecord records[], int count) {

    // 1. Check if database is empty.
    // IF count is 0, THEN:
        // Print message ("CMS: The database is empty. No summary available.").
        // RETURN.

    // 2. Initialize variables for min/max.
    // Initialize max_mark to -1.0.
    // Initialize min_mark to 101.0. // Marks are 0-100.
    // Initialize high_id and low_id (or index) to track top/bottom student.

    // 3. Calculation loop.
    // FOR loop from i = 0 up to (count - 1):
        // Current mark = records[i].mark;

        // IF current mark is GREATER THAN max_mark, THEN:
            // Update max_mark = current mark.
            // Update high_id (or high_index).

        // IF current mark is LESS THAN min_mark, THEN:
            // Update min_mark = current mark.
            // Update low_id (or low_index).

    // 4. Calculate final metrics.
    // Call calculateAverageMark and store the result (average_mark).
    // Count the number of students who passed (Mark >= 50) and failed (Mark < 50).

    // 5. Print Summary Report.
    // Print header: "--- Database Summary Report ---".
    // Print Total Records: "Total Students: X".
    // Print Average Mark: "Average Mark: Y (formatted to two decimal places)".
    // Print Highest Mark: "Highest Mark: Z by Student ID: [ID]".
    // Print Lowest Mark: "Lowest Mark: A by Student ID: [ID]".
    // Print Pass/Fail Count: "Passed: [Count], Failed: [Count]".
    // Print footer: "-----------------------------".
}