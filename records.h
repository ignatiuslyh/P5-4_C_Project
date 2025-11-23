#ifndef RECORDS_H
#define RECORDS_H

#define MAX_RECORDS 1024
#define STRING_LEN 64

typedef struct {
    int id;
    char name[STRING_LEN];
    char programme[STRING_LEN];
    float mark;
} StudentRecord;

int findRecordById(const StudentRecord records[], int count, int id);
int insertRecord(StudentRecord records[], int *count, const StudentRecord *newRecord);
int updateRecord(StudentRecord records[], int *count, int id, char *field, char *newValue);
int deleteRecord(StudentRecord records[], int *count, int id);
void showAllRecords(const StudentRecord records[], int count);
int queryRecord(const StudentRecord records[], int count, int id);

#endif /* RECORDS_H */