#ifndef RECORDS_H
#define RECORDS_H


#ifndef STRING_LEN
#define STRING_LEN 50
#endif

typedef struct StudentRecord {
    int id;
    char name[STRING_LEN];
    char programme[STRING_LEN];
    float mark;
} StudentRecord;



#endif