#ifndef DATABASE_H
#define DATABASE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "records.h"

// File I/O for the student database.
int loadDB(const char *filename, StudentRecord records[], int *count);
int saveDB(const char *filename, const StudentRecord records[], int count);

#ifdef __cplusplus
}
#endif

#endif 