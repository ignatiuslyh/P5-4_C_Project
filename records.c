// records.c handles the core data manipulation (CRUD) operations on the in-memory array of StudentRecord structures
// Operations: INSERT, QUERY, UPDATE, DELETE, SHOW ALL.

#include <stdio.h>
#include <string.h>

#include <stdio.h>
#include <string.h>

// (same as database.c)
#define MAX_RECORDS 200
#define STRING_LEN 50

typedef struct {
    int id;
    char name[STRING_LEN];
    char programme[STRING_LEN];
    float mark;
} StudentRecord;

int findRecordById(const StudentRecord records[], int count, int id) {

    // FOR loop from i = 0 up to (count - 1):
        // IF records[i].id is equal to the input id, THEN:
            // RETURN i (the index).

    // IF the loop finishes without finding a match, THEN:
        // RETURN -1 (not found).

    return -1; // Temporary placeholder return
}

int insertRecord(StudentRecord records[], int *count, const StudentRecord newRecord) {

    // 1. Check for capacity.
    // IF *count is greater than or equal to MAX_RECORDS, THEN:
        // Print error message ("CMS: ERROR: Database capacity reached.").
        // RETURN -1.

    // 2. Check for duplicate ID.
    // index = findRecordById(records, *count, newRecord.id);
    // IF index is NOT -1, THEN:
        // Print error message ("CMS: ERROR: Record with ID X already exists.").
        // RETURN 0.

    // 3. Insert the record.
    // Copy the contents of newRecord into the records[*count] slot.
    // Example: records[*count] = newRecord;

    // 4. Update count.
    // Increment the counter: (*count)++;

    // 5. Success.
    // Print success message ("CMS: New record inserted.")
    // RETURN 1.

    return 0; // Temporary placeholder return
}


int queryRecord(const StudentRecord records[], int count, int id) {

    // 1. Find the record index.
    // index = findRecordById(records, count, id);

    // 2. Check if found.
    // IF index is -1, THEN:
        // Print error message ("CMS: The record with ID X does not exist.").
        // RETURN 0.

    // 3. Print details.
    // Print the header row (ID, Name, Programme, Mark).
    // Print separating line (e.g., ----).
    // Print the details of records[index] using a formatted line.

    // 4. Success.
    // RETURN 1.

    return 0; // Temporary placeholder return
}

int updateRecord(StudentRecord records[], int count, int id, char field, void *newValue) {

    // 1. Find the record index.
    // index = findRecordById(records, count, id);

    // 2. Check if found.
    // IF index is -1, THEN:
        // Print error message ("CMS: The record with ID X does not exist.").
        // RETURN 0.

    // 3. Apply the update based on 'field'.
    // SWITCH statement on 'field' (after converting field to uppercase for safety):
        // CASE 'N' (Name):
            // Use strcpy to copy the new string from (char *)newValue into records[index].name.
            // Print success message.
            // BREAK.
        // CASE 'P' (Programme):
            // Use strcpy to copy the new string from (char *)newValue into records[index].programme.
            // Print success message.
            // BREAK.
        // CASE 'M' (Mark):
            // Cast newValue to float and assign it: records[index].mark = *(float *)newValue;
            // Print success message.
            // BREAK.
        // DEFAULT:
            // Print error message ("CMS: ERROR: Invalid field for update.").
            // RETURN -1.

    // 4. Success.
    // RETURN 1.

    return 0; // Temporary placeholder return
}

int deleteRecord(StudentRecord records[], int *count, int id) {

    // 1. Find the record index.
    // index = findRecordById(records, *count, id);

    // 2. Check if found.
    // IF index is -1, THEN:
        // Print error message ("CMS: The record with ID X does not exist.").
        // RETURN 0.

    // 3. (Optional Assignment Requirement: Confirmation)
    // NOTE: The main command loop (in main.c) should handle the "Are you sure? Y/N" prompt.
    // Assuming confirmation passed, proceed with deletion.

    // 4. Delete by shifting (The array compression loop).
    // FOR loop starting from i = index up to (*count - 2):
        // Copy the next record into the current position: records[i] = records[i + 1];

    // 5. Update count.
    // Decrement the counter: (*count)--;

    // 6. Success.
    // Print success message ("CMS: The record with ID X is successfully deleted.")
    // RETURN 1.

    return 0; // Temporary placeholder return
}

int showAllRecords(const StudentRecord records[], int count) {

    // 1. Check if database is empty.
    // IF count is 0, THEN:
        // Print message ("CMS: The database is currently empty.").
        // RETURN 0.

    // 2. Print header.
    // Print summary message ("CMS: Here are all X records found...").
    // Print table header (ID, Name, Programme, Mark) using wide format specifiers (e.g., %-10s).
    // Print separating line (e.g., ----).

    // 3. Print records.
    // FOR loop from i = 0 up to (count - 1):
        // Print the details of records[i] using a formatted line.

    // 4. Success.
    // RETURN 1.

    return 0; // Temporary placeholder return
}