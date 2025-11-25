# CLASS MANAGEMENT SYSTEM (CMS) — P5_4

Simple command-line Class Management System written in C.  
Provides an in-memory student database with basic CRUD operations, import from CSV, sorting, summary statistics and an operation history.

## Features
- Load/save database to a tab-separated text file
- INSERT, QUERY, UPDATE, DELETE records
- SHOW ALL, SHOW SUMMARY
- SHOW ALL SORT BY ID / MARK (ASC/DESC)
- IMPORT CSV (overwrites matching IDs, appends new ones)
- History of recent actions persisted to `history.txt`
- Simple statistics: average, highest/lowest marks, pass/fail counts

## Requirements
- gcc (C11)
- make
- Standard C library and math library (for `round()`): linker flag `-lm` is already used in the Makefile

Tested with the provided Makefile which compiles with:
-std=c11 -Wall -Wextra -O2 -DNDEBUG

## Build

From the project root run:

```sh
make
```

This will compile the project into the executable `cms_P5-4`.

To clean build artifacts:

```sh
make clean
```

## Run

Start the program:

```sh
./cms_P5-4
```

On start the program prints the declaration and banner and informs you that no database is loaded.

A default database filename is embedded (`P5_4-CMS.txt`). You can OPEN that file with:

```
OPEN
```

or explicitly (the code uses the default filename when saving/loading regardless of user input in the SAVE implementation).

## Command Reference (usage examples)

- OPEN
  - Load the default database file (P5_4-CMS.txt).
  - Example: `OPEN`

- SAVE
  - Save current in-memory records to the default database file.
  - Example: `SAVE`

- INSERT ID=<7-digit-id> Name=<name> Programme=<programme> Mark=<mark>
  - Insert a new student record. ID must be exactly 7 characters (digits).
  - Example:
    ```
    INSERT ID=2301234 Name=Alice Programme="Computer Science" Mark=78.5
    ```
    Note: Name and Programme values are accepted by parsing keys; do not include keys with typos (case-sensitive keys are required).

- QUERY ID=<id>
  - Lookup and display a single record by ID.
  - Example: `QUERY ID=2301234`

- UPDATE ID=<id> Name=<name>
  - Update a single field (only one of Name, Programme, or Mark per UPDATE).
  - Example: `UPDATE ID=2301234 Name=AliceNew`
  - For Mark: `UPDATE ID=2301234 Mark=88.0`

- DELETE ID=<id>
  - Delete a record with confirmation (program prompts for Y/N).
  - Example: `DELETE ID=2301234`

- SHOW ALL
  - Display all records.
  - Example: `SHOW ALL`

- SHOW ALL SORT BY <ID|MARK> [ASC|DESC]
  - Show all records sorted by ID or MARK. Default order is ascending.
  - Examples:
    - `SHOW ALL SORT BY ID`
    - `SHOW ALL SORT BY MARK DESC`

- SHOW SUMMARY
  - Prints summary statistics: total, average, highest/lowest with names, passed/failed counts.
  - Example: `SHOW SUMMARY`

- IMPORT filename.csv
  - Import rows from a CSV file with header `ID,Name,Programme,Mark`.
  - The command expects a filename argument: `IMPORT testcsv.csv`
  - Existing IDs will be overwritten (user is prompted if any duplicates are detected).
  - CSV ID column must be exactly 7 characters and contain only digits; Mark must be a number 0.0–100.0.

- HISTORY [N]
  - Show last N history entries (default 5).
  - Example: `HISTORY 10`

- EXIT / QUIT
  - Exit the program (history is saved to `history.txt`).
  - Example: `EXIT`

## Files of interest (brief)

- main.c — entry point, command parser and dispatcher
- records.c / records.h — in-memory CRUD operations and showAllRecords
- database.c / database.h — loadDB and saveDB (file I/O)
- import.c / import.h — CSV import logic
- sort.c / sort.h — bubble sort and formatted print for sorted output
- summary.c / summary.h — summary/statistics
- history.c / history.h — history persistence and display
- banner.c / banner.h — ASCII banner
- Makefile — build instructions
- P5_4-CMS.txt — sample default database (tab-separated)
- testcsv.csv — sample CSV for import
- history.txt — example history file

## Testing

- The repository includes `testcsv.csv` to exercise the IMPORT command.
- Load the sample database with `OPEN` and run `SHOW ALL` to view contents.
- Insert, update, delete and save to see how the database and `history.txt` are updated.

## Notes & Known Behaviours
- INSERT/IMPORT expect IDs of exactly 7 characters (digits). This is enforced in several places.
- UPDATE allows only one field to be updated per command.
- The program maintains a `history.txt` of operations (timestamps + description); the history is auto-saved when an entry is added or on exit.
- SAVE/OPEN use the default filename defined in main; user-supplied filenames are ignored by the current SAVE/OPEN command implementations.
- During DELETE and IMPORT, interactive confirmation is used and reads from stdin (so these commands do not play nicely in non-interactive batch environments unless stdin is provided).
- The sorting implementation uses a local copy of the records, so the original in-memory order is preserved after a SHOW ALL SORT call (it only prints sorted output).
