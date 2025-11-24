#ifndef IMPORT_H
#define IMPORT_H

#include "records.h"

// Import CSV file handler used by processCommand.

int importRecords(const char *local_args, StudentRecord records[], int *count);

#endif