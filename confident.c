#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <ncurses.h>

#define MAX_INPUT 100
#define LOG_FILE "confidence_log.txt"
#define RESULT_FILE "confidence_result.txt"

typedef enum {
    WAVE3 = 25,
    WAVE5 = 20,
    WAVEC = 15,
    WAVE1 = 10,
    CORRECTIVE = 5,
    INVALID_WAVE = -1
} WaveScore;

typedef struct {
    double score;
    char strategy[MAX_INPUT];
} ConfidenceResult;

void log_message(const char* message) {
    FILE* log_file = fopen(LOG_FILE, "a");
    if (log_file) {
        fprintf(log_file, "%s\n", message);
        fclose(log_file);
    }
}

void to_lowercase(char* str) {
    for (int i = 0; str[i]; i++) str[i] = tolower(str[i]);
}

void to_uppercase(char* str) {
    for (int i = 0; str[i]; i++) str[i] = toupper(str[i]);
}

void trim_whitespace(char* str) {
    char* end;
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return;
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    *(end + 1) = '\0';
}

void get_input(char* buffer, const char* prompt, int max_len) {
    clear();
    mvprintw(LINES / 2 - 1, (COLS - strlen(prompt)) / 2, "%s", prompt);
    echo();
    mvgetnstr(LINES / 2, (COLS - max_len) / 2, buffer, max_len);
    noecho();
    buffer[strcspn(buffer, "\n")] = 0;
    trim_whitespace(buffer);
    to_lowercase(buffer);
}

bool get_bool(const char* prompt) {
    char input[MAX_INPUT];
    while (1) {
        get_input(input, prompt, MAX_INPUT);
        if (strcmp(input, "true") == 0) return true;
        if (strcmp(input, "false") == 0) return false;
        mvprintw(LINES / 2 + 2, (COLS - 30) / 2, "Please enter 'true' or 'false'.");
        refresh();
        getch();
    }
}

WaveScore get_wave_score(const char* position) {
    if (strcmp(position, "wave3") == 0) return WAVE3;
    if (strcmp(position, "wave5") == 0) return WAVE5;
    if (strcmp(position, "wavec") == 0) return WAVEC;
    if (strcmp(position, "wave1") == 0) return WAVE1;
    if (strcmp(position, "corrective") == 0) return CORRECTIVE;
    return INVALID_WAVE;
}

bool validate_bb_status(const char* status) {
    return strcmp(status, "overbought") == 0 || strcmp(status, "oversold") == 0 || strcmp(status, "neutral") == 0;
}

ConfidenceResult evaluate_confidence(
    WaveScore wave_score,
    bool trend,
    bool macd,
    const char* bb,
    bool vol,
    bool bb_touch,
    bool vol_at_bb,
    const char* strat
) {
    double score = 0;
    score += wave_score;
    score += trend ? 20 : 5;
    score += macd ? 20 : 0;

    if (strcmp(bb, "overbought") == 0 || strcmp(bb, "oversold") == 0) {
        if (bb_touch) score += 15;
        if (vol_at_bb) score += 10;
    }

    score += vol ? 20 : 5;

    ConfidenceResult result;
    result.score = score;
    strncpy(result.strategy, strat, MAX_INPUT);
    result.strategy[MAX_INPUT - 1] = '\0';
    return result;
}

void save_result(const ConfidenceResult* res) {
    FILE* f = fopen(RESULT_FILE, "w");
    if (f) {
        fprintf(f, "Strategy: %s\nConfidence Score: %.2f%%\n", res->strategy, res->score);
        fclose(f);
        log_message("Result saved successfully.");
    } else {
        log_message("Failed to save result.");
    }
}

void run_interface() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    char wave[MAX_INPUT], bb[MAX_INPUT], strat[MAX_INPUT];
    bool trend, macd, vol, bb_touch, vol_at_bb;
    bool run_again = true;

    log_message("Application started.");

    while (run_again) {
        WaveScore wave_score;
        while (1) {
            get_input(wave, "Wave position (wave3/wave5/wavec/wave1/corrective):", MAX_INPUT);
            wave_score = get_wave_score(wave);
            if (wave_score != INVALID_WAVE) break;
            mvprintw(LINES / 2 + 2, (COLS - 40) / 2, "Invalid wave. Try again.");
            refresh();
            getch();
        }

        trend = get_bool("Is it following the trend? (true/false):");
        macd = get_bool("MACD divergence? (true/false):");

        while (1) {
            get_input(bb, "BB status (overbought/oversold/neutral):", MAX_INPUT);
            if (validate_bb_status(bb)) break;
            mvprintw(LINES / 2 + 2, (COLS - 40) / 2, "Invalid BB status. Try again.");
            refresh();
            getch();
        }

        vol = get_bool("Volume supporting the signal? (true/false):");
        bb_touch = get_bool("Price touched neutral after BB? (true/false):");
        vol_at_bb = get_bool("Volume increased at BB touch? (true/false):");

        get_input(strat, "Strategy name:", MAX_INPUT);
        if (strlen(strat) == 0) strcpy(strat, "MACD");
        to_uppercase(strat);

        ConfidenceResult res = evaluate_confidence(wave_score, trend, macd, bb, vol, bb_touch, vol_at_bb, strat);

        while (1) {
            clear();
            mvprintw(LINES / 2 - 1, (COLS - 40) / 2, "Strategy: %s", res.strategy);
            mvprintw(LINES / 2, (COLS - 40) / 2, "Confidence Score: %.2f%%", res.score);
            mvprintw(LINES / 2 + 2, (COLS - 50) / 2, "Press 'r' to run again, 's' to save, or any other key to exit.");
            refresh();
            int ch = getch();
            if (ch == 'r' || ch == 'R') break;
            else if (ch == 's' || ch == 'S') {
                save_result(&res);
                mvprintw(LINES / 2 + 4, (COLS - 30) / 2, "Result saved to confidence_result.txt");
                refresh();
                getch();
            } else {
                run_again = false;
                break;
            }
        }
    }

    log_message("Application exited.");
    endwin();
}

int main() {
    run_interface();
    return 0;
}
