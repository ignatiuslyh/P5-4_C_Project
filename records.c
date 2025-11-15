// records.c handles the core data manipulation (CRUD) operations on the in-memory array of StudentRecord structures
// Operations: INSERT, QUERY, UPDATE, DELETE, SHOW ALL.

#include <stdio.h>
#include <string.h>

#include "records.h"


// int findRecordById(const StudentRecord records[], int count, int id) {
//     return -1; 
// }

// insertRecord(StudentRecord records[], int *count, const StudentRecord newRecord)
// Purpose: add newRecord to array if there is capacity and id is unique.
// Pseudocode:
//  - If records is NULL OR count is NULL:
//      - Print "CMS: ERROR: Internal error (bad parameters)."
//      - RETURN 0
//  - If *count >= MAX_RECORDS:
//      - Print "CMS: ERROR: Database capacity reached."
//      - RETURN 0
//  - idx = findRecordById(records, *count, newRecord.id)
//  - If idx != -1:
//      - Print "CMS: ERROR: Record with ID <id> already exists."
//      - RETURN 0
//  - records[*count] = newRecord    // struct assignment
//  - (*count)++                     // increment stored count
//  - Print "CMS: INSERT successful (ID <id>)."   // optional message here or let caller print
//  - RETURN 1
// int insertRecord(StudentRecord records[], int *count, const StudentRecord newRecord) {
//     return 0; // placeholder: implement steps above
// }

// queryRecord(const StudentRecord records[], int count, int id)
// Purpose: locate record by id and print its details in a simple table.
// Pseudocode:
//  - If records is NULL:
//      - Print "CMS: ERROR: Internal error (null records pointer)."
//      - RETURN 0
//  - idx = findRecordById(records, count, id)
//  - If idx == -1:
//      - Print "CMS: The record with ID <id> does not exist."
//      - RETURN 0
//  - Print header row: "ID   Name   Programme   Mark"
//  - Print separator line
// //  - Print record details for records[idx] using formatted output
// //  - RETURN 1
// int queryRecord(const StudentRecord records[], int count, int id) {
//     return 0; // placeholder: implement steps above
// }

// // updateRecord(StudentRecord records[], int count, int id, char field, void *newValue)
// // Purpose: update one field (N/P/M) of the record identified by id.
// // Notes:
// //  - field: 'N' => name, 'P' => programme, 'M' => mark (case-insensitive)
// //  - newValue: caller-provided data (commonly a string for N/P/M); treat as const char* and parse for M
// // Pseudocode:
// //  - If records is NULL OR newValue is NULL:
// //      - Print error and RETURN 0
// //  - idx = findRecordById(records, count, id)
// //  - If idx == -1:
// //      - Print "CMS: The record with ID <id> does not exist."
// //      - RETURN 0
// //  - Convert field to uppercase
// //  - Switch (field):
// //      - Case 'N':
// //          - Copy string from (char *)newValue into records[idx].name safely (bounded copy + null-terminate)
// //          - Print "CMS: UPDATE successful (ID <id>)"   // optional here
// //          - BREAK
// //      - Case 'P':
// //          - Copy string from (char *)newValue into records[idx].programme safely
// //          - Print "CMS: UPDATE successful (ID <id>)"
// //          - BREAK
// //      - Case 'M':
// //          - Attempt to parse a float from (char *)newValue (e.g., sscanf)
// //          - If parse fails:
// //               - Print "CMS: ERROR: Invalid mark value."
// //               - RETURN 0
// //          - records[idx].mark = parsed_float
// //          - Print "CMS: UPDATE successful (ID <id>)"
// //          - BREAK
// //      - Default:
// //          - Print "CMS: ERROR: Invalid field for update. Use N, P, or M."
// //          - RETURN 0
// //  - End switch
// //  - RETURN 1
// int updateRecord(StudentRecord records[], int count, int id, char field, void *newValue) {
//     return 0; // placeholder: implement steps above
// }

// // deleteRecord(StudentRecord records[], int *count, int id)
// // Purpose: remove a record with the given id and shift subsequent records left.
// // Pseudocode:
// //  - If records is NULL OR count is NULL:
// //      - Print error and RETURN 0
// //  - idx = findRecordById(records, *count, id)
// //  - If idx == -1:
// //      - Print "CMS: The record with ID <id> does not exist."
// //      - RETURN 0
// //  - For i from idx to (*count - 2):
// //      - records[i] = records[i + 1]   // shift left
// //  - (*count)--    // decrement stored count
// //  - Print "CMS: DELETE successful (ID <id>)."
// //  - RETURN 1
// int deleteRecord(StudentRecord records[], int *count, int id) {
//     return 0; // placeholder: implement steps above
// }

void showAllRecords(const StudentRecord records[], int count) {
    if (!records) {
        printf("CMS: ERROR: Internal error (null records pointer).\n");
        return;
    }
    if (count <= 0) {
        printf("CMS: The database is empty.\n");
        return;
    }
    for (int i = 0; i < count; ++i) {
        printf("%d %s %s %.1f\n",
               records[i].id,
               records[i].name,
               records[i].programme,
               records[i].mark);
    }
}