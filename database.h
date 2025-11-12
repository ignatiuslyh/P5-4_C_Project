#ifndef DATABASE_H
#define DATABASE_H

#define MAX_RECORDS 100
#define STRING_LEN 50

typedef struct {
    int id;
    char name[STRING_LEN];
    char programme[STRING_LEN];
    float mark;
} StudentRecord;


int loadDB(const char *filename, StudentRecord records[], int *count);

int saveDB(const char *filename, const StudentRecord records[], int count);


#endif 