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
#include "history.h"

# define REQUIRED_LENGTH 7


static void printDeclaration(void) {
    static const char decl[] =
        "\n Declaration\n\n"
        "SIT's policy on copying does not allow the students to copy source code as well as assessment solutions from another person, AI, or other places. "
        "It is the students' responsibility to guarantee that their assessment solutions are their own work. Meanwhile, the students must also ensure that their work is not accessible by others. "
        "Where such plagiarism is detected, both of the assessments involved will receive ZERO mark.\n\n"
        "We hereby declare that:\n\n"
        "- We fully understand and agree to the abovementioned plagiarism policy.\n\n"
        "- We did not copy any code from others or from other places.\n\n"
        "- We did not share our codes with others or upload to any other places for public access and will not do that in the future.\n\n"
        "- We agree that our project will receive Zero mark if there is any plagiarism detected.\n\n"
        "- We agree that we will not disclose any information or material of the group project to others or upload to any other places for public access.\n\n"
        "- We agree that we did not copy any code directly from AI generated sources.\n\n"
        "Declared by: Group Name (P5-4)\n\n"
        "Team members:\n\n"
        "1. LEOW YI HAO IGNATIUS\n"
        "2. KUAH CHIN YANG, EDDISON\n"
        "3. LIM SI YUN\n"
        "4. LIAO XUE E\n"
        "5. LAZER LIDEON RAJA\n\n"
        "Date: 25/11/2025\n\n";
    fputs(decl, stdout);
}


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
            char msg[HISTORY_DESC_LEN];
            snprintf(msg, sizeof(msg), "OPEN: Failed to open %s", file);
            addHistory(msg);
        }
        return 1;
    }

    // SAVE 
    if (iequals(command, "SAVE")) {
        // Ignore any filename supplied by user; always use default_filename
        const char* file = default_filename && *default_filename ? default_filename : "P5_4-CMS.txt";
        int rc = saveDB(file, records, *count);
        if (rc == 1) {
            printf("CMS: The database file \"%s\" is successfully saved.\n", file);
            addHistory("SAVE: Saved database file");
        }
        else {
            printf("CMS: ERROR: SAVE unsuccessful for '%s'.\n", file);
            char msg[HISTORY_DESC_LEN];
            snprintf(msg, sizeof(msg), "SAVE: Failed to save %s", file);
            addHistory(msg);
        }
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

        // Find the starting index for each column for extract_input
        int index_id = input_id ? (int)(input_id - input) : -1;
        int index_name = input_name ? (int)(input_name - input) : -1;
        int index_prog = input_prog ? (int)(input_prog - input) : -1;
        int index_mark = input_mark ? (int)(input_mark - input) : -1;

        // Allocate size for each input
        char idstr[32] = {0};
        char namestr[STRING_LEN] = {0};
        char progstr[STRING_LEN] = {0};
        char markstr[32] = {0};

        // Check ID for duplicates first
        if (input_id) {
            extract_input(input, starting_len, index_id,
                          index_id, index_name, index_prog, index_mark,
                          (int)strlen("ID="), sizeof(idstr), idstr);

            if (idstr[0] != '\0') {
                int tmpid = 0;
                if (sscanf(idstr, "%d", &tmpid) == 1) {
                    if (findRecordById(records, *count, tmpid) != -1) {
                        printf("CMS: The record with ID=%d already exists.\n", tmpid);
                        char msg[HISTORY_DESC_LEN];
                        snprintf(msg, sizeof(msg), "INSERT: Failed - duplicate ID=%d", tmpid);
                        addHistory(msg);
                        return 1;
                    }
                }
            }
        }

        // Require exact case keys be present; otherwise error
        if (!input_id || !input_name || !input_prog || !input_mark) {
            printf("CMS: Invalid INSERT. Keys must be exactly: ID= Name= Programme= Mark=\n");
            addHistory("INSERT: Failed - invalid keys");
            return 1;
        }

        // Use extract_input helper to extract each input
        extract_input(input, starting_len, index_id, index_id, index_name, index_prog, index_mark, (int)strlen("ID="), sizeof(idstr), idstr);
        extract_input(input, starting_len, index_name, index_id, index_name, index_prog, index_mark, (int)strlen("Name="), sizeof(namestr), namestr);
        extract_input(input, starting_len, index_prog, index_id, index_name, index_prog, index_mark, (int)strlen("Programme="), sizeof(progstr), progstr);
        extract_input(input, starting_len, index_mark, index_id, index_name, index_prog, index_mark, (int)strlen("Mark="), sizeof(markstr), markstr);

        // Make sure there are no empty inputs
        if (idstr[0] == '\0' || namestr[0] == '\0' || progstr[0] == '\0' || markstr[0] == '\0') {
            printf("CMS: Invalid INSERT. Use: INSERT ID=<id> Name=<name> Programme=<programme> Mark=<mark>\n");
            addHistory("INSERT: Failed - missing field(s)");
            return 1;
        }

        // Make sure that ID must be 7 characters long
        length = strlen(idstr);
        if (length != REQUIRED_LENGTH) {
            printf("CMS: The ID must be 7 characters long.\n ");
            char msg[HISTORY_DESC_LEN];
            snprintf(msg, sizeof(msg), "INSERT Failed - ID length wrong '%s'", idstr);
            addHistory(msg);
            return 1;
        }

        // Parse numeric values
        int id = 0;
        float mark = 0.0f;
        if (sscanf(idstr, "%d", &id) != 1) {
            printf("CMS: Invalid ID value.\n");
            addHistory("INSERT Failed - invalid ID value");
            return 1;
        }
        if (sscanf(markstr, "%f", &mark) != 1) {
            printf("CMS: Invalid Mark value. Mark must be a number.\n");
            addHistory("INSERT Failed - invalid Mark value.");
            return 1;
        }
        // Round to 1 decimal point
        mark = round(mark * 10) / 10.0;
        // Marks only between 0.0 and 100.0
        if (mark < 0.0f || mark > 100.0f) {
            printf("CMS: Mark must be between 0.0 and 100.0.\n");
            addHistory("INSERT: Failed - mark out of range");
            return 1;
        }

        // Turn values into StudentRecord for database
        StudentRecord sr;
        sr.id = id;
        strncpy(sr.name, namestr, STRING_LEN - 1);
        sr.name[STRING_LEN - 1] = '\0';
        strncpy(sr.programme, progstr, STRING_LEN - 1);
        sr.programme[STRING_LEN - 1] = '\0';
        sr.mark = mark;

        if (!insertRecord(records, count, &sr)) {
            char msg[HISTORY_DESC_LEN];
            addHistory(msg);
        } else {
            printf("INSERT successful (ID %d).\n", id);
            char msg[HISTORY_DESC_LEN];
            snprintf(msg, sizeof(msg), "INSERT: Inserted record ID=%d", id);
            addHistory(msg);
        }
        return 1;
    }

    // IMPORT filename.csv
    // BULK INSERT rows from CSV file
    if (iequals(command, "IMPORT")) {
        if (!db_opened) {
            printf("CMS: No database opened. Use OPEN before IMPORT.\n");
            addHistory("IMPORT: Failed - no DB opened");
            return 1;
        }
        return importRecords(local_args, records, count);
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
            addHistory("QUERY: Failed - invalid format");
        }
        return 1;
    }

   // UPDATE ID= <ID> FIELD =<VALUE>
    if (iequals(command, "UPDATE")) {
    
    size_t slen = strlen(local_args);

    char msg[HISTORY_DESC_LEN]; 

    // find key positions in the original-cased local_args (case-sensitive)
    const char *p_id  = strstr(local_args, "ID=");
    const char *p_name = strstr(local_args, "Name=");
    const char *p_prog = strstr(local_args, "Programme=");
    const char *p_mark = strstr(local_args, "Mark=");

    //ensure that ID= is present; 
    if (!p_id) {
        printf("CMS: UPDATE requires ID=\n");
        addHistory("UPDATE: Failed - missing ID\n");
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
        printf("CMS: At least ONE field must be updated (Name, Programme, or Mark).\n");
        printf("UPDATE: Failed - no fields specified\n");
        return 1;
    }

    if (field_count > 1) {
        printf("CMS: UPDATE allows only ONE field (Name, Programme, or Mark).\n");
        printf("UPDATE: Failed - multiple fields specified\n");
        return 1;
    }

    // buffers for extracted values
    char id_buf[32] = {0};
    char name_buf[128] = {0};
    char prog_buf[64] = {0};
    char mark_buf[32] = {0};

    // extract ID using the extract_input helper functions
    extract_input(local_args, slen,idx_id, idx_id,idx_name, idx_prog, idx_mark, (int)strlen("ID="), sizeof(id_buf), id_buf);

    int id = atoi(id_buf);

    // Extract for Name
    if (idx_name != -1) {
        extract_input(local_args, slen,idx_name, idx_id, idx_name, idx_prog, idx_mark, (int)strlen("Name="), sizeof(name_buf), name_buf);
    }

    // Extract for Programme
    if (idx_prog != -1) {
        extract_input(local_args, slen,idx_prog, idx_id, idx_name, idx_prog, idx_mark, (int)strlen("Programme="), sizeof(prog_buf), prog_buf);
    }

    // Extract for Mark
    if (idx_mark != -1) {
        extract_input(local_args, slen, idx_mark, idx_id, idx_name, idx_prog, idx_mark, (int)strlen("Mark="), sizeof(mark_buf), mark_buf);
    }

    // Validate that the value for Name Field is not empty
    if (idx_name != -1 && name_buf[0] == '\0') {
        printf("CMS: Name field is empty. Use: UPDATE ID=<id> Name=<name>\n");
        char msg[HISTORY_DESC_LEN]; 
        snprintf(msg, sizeof(msg), "UPDATE: Failed - empty Name for ID=%d", id); 
        addHistory(msg);
        return 1;
    }
    // char msg[HISTORY_DESC_LEN]; 
    snprintf(msg, sizeof(msg), "UPDATE: Updated Name for ID=%d", id);
    addHistory(msg);

    // Validate that the value for Programme Field is not empty
    if (idx_prog != -1 && prog_buf[0] == '\0') {
        printf("CMS: Programme field is empty. Use: UPDATE ID=<id> Programme=<programme>\n");
        char msg[HISTORY_DESC_LEN]; 
        snprintf(msg, sizeof(msg), "UPDATE: Failed - empty Programme for ID=%d", id); 
        addHistory(msg);
        return 1;
    }
    // char msg[HISTORY_DESC_LEN]; 
    snprintf(msg, sizeof(msg), "UPDATE: Updated Programme for ID=%d", id);
    addHistory(msg);

    // Validate that Mark is not empty, contains only numeric input, is within 0–100, and is rounded to one decimal place.
    if (idx_mark != -1) {
        float m;
        if (mark_buf[0] == '\0') {
            printf("CMS: Mark field is empty. Use: UPDATE ID=<id> Mark=<mark>\n");
            char msg[HISTORY_DESC_LEN]; 
            snprintf(msg, sizeof(msg), "UPDATE: Failed - empty mark for ID=%d", id); 
            addHistory(msg);
            return 1;
        }

        if (sscanf(mark_buf, "%f", &m) != 1) {
            printf("CMS: Invalid Mark type.\n");
            char msg[HISTORY_DESC_LEN]; 
            snprintf(msg, sizeof(msg), "UPDATE: Failed - invalid mark for ID=%d", id); 
            addHistory(msg);
            return 1;
        }

        if (m < 0 || m > 100) {
            printf("CMS: ERROR: Mark out of range (0-100)\n");
            char msg[HISTORY_DESC_LEN]; 
            snprintf(msg, sizeof(msg), "UPDATE: Failed - mark out of range for ID=%d", id); 
            addHistory(msg);
            return 1;
        }

        // round marks to 1D.P
        m = round(m * 10) / 10.0;
        idx_mark = m;
        char msg[HISTORY_DESC_LEN]; 
        snprintf(msg, sizeof(msg), "UPDATE: Updated Mark for ID=%d", id); 
        addHistory(msg);
    }

    char fieldType[32];
    char valueBuf[128];

    if (idx_name != -1) {
        strcpy(fieldType, "Name");
        strcpy(valueBuf, name_buf);
    }
    else if (idx_prog != -1) {
        strcpy(fieldType, "Programme");
        strcpy(valueBuf, prog_buf);
    }
    else if (idx_mark != -1) {
        strcpy(fieldType, "Mark");
        strcpy(valueBuf, mark_buf);
    }

    updateRecord(records, count, id, fieldType, valueBuf);

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
                addHistory("DELETE: Failed - invalid format");
                return 1;
            }
            int id = atoi(p + 3);

            int idx = findRecordById(records, *count, id);
            if (idx == -1) {
                printf("CMS: The record with ID=%d does not exist.\n", id);
                char msg[HISTORY_DESC_LEN]; 
                snprintf(msg, sizeof(msg), "DELETE: Attempted delete ID=%d (not found)", id); 
                addHistory(msg);
                return 1;
            }

            // Ask for confirmation with exact wording
            printf("CMS: Are you sure you want to delete record with ID=%d? Type \"Y\" to Confirm or type \"N\" to cancel.\n", id);
            fflush(stdout);

            displayPrompt();
            
            char resp[16] = {0};
            if (!fgets(resp, sizeof(resp), stdin)) {
                printf("CMS: The deletion is cancelled.\n");
                addHistory("DELETE: Cancelled (no response)");
                return 1;
            }

                if (resp[0] == 'Y' || resp[0] == 'y') {
            #ifdef HAVE_DELETE_RECORD
                    if (!deleteRecord(records, count, id)) {
                        printf("CMS: ERROR: DELETE failed (not found).\n");
                        char msg[HISTORY_DESC_LEN]; 
                        snprintf(msg, sizeof(msg), "DELETE: Failed for ID=%d", id); 
                        addHistory(msg);
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
                char msg[HISTORY_DESC_LEN]; 
                snprintf(msg, sizeof(msg), "DELETE: Cancelled for ID=%d", id); 
                addHistory(msg);
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
                addHistory("SHOW SUMMARY: Displayed summary");
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
                    addHistory("SHOW: Failed - invalid sort field");
                    return 1;
                }

                int by_id = iequals(field, "ID") ? 1 : 0;
                int asc = 1; // default ascending
                if (order) {
                    if (iequals(order, "DESC")) asc = 0;
                    else if (iequals(order, "ASC")) asc = 1;
                    else {
                        printf("CMS: ERROR: Unknown sort order '%s'. Use ASC or DESC.\n", order);
                        addHistory("SHOW: Failed - invalid sort order");
                        return 1;
                    }
                }

                sort_and_print(records, *count, by_id, asc);
                addHistory("SHOW ALL SORT: Displayed sorted records");
                return 1;
            }

            printf("CMS: ERROR: Invalid SHOW command.\n");
            addHistory("SHOW: Failed - invalid SHOW command");
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
            if (n > MAX_HISTORY) n = MAX_HISTORY;
            showHistory(n);
            return 1;
        }

        // EXIT / QUIT
        if (iequals(command, "EXIT") || iequals(command, "QUIT")) {
            printf("DEBUG: Checking EXIT/QUIT. Command is: '%s'\n", command);
            printf("CMS: Program exiting.\n");
            char msg[HISTORY_DESC_LEN]; snprintf(msg, sizeof(msg), "EXIT: Program exited");
            addHistory(msg);
            saveHistoryToFile();
            return 0;
        }

        // Unknown command
        printf("CMS: ERROR: Unknown command '%s'.\n", command);
        char msg[HISTORY_DESC_LEN]; 
        snprintf(msg, sizeof(msg), "UNKNOWN: %s", command); 
        addHistory(msg);
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
        printDeclaration();
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