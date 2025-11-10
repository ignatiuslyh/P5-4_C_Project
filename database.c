// database.c is a File I/O focused Module. File contains functions: loadDB(), saveDB()
#include <stdio.h>
#include <string.h>

#define MAX_RECORDS 200
#define STRING_LEN 50

typedef struct {
    int id;
    char name[STRING_LEN];
    char programme[STRING_LEN];
    float mark;
} StudentRecord;

// Rmb to make sure file is read-only
int loadDB(const char *filename, StudentRecord records[], int *count)
{
    if (filename == NULL) {
        printf("CMS: Unable to open file (null filename).\n");
        return 0;
    }

    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("CMS: Unable to open file '%s'\n", filename);
        return 0;
    }

    *count = 0;

    // Temporary variables for scanned values
    int id;
    char name[STRING_LEN];
    char programme[STRING_LEN];
    float mark;

    // Read lines until EOF or until we reach MAX_RECORDS
    while (*count < MAX_RECORDS && fscanf(fp, "%d %49s %49s %f", &id, name, programme, &mark) == 4) {
        records[*count].id = id;
        strncpy(records[*count].name, name, STRING_LEN - 1);
        records[*count].name[STRING_LEN - 1] = '\0';
        strncpy(records[*count].programme, programme, STRING_LEN - 1);
        records[*count].programme[STRING_LEN - 1] = '\0';
        records[*count].mark = mark;
        (*count)++;
    }

    // If we stopped because we hit the limit, warn the user (but return success)
    if (!feof(fp) && *count == MAX_RECORDS) {
        printf("CMS: Maximum record limit of %d reached; additional entries were ignored.\n", MAX_RECORDS);
        // attempt to consume remaining lines (optional) or just proceed to close
    }

    // Check for read errors (other than EOF)
    if (ferror(fp)) {
        printf("CMS: Error while reading file '%s'.\n", filename);
        fclose(fp);
        return 0;
    }

    if (fclose(fp) == EOF) {
        printf("CMS: File '%s' was not closed properly. Data may be incomplete.\n", filename);
        return -1;
    }

    return 1;
}

// Rmb to make sure StudentRecord is read-only
int saveDB(const char *filename, const StudentRecord records[], int count) 
{
    if (filename == NULL) {
        printf("CMS: Unable to write to file (null filename).\n");
        return 0;
    }

    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("CMS: Unable to write to file: %s\n", filename);
        return 0;
    }

    for (int i = 0; i < count; ++i) {
        // write one record per line in the same format as loadDB expects
        int written = fprintf(fp, "%d %s %s %.1f\n",
                              records[i].id,
                              records[i].name,
                              records[i].programme,
                              records[i].mark);
        if (written < 0) {
            printf("CMS: Write error occurred while saving to file: %s\n", filename);
            fclose(fp);
            return 0;
        }
    }

    // check fclose status
    if (fclose(fp) == EOF) {
        printf("CMS: Critical error while closing file: %s\n", filename);
        return -1;
    }

    return 1;
}