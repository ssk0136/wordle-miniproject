#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define WORD_LENGTH 5
#define ATTEMPTS 6
#define MAX_NAME_LEN 20

const char *animals[] = {"tiger", "zebra", "horse", "camel", "panda"};
const char *countries[] = {"india", "china", "spain", "italy", "japan"};
const char *fruits[] = {"apple", "berry", "mango", "melon", "grape"};
const char *colors[] = {"black", "brown", "green", "beige", "white"};
const char *games[] = {"chess", "poker", "rugby", "rummy", "bingo"};

const char *theme_names[] = {"Animals", "Countries", "Fruits", "Colors", "Games"};
const char **theme_words[] = {animals, countries, fruits, colors, games};

void to_lower_str(char *str) {
    for (int i = 0; str[i]; i++) str[i] = tolower(str[i]);
}

int is_valid_guess(const char *guess) {
    if (strlen(guess) != WORD_LENGTH) return 0;
    for (int i = 0; i < WORD_LENGTH; i++) {
        if (!isalpha(guess[i])) return 0;
    }
    return 1;
}

const char *select_word_from_theme(int theme_index) {
    return theme_words[theme_index][rand() % 5];
}

const char *get_word(const char *difficulty, char *theme_info) {
    if (strcmp(difficulty, "easy") == 0) {
        printf("\nChoose a theme:\n");
        for (int i = 0; i < 5; i++) printf("%d. %s\n", i + 1, theme_names[i]);
        int choice;
        printf("Enter choice (1-5): ");
        scanf("%d", &choice);
        getchar();
        if (choice >= 1 && choice <= 5) {
            strcpy(theme_info, theme_names[choice - 1]);
            return select_word_from_theme(choice - 1);
        } else return NULL;
    } else if (strcmp(difficulty, "medium") == 0) {
        int t1 = rand() % 5, t2;
        do { t2 = rand() % 5; } while (t2 == t1);
        sprintf(theme_info, "%s & %s", theme_names[t1], theme_names[t2]);
        printf("\nThemes selected: %s\n", theme_info);
        return rand() % 2 ? select_word_from_theme(t1) : select_word_from_theme(t2);
    } else if (strcmp(difficulty, "hard") == 0) {
        strcpy(theme_info, "All themes mixed {animals, countries, fruits, colors, games}");
        return select_word_from_theme(rand() % 5);
    }
    return NULL;
}

void give_feedback(const char *guess, const char *word) {
    for (int i = 0; i < WORD_LENGTH; i++) {
        if (guess[i] == word[i]) printf("[G] %c ", guess[i]);
        else if (strchr(word, guess[i])) printf("[Y] %c ", guess[i]);
        else printf("[ ] %c ", guess[i]);
    }
    printf("\n");
}

int calculate_score(int attempts_used) {
    return (ATTEMPTS - attempts_used + 1) * 10;
}

void save_score(const char *name, int score, const char *difficulty) {
    char filename[30];
    sprintf(filename, "leaderboard_%s.txt", difficulty);
    FILE *file = fopen(filename, "a");
    if (file) {
        fprintf(file, "%s %d\n", name, score);
        fclose(file);
    }
}

void show_leaderboard(const char *difficulty) {
    char filename[30];
    sprintf(filename, "leaderboard_%s.txt", difficulty);
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("\nNo leaderboard data available for %s.\n", difficulty);
        return;
    }
    char names[100][MAX_NAME_LEN];
    int scores[100];
    int count = 0;
    while (fscanf(file, "%s %d", names[count], &scores[count]) == 2) count++;
    fclose(file);
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (scores[j] > scores[i]) {
                int temp_score = scores[i];
                scores[i] = scores[j];
                scores[j] = temp_score;
                char temp_name[MAX_NAME_LEN];
                strcpy(temp_name, names[i]);
                strcpy(names[i], names[j]);
                strcpy(names[j], temp_name);
            }
        }
    }
    printf("\n--- %s Leaderboard ---\n", difficulty);
    for (int i = 0; i < count; i++) printf("%-15s %d\n", names[i], scores[i]);
}

int main() {
    srand(time(NULL));
    char play_again;
    do {
        char player_name[MAX_NAME_LEN];
        printf("Enter your name: ");
        fgets(player_name, sizeof(player_name), stdin);
        player_name[strcspn(player_name, "\n")] = '\0';
        printf("Welcome to Wordle\n");
        printf("Choose difficulty (easy, medium, hard): ");
        char difficulty[10], theme_info[50] = "";
        scanf("%s", difficulty);
        getchar();
        to_lower_str(difficulty);
        const char *word = get_word(difficulty, theme_info);
        if (!word) {
            printf("Error selecting word. Exiting.\n");
            return 1;
        }
        printf("\nWordle Game Started (5-letter word)\n");
        printf("Hint: The word is from %s.\n", theme_info);
        printf("You have %d attempts.\n", ATTEMPTS);
        int attempts_used = 0;
        for (int attempts = ATTEMPTS; attempts > 0; attempts--) {
            char guess[WORD_LENGTH + 2];
            printf("\nAttempt %d - Enter a 5-letter word: ", ATTEMPTS - attempts + 1);
            fgets(guess, sizeof(guess), stdin);
            guess[strcspn(guess, "\n")] = '\0';
            to_lower_str(guess);
            if (!is_valid_guess(guess)) {
                printf("Invalid input! Enter exactly 5 letters with no numbers or symbols.\n");
                attempts++;
                continue;
            }
            attempts_used++;
            give_feedback(guess, word);
            if (strcmp(guess, word) == 0) {
                int score = calculate_score(attempts_used);
                printf("You guessed the word correctly.\nScore: %d points\n", score);
                save_score(player_name, score, difficulty);
                show_leaderboard(difficulty);
                break;
            }
        }
        printf("\nGame Over. The word was: %s\n", word);
        printf("Play again? (y/n): ");
        scanf(" %c", &play_again);
        getchar();
    } while (play_again == 'y' || play_again == 'Y');
    return 0;
}
