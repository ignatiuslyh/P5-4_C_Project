#ifndef HISTORY_H
#define HISTORY_H

#include <time.h>

#define MAX_HISTORY 20
#define HISTORY_DESC_LEN 150
#define HISTORY_FILE "history.txt"

// Format of each History structure
typedef struct {
    time_t timestamp;
    char description[HISTORY_DESC_LEN];
} HistoryEntry;

// Initialize the history system (loads existing file)
void initHistory(void);

// Add a new operation to history (appends to file, keeps max 20)
void addHistory(const char* description);

// Display last N operations (default 5 if n <= 0)
void showHistory(int n);

void saveHistoryToFile(void);

#endif
