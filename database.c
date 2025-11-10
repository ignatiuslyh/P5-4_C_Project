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
    FILE *ptr = fopen(filename, "r");
    if (ptr == NULL)
    {
        printf("CMS: Unable to open file '%s' \n", filename);
        return 0;
    }

    *count = 0;

    // Read and store the ID, name, programme in the correct fields of our array if the 4 correct datatypes are found
    while(fscanf(ptr, "%d %49s %49s %f",  
        &records[*count].id, records[*count].name, 
        records[*count].programme, &records[*count].mark) == 4) 
        {
            if (*count < MAX_RECORDS - 1)
            {
                (*count)++;
            }
            else{
                printf("CMS: Maximum record limit of %d reached \n", MAX_RECORDS);
            }
        }  
        
        // Check if any errors occurred during fileclose
        int close_status = fclose(ptr);
        if(close_status == EOF)
        {
            printf("CMS: File '%s' was not closed properly. Data may be incomplete");
            return -1;
        }
        printf("CMS: The database file '%s' was succcessfully opened.\n", filename);
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

    if (fclose(fp) == EOF) {
        printf("CMS: Critical error while closing file: %s\n", filename);
        return -1;
    }

    printf("CMS: The database file '%s' was successfully saved.\n", filename);
    return 1;
}