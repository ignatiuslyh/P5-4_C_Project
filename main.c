#include <stdio.h>
#include <string.h>
#include <ctype.h> 
#include <stdlib.h>

#include "database.h"
#include "records.h"
#include "sort.h"
#include "summary.h"
#include "banner.h"


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
    char local_args[256] = {0};
    if (args && *args) {
        strncpy(local_args, args, sizeof(local_args) - 1);
        trim(local_args);
    }

    // OPEN 
    if (iequals(command, "OPEN")) {
        const char *file = default_filename && *default_filename ? default_filename : "P5_4-CMS.txt";
        int rc = loadDB(file, records, count);
        if (rc == 1) printf("CMS: The database file \"%s\" is successfully opened.\n", file);
        else printf("CMS: ERROR: The database file \"%s\" failed to open.\n", file);
        return 1;
    }

    // SAVE 
    if (iequals(command, "SAVE")) {
        // Ignore any filename supplied by user; always use default_filename
        const char *file = default_filename && *default_filename ? default_filename : "P5_4-CMS.txt";
        int rc = saveDB(file, records, *count);
        if (rc == 1) printf("CMS: SAVE successful (%s).\n", file);
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
    // UPDATE ID FIELD VALUE
    // FIELD: N (name), P (programme), M (mark)
    // VALUE may contain spaces for N/P (we take rest of string)
 // UPDATE ID FIELD VALUE
    // FIELD: N (name), P (programme), M (mark)
    // VALUE may contain spaces for N/P (we take rest of string)
    if (iequals(command, "UPDATE")) {
        char buf[256];
        strncpy(buf, local_args, sizeof(buf)-1);
        char *tok = strtok(buf, " \t");
        if (!tok) { printf("CMS: ERROR: Invalid UPDATE.\n"); return 1; }
        int id = atoi(tok);
        char *field = strtok(NULL, " \t");
        char *value = strtok(NULL, "\n"); // rest of line
        if (!field || !value) {
            printf("CMS: ERROR: Invalid UPDATE. Use: UPDATE <ID> <N|P|M> <value>\n");
            return 1;
        }
        char f = toupper((unsigned char)field[0]);
        trim(value);
        #ifdef HAVE_UPDATE_RECORD
        if (!updateRecord(records, *count, id, f, value)) {
            printf("CMS: ERROR: UPDATE failed (record not found or bad value).\n");
        } else {
            printf("CMS: UPDATE successful (ID %d).\n", id);
        }
        #else
        int found = 0;
        for (int i = 0; i < *count; ++i) {
            if (records[i].id == id) {
                found = 1;
                if (iequals(field,"Name")) {
                    if (isValidNames(value) ){
                        strncpy(records[i].name, value, STRING_LEN-1);
                        printf("CMS: UPDATE successful (ID %d).\n", id);
                    }
                    else{
                        printf("CMS: INVALID name Type\n");
                    }
                } else if (iequals(field,"Programme")) {

                    if (isValidNames(value)){
                        strncpy(records[i].programme, value, STRING_LEN-1);
                        printf("CMS: UPDATE successful (ID %d).\n", id);
                    }
                    else{
                        printf("CMS: INVALID Programme \n");
                    }

                   
                } else if (iequals(field,"Mark")) {
                    float m;
                    if (sscanf(value, "%f", &m) == 1) records[i].mark = m;
                    else { printf("CMS: ERROR: Invalid mark.\n"); return 1; }
                } else {
                    printf("CMS: ERROR: Unknown field '%c'. Use Name, Programme, or Mark.\n", f);
                    return 1;
                }
                // printf("CMS: UPDATE successful (ID %d).\n", id);
                break;
            }
        }
        if (!found) 
        printf("CMS: The record with ID=%d does not exist.\n", id);
        #endif
        return 1;
    }

    // DELETE ID
    if (iequals(command, "DELETE")) {
        int id;
        if (sscanf(local_args, "%d", &id) == 1) {
            printf("Are you sure you want to delete ID %d? (Y/N): ", id);
            fflush(stdout);
            char resp[8] = {0};
            if (!fgets(resp, sizeof(resp), stdin)) { printf("\nCMS: ERROR: No response; delete cancelled.\n"); return 1; }
            if (resp[0] == 'Y' || resp[0] == 'y') {
                #ifdef HAVE_DELETE_RECORD
                if (!deleteRecord(records, count, id)) printf("CMS: ERROR: DELETE failed (not found).\n");
                else printf("CMS: DELETE successful (ID %d).\n", id);
                #else
                int found = 0;
                for (int i = 0; i < *count; ++i) {
                    if (records[i].id == id) {
                        found = 1;
                        for (int j = i; j + 1 < *count; ++j) records[j] = records[j+1];
                        (*count)--;
                        printf("CMS: DELETE successful (ID %d).\n", id);
                        break;
                    }
                }
                if (!found) printf("CMS: ERROR: DELETE failed (not found).\n");
                #endif
            } else {
                printf("CMS: DELETE cancelled.\n");
            }
        } else {
            printf("CMS: ERROR: Invalid DELETE. Use: DELETE <ID>\n");
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
            return 1;
        }

        // SHOW SUMMARY
        if (iequals(buf, "SUMMARY")) {
            showSummary(records, *count);
            return 1;
        }

        // parse into up to 5 tokens
        char t1[16] = {0}, t2[16] = {0}, t3[16] = {0}, t4[16] = {0}, t5[16] = {0};
        int n = sscanf(buf, "%15s %15s %15s %15s %15s", t1, t2, t3, t4, t5);

        // expect: ALL SORT BY <FIELD> [ORDER]
        if (n >= 4 && iequals(t1, "ALL") && iequals(t2, "SORT") && iequals(t3, "BY")) {
            char *field = t4;
            char *order = (n >= 5) ? t5 : NULL;

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

    // EXIT / QUIT
    if (iequals(command, "EXIT") || iequals(command, "QUIT")) {
        printf("CMS: Program exiting.\n");
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
    
    printBanner();
    printf("CMS: To start, use OPEN to load \"%s\" \n", filename);

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

    return 0;
}