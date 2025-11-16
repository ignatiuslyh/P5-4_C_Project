// database.c is a File I/O focused Module. File contains functions: loadDB(), saveDB()
#include <stdio.h>
#include <string.h>

#include "records.h"

// Rmb to make sure file is read-only
int loadDB(const char *filename, StudentRecord records[], int *count)
{
    if (!filename) {
        printf("CMS: Unable to open file (null filename).\n");
        return 0;
    }

    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("CMS: Unable to open file '%s'\n", filename);
        return 0;
    }

    char line[512];
    *count = 0;

    while (*count < MAX_RECORDS && fgets(line, sizeof(line), fp)) {
        size_t len = strlen(line);
        
        while (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r')) line[--len] = '\0';
        if (len == 0) continue; // skip blank lines

    
        char *s = line;
        while (*s && isspace((unsigned char)*s)) s++;

        // skip non-data lines (metadata or header). Data lines start with a digit (ID).
        if (!isdigit((unsigned char)*s)) {
            // also skip header like "ID\tName..." 
            if (strncmp(s, "ID\t", 3) == 0 || strncmp(s, "ID ", 3) == 0) continue;
            continue;
        }

        // Try to parse tab-separated: ID<TAB>Name<TAB>Programme<TAB>Mark 
        int id = 0;
        char name_buf[STRING_LEN];
        char prog_buf[STRING_LEN];
        float mark = 0.0f;

        int matched = sscanf(s, "%d\t%49[^\t]\t%49[^\t]\t%f", &id, name_buf, prog_buf, &mark);
        if (matched != 4) {
            // fallback: try whitespace-separated tokens (names/programme without spaces) 
            matched = sscanf(s, "%d %49s %49s %f", &id, name_buf, prog_buf, &mark);
            if (matched != 4) continue; // could not parse; skip line 
        }

        // store record safely 
        records[*count].id = id;
        strncpy(records[*count].name, name_buf, STRING_LEN - 1);
        records[*count].name[STRING_LEN - 1] = '\0';
        strncpy(records[*count].programme, prog_buf, STRING_LEN - 1);
        records[*count].programme[STRING_LEN - 1] = '\0';
        records[*count].mark = mark;
        (*count)++;
    }

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

int saveDB(const char *filename, const StudentRecord records[], int count)
{
    if (!filename) {
        printf("CMS: Unable to write to file (null filename).\n");
        return 0;
    }

    FILE *fp = fopen(filename, "w");
    if (!fp) {
        printf("CMS: Unable to write to file: %s\n", filename);
        return 0;
    }

    // save as tab-separated to preserve spaces inside name/programme 
    for (int i = 0; i < count; ++i) {
        if (fprintf(fp, "%d\t%s\t%s\t%.1f\n",
                    records[i].id,
                    records[i].name,
                    records[i].programme,
                    records[i].mark) < 0) {
            printf("CMS: Write error occurred while saving to file: %s\n", filename);
            fclose(fp);
            return 0;
        }
    }

    if (fclose(fp) == EOF) {
        printf("CMS: Critical error while closing file: %s\n", filename);
        return -1;
    }
    return 1;
}