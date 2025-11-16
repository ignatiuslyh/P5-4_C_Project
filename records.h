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
int updateRecord(StudentRecord records[], int count, int id);

#endif