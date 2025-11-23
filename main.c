#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h> 
#include <stdlib.h>
#include <math.h>
#include "database.h"
#include "records.h"
#include "sort.h"
#include "summary.h"
#include "banner.h"
#include "query.h"
#include "history.h"

# define REQUIRED_LENGTH 7

// UTILITY FUNCTION: displayPrompt
void displayPrompt() {
    printf("P5_4: ");
}

// Used to track whether database has been opened
static int db_opened = 0;

void parseCommand(char *input, char *command, size_t cmd_size, char *args, size_t args_size) {
    // null terminate and guard clause
    if (command && cmd_size > 0) command[0] = '\0';
    if (args && args_size > 0) args[0] = '\0';
    if (!input || !command || !args) return;

    // skip leading whitespace
    char *p = input;
    while (*p && isspace((unsigned char)*p)) p++;

    // find end of command (first whitespace or NUL)
    char *q = p;
    while (*q && !isspace((unsigned char)*q)) q++;

    // copy command (up to cmd_size-1) and uppercase
    size_t cmd_len = (size_t)(q - p);
    if (cmd_len >= cmd_size) cmd_len = cmd_size - 1;
    strncpy(command, p, cmd_len);
    command[cmd_len] = '\0';
    for (size_t i = 0; i < cmd_len; ++i) command[i] = (char)toupper((unsigned char)command[i]);

    // skip whitespace between command and args
    while (*q && isspace((unsigned char)*q)) q++;

    // copy the rest into args (up to args_size-1)
    size_t avail = args_size ? args_size - 1 : 0;
    if (avail > 0) {
        strncpy(args, q, avail);
        args[avail] = '\0';
        // trim trailing whitespace from args
        size_t alen = strlen(args);
        while (alen > 0 && isspace((unsigned char)args[alen - 1])) {
            args[--alen] = '\0';
        }
    } else if (args_size > 0) {
        args[0] = '\0';
    }
}


// Trim leading/trailing whitespace in place. Returns pointer to trimmed string.
static char *trim(char *s) {
    if (!s) return s;
    while (*s && isspace((unsigned char)*s)) s++;
    if (*s == '\0') return s;
    char *end = s + strlen(s) - 1;
    while (end > s && isspace((unsigned char)*end)) *end-- = '\0';
    return s;
}

// Case-insensitive equality for short command words
static int iequals(const char *a, const char *b) {
    if (!a || !b) return 0;
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) return 0;
        a++; b++;
    }
    return *a == *b;
}

// Print single record in a simple format
static void print_record(const StudentRecord *r) {
    if (!r) return;
    printf("%d %s %s %.1f\n", r->id, r->name, r->programme, r->mark);
}

// Fallback simple query (prints record or error)
static int fallback_query(const StudentRecord records[], int count, int id) {
    for (int i = 0; i < count; ++i) {
        if (records[i].id == id) {
            print_record(&records[i]);
            return 1;
        }
    }
    printf("CMS: ERROR: Record with ID %d not found.\n", id);
    return 0;
}

// extract_input helper function to take inputs without quotes
static void extract_input(const char *input, size_t starting_len,
                          int start_index,
                          int index_id, int index_name, int index_prog, int index_mark,
                          int key_len, int out_size, char *out_buf)
{
    // start (e.g 'ID=') to preserve spacing/case
    int start = start_index + key_len;
    // end as the nearest key position after starting_len
    int end = (int)starting_len;
    if (index_id > start_index && index_id < end) end = index_id;
    if (index_name > start_index && index_name < end) end = index_name;
    if (index_prog > start_index && index_prog < end) end = index_prog;
    if (index_mark > start_index && index_mark < end) end = index_mark;
    // Trim leading spaces
    while (start < end && isspace((unsigned char)input[start])) start++;
    // Trim trailing spaces
    while (end > start && isspace((unsigned char)input[end - 1])) end--;
    // Get input value
    int value_len = end - start;
    if (value_len >= out_size) value_len = out_size - 1;
    if (value_len > 0) memcpy(out_buf, input + start, (size_t)value_len);
    out_buf[value_len] = '\0';
}

static int isValidNames(const char *name) {
    if (!name) return 0;
    for (int i = 0; name[i] != '\0'; ++i) {
        unsigned char c = (unsigned char)name[i];
        // Allow letters, space, hyphen, apostrophe, and period
        if (!isalnum(c) && c != ' ' && c != '-' && c != '\'' && c != '.' && c != '('&& c != ')') {
            return 0; // invalid character found
        }
    }
    return 1; // all characters are valid
}

int processCommand(const char *command, char *args, StudentRecord records[], int *count, const char *default_filename) {
    if (!command || !records || !count) {
        printf("CMS: ERROR: Internal error (bad parameters).\n");
        return 1;
    }

    // Make a writable, trimmed copy of args for parsing
    char local_args[256] = { 0 };
    if (args && *args) {
        strncpy(local_args, args, sizeof(local_args) - 1);
        trim(local_args);
    }

    // OPEN 
    if (iequals(command, "OPEN")) {
        const char* file = default_filename && *default_filename ? default_filename : "P5_4-CMS.txt";
        int rc = loadDB(file, records, count);
        if (rc == 1) {
            printf("CMS: The database file \"%s\" is successfully opened.\n", file);
            db_opened = 1;
            addHistory("OPEN: Opened database file");
        }
        else { 
            printf("CMS: ERROR: The database file \"%s\" is failed to open.\n", file);
            db_opened = 0;
        }
        return 1;
    }

    // SAVE 
    if (iequals(command, "SAVE")) {
        // Ignore any filename supplied by user; always use default_filename
        const char* file = default_filename && *default_filename ? default_filename : "P5_4-CMS.txt";
        int rc = saveDB(file, records, *count);
        if (rc == 1) {
            printf("CMS: SAVE successful (%s).\n", file);
            addHistory("SAVE: Saved database file");
        }
        else printf("CMS: ERROR: SAVE unsuccessful for '%s'.\n", file);
        return 1;
    }

    // INSERT ID Name Programme Mark
    // (Name and Programme must not contain spaces)
    if (iequals(command, "INSERT")) {
        // Make a copy of input for parsing, while keeping case & spaces
        char input[256] = {0};
        strncpy(input, local_args, sizeof(input) - 1);
        size_t starting_len = strlen(input);
        size_t length;

        // Find starting location of each column in input
        char *input_id = strstr(input, "ID=");
        char *input_name = strstr(input, "Name=");
        char *input_prog = strstr(input, "Programme=");
        char *input_mark = strstr(input, "Mark=");

        // Return error if column names are not in correct format
        if (!input_id || !input_name || !input_prog || !input_mark) {
            printf("CMS: Invalid INSERT. Keys must be exactly: ID= Name= Programme= Mark=\n");
            return 1;
        }

        // Find the starting index for each column for extract_input
        int index_id = (int)(input_id - input);
        int index_name = (int)(input_name - input);
        int index_prog = (int)(input_prog - input);
        int index_mark = (int)(input_mark - input);

        // Allocate size for each input
        char idstr[32] = {0};
        char namestr[STRING_LEN] = {0};
        char progstr[STRING_LEN] = {0};
        char markstr[32] = {0};

        // Use extract_value helper to extract each input
        extract_input(input, starting_len, index_id, index_id, index_name, index_prog, index_mark, (int)strlen("ID="), sizeof(idstr), idstr);
        extract_input(input, starting_len, index_name, index_id, index_name, index_prog, index_mark, (int)strlen("Name="), sizeof(namestr), namestr);
        extract_input(input, starting_len, index_prog, index_id, index_name, index_prog, index_mark, (int)strlen("Programme="), sizeof(progstr), progstr);
        extract_input(input, starting_len, index_mark, index_id, index_name, index_prog, index_mark, (int)strlen("Mark="), sizeof(markstr), markstr);

        // Make sure there are no empty inputs
        if (idstr[0] == '\0' || namestr[0] == '\0' || progstr[0] == '\0' || markstr[0] == '\0') {
            printf("CMS: ERROR: Invalid INSERT. Use: INSERT ID=<id> Name=<name> Programme=<programme> Mark=<mark>\n");
            return 1;
        }

        // Make sure that ID must be 7 characters long
        length = strlen(idstr);
        if (length != REQUIRED_LENGTH) {
            printf("CMS: ERROR: The ID must be 7 characters long.\n ");
            return 1;
        }

        // Validation for numeric values
        int id = 0;
        float mark = 0.0f;
        if (sscanf(idstr, "%d", &id) != 1) {
            printf("CMS: ERROR: Invalid ID value.\n");
            return 1;
        }
        if (sscanf(markstr, "%f", &mark) != 1) {
            printf("CMS: ERROR: Invalid Mark value.\n");
            return 1;
        }
        // Round to 1 decimal point
         mark = round(mark * 10) / 10.0;
        // Marks only between 0.0 and 100.0
        if (mark < 0.0f || mark > 100.0f) {
            printf("CMS: ERROR: Mark must be between 0.0 and 100.0.\n");
            return 1;
        }
         
        // Turn values into StudentRecord for database
        StudentRecord sr;
        sr.id = id;
        strncpy(sr.name, namestr, STRING_LEN - 1); sr.name[STRING_LEN - 1] = '\0';
        strncpy(sr.programme, progstr, STRING_LEN - 1); sr.programme[STRING_LEN - 1] = '\0';
        sr.mark = mark;

        if (!insertRecord(records, count, &sr)) {
            
        } else {
            printf("CMS: INSERT successful (ID %d).\n", id);
        }
        return 1;
    }

    // IMPORT filename.csv
    // BULK INSERT rows from CSV file
    if (iequals(command, "IMPORT")) {
        // Ensure DB is opened this session
        if (!db_opened) {
            printf("CMS: ERROR: No database opened. Use OPEN before IMPORT.\n");
            return 1;
        }
 
        // Make sure IMPORT contains filename
        if (!local_args || !local_args[0]) {
            printf("CMS: ERROR: IMPORT requires a filename. Usage: IMPORT file.csv\n");
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
            printf("CMS: ERROR: Unable to open file '%s' for import.\n", fname);
            return 1;
        }
 
        // Temporary storage for parsed rows
        StudentRecord tmp[MAX_RECORDS];
        int tmp_count = 0;
        int dup_count = 0;
 
        // Read file line by line
        char line[512];
        while (fgets(line, sizeof(line), fp)) {
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
 
            // Skip missing fields
            if (!f0 || !f1 || !f2 || !f3) continue;
 
            // Trim whitespace from each field
            char *p0 = trim(f0);
            char *p1 = trim(f1);
            char *p2 = trim(f2);
            char *p3 = trim(f3);
 
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
 
            // Warn user if there are duplicates
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
            printf("CMS: No valid rows found in \"%s\". Nothing imported.\n", fname);
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
 
        // Apply rows: overwrite existing IDs or append new ones
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
        return 1;
    }
 
        // QUERY
       // Uses ID to search for record
    if (iequals(command, "QUERY")) {

        char buf[256];
        strncpy(buf, local_args, sizeof(buf) - 1);
        buf[sizeof(buf) - 1] = '\0';
        // Catch unexpected spaces by removing all spaces
        char clean[256];
        int j = 0;
        for (int i = 0; buf[i] != '\0' && j < 255; i++) {
            if (!isspace((unsigned char)buf[i])) {
                clean[j++] = buf[i];
            }
        }
        clean[j] = '\0';
        // Modified ID search based on the expected format
        char* id_str = strstr(clean, "ID=");
        if (!id_str) {
            id_str = strstr(clean, "id=");
        }
        if (id_str) {
            int id = atoi(id_str + 3);
            int found = queryRecord(records, *count, id);

            // Add to history - track both successful and failed queries
            if (found) {
                char msg[HISTORY_DESC_LEN];
                snprintf(msg, sizeof(msg), "QUERY: Found record ID=%d", id);
                addHistory(msg);
            }
            else {
                char msg[HISTORY_DESC_LEN];
                snprintf(msg, sizeof(msg), "QUERY: Attempted search for ID=%d (not found)", id);
                addHistory(msg);
            }
        }
        else {
            printf("CMS: ERROR: Invalid QUERY format. Use: QUERY ID=<student_id>\n");
        }
        return 1;
    }

    /*
    // QUERY ID
    if (iequals(command, "QUERY")) {
        int id;
        if (sscanf(local_args, "%d", &id) == 1) {
            #ifdef HAVE_QUERY_RECORD
            if (!queryRecord(records, *count, id)) {
                printf("CMS: ERROR: Record with ID %d not found.\n", id);
            }
            #else
            fallback_query(records, *count, id);
            #endif
        } else {
            printf("CMS: ERROR: Invalid QUERY. Use: QUERY <ID>\n");
        }
        return 1;
    }
*/
   // UPDATE ID= <ID> FIELD =<VALUE>
    if (iequals(command, "UPDATE")) {
    
    size_t slen = strlen(local_args);

    // find key positions in the original-cased local_args (case-sensitive)
    const char *p_id  = strstr(local_args, "ID=");
    const char *p_name = strstr(local_args, "Name=");
    const char *p_prog = strstr(local_args, "Programme=");
    const char *p_mark = strstr(local_args, "Mark=");

    //ensure that ID= is present; 
    if (!p_id) {
        printf("CMS: UPDATE requires ID=\n");
        return 1;
    }

    // convert pointers to integer indices
    int idx_id  = p_id  ? (int)(p_id  - local_args) : -1;
    int idx_name = p_name ? (int)(p_name - local_args) : -1;
    int idx_prog = p_prog ? (int)(p_prog - local_args) : -1;
    int idx_mark = p_mark ? (int)(p_mark - local_args) : -1;

    // validate that user enter at least one field
    int field_count = 0;
    if (p_name) field_count++;
    if (p_prog) field_count++;
    if (p_mark) field_count++;

    if (field_count == 0) {
        printf("CMS:At least ONE field must be updated (Name, Programme, or Mark).\n");
        return 1;
    }

    if (field_count > 1) {
        printf("CMS: UPDATE allows only ONE field (Name, Programme, or Mark).\n");
        return 1;
    }

    // buffers for extracted values
    char id_buf[32] = {0};
    char name_buf[128] = {0};
    char prog_buf[64] = {0};
    char mark_buf[32] = {0};

    // extract each value using the existing helper (preserve spaces)
    extract_input(local_args, slen,idx_id, idx_id, idx_name, idx_prog, idx_mark,(int)strlen("ID="), sizeof(id_buf), id_buf);
    int id = atoi(id_buf);

    // Extract for Name
    if (idx_name != -1) {
        extract_input(local_args, slen,idx_name, idx_id, idx_name, idx_prog, idx_mark,(int)strlen("Name="), sizeof(name_buf), name_buf);
    }
    // Extract for Programme
    if (idx_prog != -1) {
        extract_input(local_args, slen,idx_prog, idx_id, idx_name, idx_prog, idx_mark,(int)strlen("Programme="), sizeof(prog_buf), prog_buf);
    }
    // Extract for Mark
    if (idx_mark != -1) {
        extract_input(local_args, slen,idx_mark, idx_id, idx_name, idx_prog, idx_mark,(int)strlen("Mark="), sizeof(mark_buf), mark_buf);
    }

    // Updates record
    int found = 0;
    for (int i = 0; i < *count; ++i) {
        // check if the student id is in the database, if it is excute the below code
        if (records[i].id == id) {
            found = 1;
            // validate characters for name to contain the following letter,space,Apostrophe,Slash,Parentheses and cannot be null
            if (idx_name != -1) {
                if (!isValidNames(name_buf)) {
                    printf("CMS: Invalid characters in Name. Allowed: letters, space, -, ',(, ).\n");
                    return 1;
                }

                if (name_buf[0] == '\0') {
                    printf("CMS: Name field is empty. Use: UPDATE ID=<id> Name=<name>\n");
                    return 1;
                }
                strncpy(records[i].name, name_buf, STRING_LEN-1);
            }

            // validate characters in Programme to contain the following letter,space,Apostrophe,Slash,Parentheses and cannot be null
            if (idx_prog != -1) {
                if (!isValidNames(prog_buf)) {
                     printf("CMS: Invalid characters in Programme. Allowed: letters, space, -, ',(, ).\n");
                    return 1;
                }

                 if (prog_buf[0] == '\0') {
                        printf("CMS: Programme field is empty.Use: UPDATE ID=<id> Programme=<programme>\n");
                        return 1;
                    }
                strncpy(records[i].programme, prog_buf, STRING_LEN-1);
            }
            // validate marks field to not contain letter and mark goes from between 0 to 100 also update to 1 decimal point
            if (idx_mark != -1) {
                float m;
                if (sscanf(mark_buf, "%f", &m) != 1) {
                    printf("CMS: Invalid Mark type\n");
                    return 1;
                }

                if (m < 0 || m > 100) {
                    printf("CMS: ERROR: Mark out of range (0-100)\n");
                    return 1;
                
                }

                if (mark_buf[0] == '\0') {
                    printf("CMS: Mark field is empty. Use: UPDATE ID=<id> Mark=<mark>\n");
                    return 1;
                }
                // round marks to 1D.P
                m = round(m * 10) / 10.0;
                records[i].mark = m;
            }

            printf("CMS: The record with ID=%d is successfully updated.\n", id);
            break;
        }
    }
    // student id does not exist, it will print out error message
    if (!found) {
        printf("CMS: The record with ID=%d does not exist.\n", id);
    }

    return 1;
}


    // DELETE ID
    if (iequals(command, "DELETE")) {
            char buf[256]; 
            strncpy(buf, local_args, sizeof(buf) - 1);
            buf[sizeof(buf) - 1] = '\0';

            // Remove spaces to make key detection simpler, keep a clean copy
            char clean[256];
            int j = 0;
            for (int i = 0; buf[i] != '\0' && j < 255; ++i) {
                if (!isspace((unsigned char)buf[i])) clean[j++] = buf[i];
            }
            clean[j] = '\0';            
            
            char *p = strstr(clean, "ID=");
            if (!p) p = strstr(clean, "id="); // case-insensitive fallback
            if (!p || strlen(p) < 4) {
                printf("CMS: ERROR: Invalid DELETE. Use: DELETE ID=<ID>\n");
                return 1;
            }
            int id = atoi(p + 3);

            int idx = findRecordById(records, *count, id);
            if (idx == -1) {
                printf("CMS: The record with ID=%d does not exist.\n", id);
                return 1;
            }

            // Ask for confirmation with exact wording
            printf("CMS: Are you sure you want to delete record with ID=%d? Type \"Y\" to Confirm or type \"N\" to cancel.\n", id);
            fflush(stdout);

            displayPrompt();
            
            char resp[16] = {0};
            if (!fgets(resp, sizeof(resp), stdin)) {
                printf("CMS: The deletion is cancelled.\n");
                return 1;
            }

                if (resp[0] == 'Y' || resp[0] == 'y') {
            #ifdef HAVE_DELETE_RECORD
                    if (!deleteRecord(records, count, id)) {
                        printf("CMS: ERROR: DELETE failed (not found).\n");
                    } else {
                        printf("CMS: The record with ID=%d is successfully deleted.\n", id);
                        char msg[HISTORY_DESC_LEN];
                        snprintf(msg, sizeof(msg), "DELETE: Deleted record ID=%d", id);
                        addHistory(msg);   
                    }
#else
        // Fallback delete logic if your build doesn't have HAVE_DELETE_RECORD
            for (int i = idx; i + 1 < *count; ++i) records[i] = records[i + 1];
            (*count)--;
            printf("CMS: The record with ID=%d is successfully deleted.\n", id);
            char msg[HISTORY_DESC_LEN];
            snprintf(msg, sizeof(msg), "DELETE: Deleted record ID=%d", id);
            addHistory(msg);
#endif
            } else { 
                printf("CMS: The deletion is cancelled.\n");
            }
            return 1;
    }


        // SHOW ALL | SHOW ALL SORT BY ID | SHOW ALL SORT BY MARK | SHOW SUMMARY
        if (iequals(command, "SHOW")) {
            // copy and trim arguments
            char buf[128];
            strncpy(buf, local_args, sizeof(buf) - 1);
            buf[sizeof(buf) - 1] = '\0';
            trim(buf);

            // SHOW or SHOW ALL
            if (buf[0] == '\0' || iequals(buf, "ALL")) {
                showAllRecords(records, *count);
                addHistory("SHOW ALL: Displayed all records");
                return 1;
            }

            // SHOW SUMMARY
            if (iequals(buf, "SUMMARY")) {
                showSummary(records, *count);
                return 1;
            }

            // parse into up to 5 tokens
            char t1[16] = { 0 }, t2[16] = { 0 }, t3[16] = { 0 }, t4[16] = { 0 }, t5[16] = { 0 };
            int n = sscanf(buf, "%15s %15s %15s %15s %15s", t1, t2, t3, t4, t5);

            // expect: ALL SORT BY <FIELD> [ORDER]
            if (n >= 4 && iequals(t1, "ALL") && iequals(t2, "SORT") && iequals(t3, "BY")) {
                char* field = t4;
                char* order = (n >= 5) ? t5 : NULL;

                if (!iequals(field, "ID") && !iequals(field, "MARK")) {
                    printf("CMS: ERROR: Invalid SHOW SORT field '%s'. Use ID or MARK.\n", field);
                    return 1;
                }

                int by_id = iequals(field, "ID") ? 1 : 0;
                int asc = 1; // default ascending
                if (order) {
                    if (iequals(order, "DESC")) asc = 0;
                    else if (iequals(order, "ASC")) asc = 1;
                    else {
                        printf("CMS: ERROR: Unknown sort order '%s'. Use ASC or DESC.\n", order);
                        return 1;
                    }
                }

                sort_and_print(records, *count, by_id, asc);
                return 1;
            }

            printf("CMS: ERROR: Invalid SHOW command.\n");
            return 1;
        }

        // HISTORY
        // Show last 20 (max)
        if (iequals(command, "HISTORY")) {
            int n = 5; // default
            if (local_args[0] != '\0') {
                n = atoi(local_args);
                if (n <= 0) n = 5; // fallback to default
            }
            showHistory(n);
            return 1;
        }

        // EXIT / QUIT
        if (iequals(command, "EXIT") || iequals(command, "QUIT")) {
            printf("DEBUG: Checking EXIT/QUIT. Command is: '%s'\n", command);
            printf("CMS: Program exiting.\n");
            saveHistoryToFile();
            return 0;
        }

        // Unknown command
        printf("CMS: ERROR: Unknown command '%s'.\n", command);
        return 1;
    }


int main(void) {
    StudentRecord records[MAX_RECORDS];    
    int record_count = 0;                 
    const char *filename = "P5_4-CMS.txt"; // default DB filename

    enum { MAX_CMD_LEN = 512, CMD_WORD_LEN = 32, ARGS_LEN = 480 };
    char input_buffer[MAX_CMD_LEN];
    char command[CMD_WORD_LEN];
    char arguments[ARGS_LEN];

    int running = 1;

    initHistory(); // Initialise History Function

    if (loadDB(filename, records, &record_count) == 1) {
        printBanner();
    } else {
        printf("CMS: No database loaded (starting with empty database). Use OPEN or INSERT to add records.\n");
    }

    // Main command loop
    while (running) {
        displayPrompt();

        // Read a line from stdin; handle EOF (Ctrl-D)
        if (!fgets(input_buffer, sizeof(input_buffer), stdin)) {
            putchar('\n'); // keep prompt formatting tidy on EOF
            break;
        }

        // Remove trailing newline if present
        size_t L = strlen(input_buffer);
        if (L > 0 && input_buffer[L - 1] == '\n') input_buffer[L - 1] = '\0';

        // Parse into command and args (command will be uppercased)
        parseCommand(input_buffer, command, sizeof(command), arguments, sizeof(arguments));

        // If user entered empty line, continue
        if (command[0] == '\0') continue;

        // Dispatch command. processCommand returns 0 to exit, 1 to continue.
        running = processCommand(command, arguments, records, &record_count, filename);
    }

    printf("CMS: Program exiting. If you want to save changes run 'SAVE' before exit next time.\n");

    saveHistoryToFile();

    return 0;
}