#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "import.h"
#include "history.h"
#include "records.h"

#ifndef REQUIRED_LENGTH
#define REQUIRED_LENGTH 7
#endif

// local trim
static char *trim(char *s) {
    if (!s) return s;
    // trim leading
    while (*s && isspace((unsigned char)*s)) s++;
    if (*s == '\0') return s;
    // trim trailing
    char *end = s + strlen(s) - 1;
    while (end > s && isspace((unsigned char)*end)) *end-- = '\0';
    return s;
}

// Logic for IMPORT feature
int importRecords(const char *local_args, StudentRecord records[], int *count) {
    // Make sure IMPORT contains filename
    if (!local_args || !local_args[0]) {
        printf("CMS: IMPORT requires a filename. Usage: IMPORT file.csv\n");
        addHistory("IMPORT: Failed - no filename");
        return 1;
    }

    // Make a copy of input and trim filename
    char fname[260];
    strncpy(fname, local_args, sizeof(fname) - 1);
    fname[sizeof(fname) - 1] = '\0';
    trim(fname);

    // Open CSV file
    FILE *fp = fopen(fname, "r");
    if (!fp) {
        printf("CMS: Unable to find '%s' for import.\n", fname);
        char msg[HISTORY_DESC_LEN]; 
        snprintf(msg, sizeof(msg), "IMPORT: Failed to open '%s'", fname); 
        addHistory(msg);
        return 1;
    }

    // Temporary storage for parsed rows
    StudentRecord tmp[MAX_RECORDS];
    int tmp_count = 0;
    int dup_count = 0;

    // Read file line by line
    char line[512];
    char header[512];
    // Skip first row, since it is header
    if (!fgets(header, sizeof(header), fp)) {
        // Return error if no rows or missing rows
        printf("CMS: Missing key columns in \"%s\". IMPORT cancelled.\n", fname);
        fclose(fp);
        return 1;
    }
    int line_no = 1;
    while (fgets(line, sizeof(line), fp)) {
        line_no++;
        // Remove trailing newlines and skip empty newlines
        size_t L = strlen(line);
        while (L > 0 && (line[L - 1] == '\n' || line[L - 1] == '\r')) line[--L] = '\0';
        if (L == 0) continue;

        // Make a copy for splitting string
        char buf[512];
        strncpy(buf, line, sizeof(buf) - 1);
        buf[sizeof(buf) - 1] = '\0';

        // Split CSV into ID, Name, Programme, Mark
        char *f0 = strtok(buf, ",");
        char *f1 = strtok(NULL, ",");
        char *f2 = strtok(NULL, ",");
        char *f3 = strtok(NULL, ",");

        // Check for missing columns, if any missing abort IMPORT
        if (!f0 || !f1 || !f2 || !f3) {
            printf("CMS: Missing value on line %d in \"%s\"\n", line_no, fname);
            char msg[HISTORY_DESC_LEN];
            snprintf(msg, sizeof(msg), "IMPORT: Failed - malformed CSV '%s' line %d", fname, line_no);
            addHistory(msg);
            fclose(fp);
            return 1;
        }

        // Trim whitespace from each field
        char *p0 = trim(f0);
        char *p1 = trim(f1);
        char *p2 = trim(f2);
        char *p3 = trim(f3);
        
        // Make sure ID has REQUIRED_LENGTH = 7, if not, return error
        size_t idlen = strlen(p0);
        if (idlen != REQUIRED_LENGTH) {
            printf("CMS: Invalid ID on line %d in \"%s\" - expected %d characters.\n", line_no, fname, REQUIRED_LENGTH);
            char msg[HISTORY_DESC_LEN];
            snprintf(msg, sizeof(msg), "IMPORT: Failed - invalid ID length in '%s' line %d", fname, line_no);
            addHistory(msg);
            fclose(fp);
            return 1;
        }
        int id_digits = 1;
        for (size_t k = 0; k < idlen; ++k) {
            if (!isdigit((unsigned char)p0[k])) { id_digits = 0; break; }
        }
        if (!id_digits) {
            printf("CMS: Invalid ID on line %d in \"%s\" - ID must contain only digits.\n", line_no, fname);
            char msg[HISTORY_DESC_LEN];
            snprintf(msg, sizeof(msg), "IMPORT: Failed - non-digit ID in '%s' line %d", fname, line_no);
            addHistory(msg);
            fclose(fp);
            return 1;
        }

        // Parse and validate numeric/text fields
        int id = 0;
        float mark = 0.0f;
        if (sscanf(p0, "%d", &id) != 1) continue;        // invalid ID
        if (sscanf(p3, "%f", &mark) != 1) continue;      // invalid mark
        if (mark < 0.0f || mark > 100.0f) continue;      // out-of-range mark

        // Store parsed row into temporary array
        if (tmp_count >= MAX_RECORDS) break;
        tmp[tmp_count].id = id;
        strncpy(tmp[tmp_count].name, p1, STRING_LEN - 1); tmp[tmp_count].name[STRING_LEN - 1] = '\0';
        strncpy(tmp[tmp_count].programme, p2, STRING_LEN - 1); tmp[tmp_count].programme[STRING_LEN - 1] = '\0';
        tmp[tmp_count].mark = mark;

        // Check for duplicates
        int is_dup = 0;
        for (int i = 0; i < *count; ++i) {
            if (records[i].id == id) { is_dup = 1; break; }
        }
        if (is_dup) dup_count++;

        tmp_count++;
    }

    // Close file
    fclose(fp);

    // Return error if no valid rows found
    if (tmp_count == 0) {
        printf("CMS: Missing valid rows in \"%s\". IMPORT cancelled.\n", fname);
        return 1;
    }

    // Prompt if any rows will be overwritten (Y/N) to continue
    if (dup_count > 0) {
        char resp[8];
        printf("WARNING: %d existing record(s) will be overridden. Continue? (Y/N): ", dup_count);
        fflush(stdout);
        if (!fgets(resp, sizeof(resp), stdin)) {
            printf("\nCMS: IMPORT cancelled.\n");
            return 1;
        }
        if (!(resp[0] == 'Y' || resp[0] == 'y')) {
            printf("CMS: IMPORT cancelled by user.\n");
            return 1;
        }
    }

    // Overwrite existing IDs or append new ones
    for (int t = 0; t < tmp_count; ++t) {
        int found = 0;
        for (int i = 0; i < *count; ++i) {
            if (records[i].id == tmp[t].id) {
                records[i] = tmp[t];
                found = 1;
                break;
            }
        }
        if (!found) {
            if (*count >= MAX_RECORDS) break;
            records[*count] = tmp[t];
            (*count)++;
        }
    }

    // Confirmation message
    printf("Imported successfully!\n");
    char msg_imp[HISTORY_DESC_LEN]; 
    snprintf(msg_imp, sizeof(msg_imp), "IMPORT: Imported file '%s' (%d rows)", fname, 0);
    addHistory(msg_imp);
    return 1;
}

