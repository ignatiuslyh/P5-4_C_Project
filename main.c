#include <stdio.h>
#include <string.h>
#include <ctype.h> // Useful for command parsing (e.g., converting to uppercase)

// --- Local Header Files ---
// NOTE: These files will contain the StudentRecord definition, constants,
// and the public function prototypes for the other modules.
#include "database.h"
#include "records.h"
#include "sort.h"
#include "summary.h"

// --- Global Constants (Defined in headers, but repeated here for context) ---
#define MAX_RECORDS 100 // Should match definition in records.h
#define MAX_CMD_LENGTH 100 // Max length of a user command string


// UTILITY FUNCTION: displayPrompt
void displayPrompt() {
    // PSEUDOCODE:
    // Print "P5_4: ".
}


// UTILITY FUNCTION: parseCommand
void parseCommand(char *input, char *command, char *args) {
    // PSEUDOCODE:

    // 1. Find the first space.
    // char *first_space = strchr(input, ' ');

    // 2. Extract the command word.
    // IF a space is found, THEN:
        // Copy characters before the space into 'command' buffer.
        // Copy characters after the space into 'args' buffer.
    // ELSE (only a command word, no arguments, e.g., 'EXIT'), THEN:
        // Copy the whole input into 'command'.
        // Clear the 'args' buffer (set to an empty string).

    // 3. Convert the command word to uppercase (e.g., "insert" -> "INSERT").
    // FOR loop through 'command' string:
        // Convert char to upper (using toupper()).
}


// FUNCTION: processCommand
int processCommand(const char *command, char *args, StudentRecord records[], int *count, const char *filename) {
    // PSEUDOCODE:

    // SWITCH statement based on the 'command' string:

    // CASE "OPEN":
        // Call loadDatabase(filename, records, count).
        // BREAK.

    // CASE "SAVE":
        // Call saveDatabase(filename, records, *count).
        // BREAK.

    // CASE "INSERT":
        // IF successfully parse ID, Name, Programme, Mark from 'args', THEN:
            // Create a temporary StudentRecord (newRecord).
            // Call insertRecord(records, count, newRecord).
        // ELSE:
            // Print error ("CMS: ERROR: Invalid INSERT format.").
        // BREAK.

    // CASE "QUERY":
        // IF successfully parse ID from 'args', THEN:
            // Call queryRecord(records, *count, ID).
        // ELSE:
            // Print error ("CMS: ERROR: Invalid QUERY format.").
        // BREAK.

    // CASE "UPDATE":
        // Complex parsing required to determine ID, field (N/P/M), and new value.
        // IF successfully parse the required values from 'args', THEN:
            // Call updateRecord(records, *count, ID, FieldChar, NewValuePointer).
        // ELSE:
            // Print error ("CMS: ERROR: Invalid UPDATE format.").
        // BREAK.

    // CASE "DELETE":
        // IF successfully parse ID from 'args', THEN:
            // Prompt user for confirmation (Are you sure? Y/N).
            // IF user confirms 'Y', THEN:
                // Call deleteRecord(records, count, ID).
            // ELSE:
                // Print cancellation message.
        // ELSE:
            // Print error ("CMS: ERROR: Invalid DELETE format.").
        // BREAK.

    // CASE "SHOW":
        // IF args is "ALL SORT BY ID", THEN:
            // Call sortById(records, *count).
            // Call showAllRecords(records, *count).
        // ELSE IF args is "ALL SORT BY MARK", THEN:
            // Call sortByMark(records, *count).
            // Call showAllRecords(records, *count).
        // ELSE IF args is "ALL" or empty, THEN:
            // Call showAllRecords(records, *count).
        // ELSE IF args is "SUMMARY", THEN:
            // Call showSummary(records, *count).
        // ELSE:
            // Print error ("CMS: ERROR: Invalid SHOW command.").
        // BREAK.

    // CASE "EXIT":
        // Print exit message ("CMS: Program exiting.").
        // RETURN 0 (Signal to stop the loop).

    // DEFAULT (Command not recognized):
        // Print error message ("CMS: ERROR: Unknown command.").
        // BREAK.

    // RETURN 1 (Signal to continue the loop).
    return 1;
}


// MAIN FUNCTION: main
int main() {
    // PSEUDOCODE:

    // 1. Initialization and Setup.
    // StudentRecord records[MAX_RECORDS]; // The in-memory database array.
    // int record_count = 0;              // Current number of records.
    // char filename[] = "P1_1-CMS.txt";  // The default file name.
    // char input_buffer[MAX_CMD_LENGTH]; // Buffer to store user input.
    // char command[20];                  // Buffer to store the main command word.
    // char arguments[MAX_CMD_LENGTH];    // Buffer to store command arguments.
    // int running = 1;                   // Control flag for the main loop.

    // 2. Initial Data Load.
    // Call loadDatabase(filename, records, &record_count);

    // 3. Main Command Loop.
    // WHILE running is 1:
        // Call displayPrompt().
        // Read user input into input_buffer (using fgets or similar).
        // IF reading fails (EOF), THEN break the loop.
        // Remove trailing newline character from input_buffer if present.

        // Call parseCommand(input_buffer, command, arguments).
        // running = processCommand(command, arguments, records, &record_count, filename);

    // 4. Cleanup (Optional).
    // Print a final message or call saveDatabase one last time if user did not save.

    // RETURN 0 (Standard program success exit code).
    return 0;
}