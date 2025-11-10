#ifndef SUMMARY_H
#define SUMMARY_H

#include "records.h"

/* Print all records in a neat table. Returns 1 if printed, 0 if empty/error. */
int showAllRecords(const StudentRecord records[], int count);

/* Local summary implementations */
void show_summary_local(const StudentRecord records[], int count);

/* Wrapper for optional HAVE_SUMMARY build path */
void showSummary(const StudentRecord records[], int count);

#endif