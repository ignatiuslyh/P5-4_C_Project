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


// UTILITY FUNCTION: displayPrompt
void displayPrompt() {
    printf("P5_4: ");
}


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

static void extract_input(const char *src, size_t slen,
                              int start_idx,
                              int idx_id, int idx_name, int idx_prog, int idx_mark,
                              int key_len, int out_size, char *out_buf)
{
    // compute start (after key=) in original src to preserve spacing/case
    int start = start_idx + key_len;
    // compute end as the nearest key position after start, or slen
    int end = (int)slen;
    if (idx_id  > start_idx && idx_id  < end) end = idx_id;
    if (idx_name> start_idx && idx_name< end) end = idx_name;
    if (idx_prog> start_idx && idx_prog< end) end = idx_prog;
    if (idx_mark> start_idx && idx_mark< end) end = idx_mark;
    // trim leading spaces
    while (start < end && isspace((unsigned char)src[start])) start++;
    // trim trailing spaces
    while (end > start && isspace((unsigned char)src[end - 1])) end--;
    // copy bounded
    int vlen = end - start;
    if (vlen >= out_size) vlen = out_size - 1;
    if (vlen > 0) memcpy(out_buf, src + start, (size_t)vlen);
    out_buf[vlen] = '\0';
}

static int isValidNames(const char *name) {
    if (!name) return 0;
    for (int i = 0; name[i] != '\0'; ++i) {
        unsigned char c = (unsigned char)name[i];
        // Allow letters, space, hyphen, apostrophe, and period
        if (!isalpha(c) && c != ' ' && c != '-' && c != '\'' && c != '.' && c != '('&& c != ')') {
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
            addHistory("OPEN: Opened database file");
        }
        else printf("CMS: ERROR: The database file \"%s\" is failed to open.\n", file);
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
        // copy args to local buffer (preserve case and spacing)
        char src[256] = {0};
        strncpy(src, local_args, sizeof(src) - 1);
        size_t slen = strlen(src);

        // find key positions in the original-cased src (case-sensitive)
        char *p_id   = strstr(src, "ID=");
        char *p_name = strstr(src, "Name=");
        char *p_prog = strstr(src, "Programme=");
        char *p_mark = strstr(src, "Mark=");

        // require exact case keys be present; otherwise error (simple behavior)
        if (!p_id || !p_name || !p_prog || !p_mark) {
            printf("CMS: ERROR: Invalid INSERT. Keys must be exactly: ID= Name= Programme= Mark=\n");
            return 1;
        }

        // convert pointers to integer indices
        int idx_id   = (int)(p_id - src);
        int idx_name = (int)(p_name - src);
        int idx_prog = (int)(p_prog - src);
        int idx_mark = (int)(p_mark - src);

        // buffers for extracted values
        char idstr[32] = {0};
        char namestr[STRING_LEN] = {0};
        char progstr[STRING_LEN] = {0};
        char markstr[32] = {0};

        // extract each value using the existing helper (preserve spaces)
        extract_input(src, slen, idx_id,   idx_id, idx_name, idx_prog, idx_mark, (int)strlen("ID="),        sizeof(idstr),   idstr);
        extract_input(src, slen, idx_name, idx_id, idx_name, idx_prog, idx_mark, (int)strlen("Name="),      sizeof(namestr), namestr);
        extract_input(src, slen, idx_prog, idx_id, idx_name, idx_prog, idx_mark, (int)strlen("Programme="), sizeof(progstr), progstr);
        extract_input(src, slen, idx_mark, idx_id, idx_name, idx_prog, idx_mark, (int)strlen("Mark="),      sizeof(markstr), markstr);

        // validate required fields are not empty
        if (idstr[0] == '\0' || namestr[0] == '\0' || progstr[0] == '\0' || markstr[0] == '\0') {
            printf("CMS: ERROR: Invalid INSERT. Use: INSERT ID=<id> Name=<name> Programme=<programme> Mark=<mark>\n");
            return 1;
        }

        // validate characters in Name and Programme
        if (!isValidNames(namestr)) {
            printf("CMS: ERROR: Invalid characters in Name. Allowed: letters, space, -, ', .\n");
            return 1;
        }
        if (!isValidNames(progstr)) {
            printf("CMS: ERROR: Invalid characters in Programme. Allowed: letters, space, -, ', .\n");
            return 1;
        }

        // parse numeric values safely
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
        // round up to 1 decimal point
         mark = round(mark * 10) / 10.0;
        // enforce mark range [0.0, 100.0]
        if (mark < 0.0f || mark > 100.0f) {
            printf("CMS: ERROR: Mark must be between 0.0 and 100.0.\n");
            return 1;
        }
         
               
        // build record and insert
        StudentRecord sr;
        sr.id = id;
        strncpy(sr.name, namestr, STRING_LEN - 1); sr.name[STRING_LEN - 1] = '\0';
        strncpy(sr.programme, progstr, STRING_LEN - 1); sr.programme[STRING_LEN - 1] = '\0';
        sr.mark = mark;

        if (!insertRecord(records, count, &sr)) {
            // insertRecord prints an error (duplicate or full)
        } else {
            printf("CMS: INSERT successful (ID %d).\n", id);
        }
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
            // validate marks field to not contain letter and mark goes from between 0 to 100 also update to 1d.p
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