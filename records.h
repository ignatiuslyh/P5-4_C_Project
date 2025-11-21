#ifndef RECORDS_H
#define RECORDS_H

#ifdef __cplusplus
extern "C" {
#endif

#define STRING_LEN 50
#define MAX_RECORDS 100


// define struct 
typedef struct StudentRecord {
    int id;
    char name[STRING_LEN];
    char programme[STRING_LEN];
    float mark;
} StudentRecord;

int deleteRecord(StudentRecord records[], int *count, int id);
void showAllRecords(const StudentRecord records[], int count);
int updateRecord(StudentRecord records[], int count, int id, char *field, char *newValue) ;
int findRecordById(const StudentRecord records[], int count, int id);
int insertRecord(StudentRecord records[], int *count, const StudentRecord *newRecord);

#endif