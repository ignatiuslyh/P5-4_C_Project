#ifndef QUERY_H
#define QUERY_H

#include "records.h"

// Query by using ID to find student record
// Returns 1 if found, 0 if not found
int queryRecord(const StudentRecord records[], int count, int id);

#endif