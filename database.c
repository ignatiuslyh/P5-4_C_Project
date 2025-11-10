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
        printf("CMS: Unable to open file - %s \n", filename);
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
            printf("CMS: File - %s was not closed properly. Data may be incomplete");
            return -1;
        }
        printf("CMS: The database file - %s was succcessfully opened.\n", filename);
        return 1;
}

// Rmb to make sure StudentRecord is read-only
int saveDB(const char *filename, const StudentRecord records[], int count) 
{
    // TODO
    // 1. Open the file for writing ("w").
    // The "w" mode will CREATE the file if it doesn't exist or TRUNCATE (delete content) if it does.
    // FILE *fp = fopen(filename, "w");

    // 1a. Check for initial file open error.
    // IF fp is NULL, THEN print error message ("CMS: Unable to write to file...") and RETURN 0.

    // 2. Write records in a loop.
    // FOR loop from i = 0 up to (count - 1):
        // Use fprintf to write the data for records[i] to the file pointer fp.
        // The format should match how loadDatabase expects to read it, e.g.:
        // fprintf(fp, "%d %s %s %.1f\n", ...); // Note: Use \n to put each record on a new line.

        // Optional: Check the return value of fprintf. If it's negative, a write error occurred.

    // 3. Cleanup: Close the file and check for closing errors.
    // close_status = fclose(fp);

    // IF close_status is EOF, THEN print critical error message and RETURN -1.

    // 4. Success.
    // Print success message ("CMS: The database file ... was successfully saved.") and RETURN 1.

    return 0; 
}