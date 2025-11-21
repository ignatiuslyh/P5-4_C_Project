#include "history.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

static HistoryEntry history[MAX_HISTORY];
static int history_count = 0;

// Load existing history from file into memory
void initHistory(void) {
    FILE *fp = fopen(HISTORY_FILE, "r");
    if (!fp) return; // no file yet

    history_count = 0;
    char line[256];
    while (fgets(line, sizeof(line), fp) && history_count < MAX_HISTORY) {
        // Expect line format: <timestamp>\t<description>\n
        char *tab = strchr(line, '\t');
        if (!tab) continue;

        *tab = '\0';
        time_t t = (time_t)atoll(line);
        strncpy(history[history_count].description, tab + 1, HISTORY_DESC_LEN - 1);
        history[history_count].description[HISTORY_DESC_LEN - 1] = '\0';

        // remove trailing newline
        size_t len = strlen(history[history_count].description);
        if (len > 0 && history[history_count].description[len - 1] == '\n')
            history[history_count].description[len - 1] = '\0';

        history[history_count].timestamp = t;
        history_count++;
    }

    fclose(fp);
}

// Helper: write current history array back to file
 void saveHistoryToFile(void) {
    FILE *fp = fopen(HISTORY_FILE, "w");
    if (!fp) return;

    for (int i = 0; i < history_count; i++) {
        fprintf(fp, "%lld\t%s\n", (long long)history[i].timestamp, history[i].description);
    }

    fclose(fp);
}

// Add a new history entry
void addHistory(const char* description) {
    if (!description) return;

    // Shift older entries if already at max
    if (history_count == MAX_HISTORY) {
        for (int i = 1; i < MAX_HISTORY; i++) {
            history[i - 1] = history[i];
        }
        history_count--;
    }

    // Add new entry at the end
    history[history_count].timestamp = time(NULL);
    strncpy(history[history_count].description, description, HISTORY_DESC_LEN - 1);
    history[history_count].description[HISTORY_DESC_LEN - 1] = '\0';
    history_count++;

    saveHistoryToFile();
}

// Show last N entries (default 5)
void showHistory(int n) {
    if (n <= 0) n = 5;
    if (n > history_count) n = history_count;

    printf("CMS: Last %d history entries:\n", n);
    for (int i = history_count - n; i < history_count; i++) {
        struct tm *tm_info = localtime(&history[i].timestamp);
        char timestr[32];
        strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M:%S", tm_info);
        printf("%s - %s\n", timestr, history[i].description);
    }
}
