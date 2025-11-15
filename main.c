#include <stdio.h>
#include <string.h>
#include <ctype.h> 

#include "database.h"
#include "records.h"
#include "sort.h"
#include "summary.h"


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
        if (rc == 1) printf("CMS: OPEN successful (%s). %d records.\n", file, *count);
        else printf("CMS: ERROR: OPEN failed for '%s'.\n", file);
        return 1;
    }

    // SAVE 
    if (iequals(command, "SAVE")) {
        // Ignore any filename supplied by user; always use default_filename
        const char *file = default_filename && *default_filename ? default_filename : "P5_4-CMS.txt";
        int rc = saveDB(file, records, *count);
        if (rc == 1) printf("CMS: SAVE successful (%s).\n", file);
        else printf("CMS: ERROR: SAVE failed for '%s'.\n", file);
        return 1;
    }
/*
    // INSERT ID Name Programme Mark
    // (Name and Programme must not contain spaces)
    if (iequals(command, "INSERT")) {
        int id;
        char name[STRING_LEN] = {0};
        char programme[STRING_LEN] = {0};
        float mark;
        if (sscanf(local_args, "%d %49s %49s %f", &id, name, programme, &mark) == 4) {
            if (*count >= MAX_RECORDS) {
                printf("CMS: ERROR: Database full.\n");
                return 1;
            }
            StudentRecord sr = { .id = id, .mark = mark };
            strncpy(sr.name, name, STRING_LEN-1);
            strncpy(sr.programme, programme, STRING_LEN-1);
            #ifdef HAVE_INSERT_RECORD
            if (!insertRecord(records, count, sr)) {
                printf("CMS: ERROR: INSERT failed.\n");
            } else {
                printf("CMS: INSERT successful (ID %d).\n", id);
            }
            #else
            records[*count] = sr;
            (*count)++;
            printf("CMS: INSERT successful (ID %d).\n", id);
            #endif
        } else {
            printf("CMS: ERROR: Invalid INSERT. Use: INSERT <ID> <Name> <Programme> <Mark>\n");
        }
        return 1;
    }

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
                if (f == 'N') {
                    strncpy(records[i].name, value, STRING_LEN-1);
                } else if (f == 'P') {
                    strncpy(records[i].programme, value, STRING_LEN-1);
                } else if (f == 'M') {
                    float m;
                    if (sscanf(value, "%f", &m) == 1) records[i].mark = m;
                    else { printf("CMS: ERROR: Invalid mark.\n"); return 1; }
                } else {
                    printf("CMS: ERROR: Unknown field '%c'. Use N, P, or M.\n", f);
                    return 1;
                }
                printf("CMS: UPDATE successful (ID %d).\n", id);
                break;
            }
        }
        if (!found) printf("CMS: ERROR: Record with ID %d not found.\n", id);
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
*/
       // SHOW [ALL] | [ALL SORT BY ID] | [ALL SORT BY MARK] | [SUMMARY]
    if (iequals(command, "SHOW")) {
        // Build uppercase view of local_args for robust matching
        char tmp[128] = {0};
        for (int i = 0; i < (int)sizeof(tmp)-1 && local_args[i]; ++i) tmp[i] = (char)toupper((unsigned char)local_args[i]);
        trim(tmp);
        if (tmp[0] == '\0' || strcmp(tmp, "ALL") == 0) {
            // Use centralized display function to print all records (keeps formatting consistent)
            showAllRecords(records, *count);
            return 1;
        }

        // Tokenize tmp into words for pattern matching: e.g., ALL SORT BY ID DESC
        char toks[8][32];
        int ntoks = 0;
        {
            char work[128];
            strncpy(work, tmp, sizeof(work)-1);
            char *p = strtok(work, " \t");
            while (p && ntoks < 8) {
                strncpy(toks[ntoks++], p, sizeof(toks[0])-1);
                p = strtok(NULL, " \t");
            }
        }
        if (ntoks >= 1 && iequals(toks[0], "SUMMARY")) {
            showSummary(records, *count);
            return 1;
        }

        // Check for pattern: ALL SORT BY <ID|MARK> [ASC|DESC]
        if (ntoks >= 4 && iequals(toks[0], "ALL") && iequals(toks[1], "SORT") && iequals(toks[2], "BY")) {
            int by_id = 0;
            if (iequals(toks[3], "ID")) by_id = 1;
            else if (iequals(toks[3], "MARK")) by_id = 0;
            else {
                printf("CMS: ERROR: Invalid SHOW SORT field '%s'. Use ID or MARK.\n", toks[3]);
                return 1;
            }
            int asc = 1; // default ascending
            if (ntoks >= 5) {
                if (iequals(toks[4], "DESC")) asc = 0;
                else if (iequals(toks[4], "ASC")) asc = 1;
                else {
                    printf("CMS: ERROR: Unknown sort order '%s'. Use ASC or DESC.\n", toks[4]);
                    return 1;
                }
            }
            // Sort a copy and print (preserves DB order)
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
    StudentRecord records[MAX_RECORDS];    // in-memory DB
    int record_count = 0;                 // current number of records
    const char *filename = "P5_4-CMS.txt"; // default DB filename

    enum { MAX_CMD_LEN = 512, CMD_WORD_LEN = 32, ARGS_LEN = 480 };
    char input_buffer[MAX_CMD_LEN];
    char command[CMD_WORD_LEN];
    char arguments[ARGS_LEN];

    int running = 1;

    if (loadDB(filename, records, &record_count) == 1) {
        printf("CMS: Loaded %d record(s) from %s\n", record_count, filename);
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

    return 0;
}