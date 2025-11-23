// records.c handles the core data manipulation (CRUD) operations on the in-memory array of StudentRecord structures
// Operations: INSERT, QUERY, UPDATE, DELETE, SHOW ALL.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "records.h"


int findRecordById(const StudentRecord records[], int count, int id) {

    // FOR loop from i = 0 up to (count - 1):
        // IF records[i].id is equal to the input id, THEN:
            // RETURN i (the index).
    for (int i = 0; i <count; i++)
    {
        if (records[i].id == id)
            return i;
    }
    // IF the loop finishes without finding a match, THEN:
        // RETURN -1 (not found).

    return -1; 
}

// // insertRecord(StudentRecord records[], int *count, const StudentRecord newRecord)
// // Purpose: add newRecord to array if there is capacity and id is unique.
// // Pseudocode:
// //  - If records is NULL OR count is NULL:
// //      - Print "CMS: ERROR: Internal error (bad parameters)."
// //      - RETURN 0
// //  - If *count >= MAX_RECORDS:
// //      - Print "CMS: ERROR: Database capacity reached."
// //      - RETURN 0
// //  - idx = findRecordById(records, *count, newRecord.id)
// //  - If idx != -1:
// //      - Print "CMS: ERROR: Record with ID <id> already exists."
// //      - RETURN 0
// //  - records[*count] = newRecord    // struct assignment
// //  - (*count)++                     // increment stored count
// //  - Print "CMS: INSERT successful (ID <id>)."   // optional message here or let caller print
// //  - RETURN 1
// // int insertRecord(StudentRecord records[], int *count, const StudentRecord newRecord) {
// //     return 0; // placeholder: implement steps above
// // }

int insertRecord(StudentRecord records[], int *count, const StudentRecord *newRecord) {
    // Validate input pointers
    if (!records || !count || !newRecord) {
        printf("CMS: ERROR: Internal error (bad parameters).\n");
        return 0;
    }

    // Check capacity
    if (*count >= MAX_RECORDS) {
        printf("CMS: ERROR: Database full.\n");
        return 0;
    }

    // Check for duplicate ID
    if (findRecordById(records, *count, newRecord->id) != -1) {
        printf("CMS: ERROR: Record with ID %d already exists.\n", newRecord->id);
        return 0;
    }

    // Insert record
    records[*count].id = newRecord->id;
    strncpy(records[*count].name, newRecord->name, STRING_LEN - 1);
    records[*count].name[STRING_LEN - 1] = '\0';
    strncpy(records[*count].programme, newRecord->programme, STRING_LEN - 1);
    records[*count].programme[STRING_LEN - 1] = '\0';
    records[*count].mark = newRecord->mark;

    // Increment stored count
    (*count)++;

    return 1;
}

// // queryRecord(const StudentRecord records[], int count, int id)
// // Purpose: locate record by id and print its details in a simple table.
// // Pseudocode:
// //  - If records is NULL:
// //      - Print "CMS: ERROR: Internal error (null records pointer)."
// //      - RETURN 0
// //  - idx = findRecordById(records, count, id)
// //  - If idx == -1:
// //      - Print "CMS: The record with ID <id> does not exist."
// //      - RETURN 0
// //  - Print header row: "ID   Name   Programme   Mark"
// //  - Print separator line
// // //  - Print record details for records[idx] using formatted output
// // //  - RETURN 1
// // int queryRecord(const StudentRecord records[], int count, int id) {
// //     return 0; // placeholder: implement steps above
// // }

int updateRecord(StudentRecord records[], int *count, int id, char *field, char *newValue) {

    // 1. Find the record index.
    int index = findRecordById(records, *count, id);
    // 2. Check if there is a record index.
    if (index != -1)
    {
    // 3.Update the name field with newValue when user typed "Name" only
        if (strcmp(field, "Name") == 0)
        {
            strcpy(records[index].name, newValue);     
        }
        //Update the programme field with newValue when user typed "Programme" only
        else if (strcmp(field, "Programme") == 0)
        {
            strcpy(records[index].programme, newValue);
        }
       //Update the mark field with newValue when user typed "Mark" only
        else if(strcmp(field, "Mark") == 0)
        {
            records[index].mark = atof(newValue);
        }
    }
    
    //4. No existing record with the student ID found in database
    else
    {
         return 0;
    }

    //5. Update Success.
    return 1; 
    }

int deleteRecord(StudentRecord records[], int *count, int id) {
    
     // Validate parameters
    if (records == NULL || count == NULL) {
        printf("CMS: ERROR: Internal error (bad parameters).\n");
        return 0;
    }
    
    // Find the record to delete
    int index = findRecordById(records, *count, id);
    if (index == -1) {
        printf("CMS: The record with ID %d does not exist.\n", id);
        return 0;
    }

    for (int i = 0; i < *count; ++i) {
        printf("%-8d %-20s %-24s %.1f\n",
               records[i].id,
               records[i].name,
               records[i].programme,
               records[i].mark);
    }
    
    // Shift all subsequent records left to overwrite the deleted record
    for (int i = index; i < *count - 1; i++) {
        records[i] = records[i + 1];
    }
    
    // Decrement the count
    (*count)--;
    
    printf("CMS:  The record with ID=%d is successfully deleted. \n", id);
    return 1;
}



void showAllRecords(const StudentRecord records[], int count)
{
    if (!records) {
        printf("CMS: ERROR: Internal error (no records buffer).\n");
        return;
    }

    printf("CMS: Here are all the records found in the table \"StudentRecords\".\n");
    printf("%-8s %-20s %-24s %s\n", "ID", "Name", "Programme", "Mark");

    if (count <= 0) {
        printf("No records.\n");
        return;
    }

    for (int i = 0; i < count; ++i) {
        printf("%-8d %-20s %-24s %.1f\n",
               records[i].id,
               records[i].name,
               records[i].programme,
               records[i].mark);
    }
}