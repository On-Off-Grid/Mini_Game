#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

#define MAX_NAME 50
#define MAX_PLAYERS 10
#define MAX_MOVES 1000
#define MAX_OP_LEN 4
#define MAX_MOVES_PER_ROUND 3

// Structure to hold both stacks
typedef struct {
    int *a;
    int *b;
    int size_a;
    int size_b;
    int max_size;
} Stacks;

// Structure to hold player information
typedef struct {
    char name[MAX_NAME];
    Stacks stacks;
    int moves;
    int has_won;
    char move_history[MAX_MOVES][MAX_OP_LEN];
    double time_remaining;
    char status[20];
    int color_code;
    int initial_stack[MAX_MOVES];
    int initial_stack_size;
} Player;

double get_current_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

// Add this function to calculate the rank of a number in the initial array
int get_number_rank(int number, int *initial_array, int size) {
    int rank = 0;
    for (int i = 0; i < size; i++) {
        if (initial_array[i] < number) {
            rank++;
        }
    }
    return rank;
}

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Function to assign random vibrant color to player
void assign_player_color(Player *player) {
    static const int colors[] = {31, 32, 33, 34, 35, 36};  // Red, Green, Yellow, Blue, Magenta, Cyan
    static int used_colors[6] = {0};
    int available_colors = 0;
    
    // Count available colors
    for (int i = 0; i < 6; i++) {
        if (!used_colors[i]) available_colors++;
    }
    
    // If all colors used, reset
    if (available_colors == 0) {
        memset(used_colors, 0, sizeof(used_colors));
    }
    
    // Assign random available color
    int color_index;
    do {
        color_index = rand() % 6;
    } while (used_colors[color_index]);
    
    used_colors[color_index] = 1;
    player->color_code = colors[color_index];
}
// Standard stack display without ranks
void print_standard_stacks(Player *player) {
    printf("\n\033[1;%dm%s's stacks:\033[0m\n", player->color_code, player->name);
    printf("Time remaining: %.1f seconds\n", player->time_remaining);
    
    int max_height = (player->stacks.size_a > player->stacks.size_b) ? 
                     player->stacks.size_a : player->stacks.size_b;
    
    printf("\nStack A  Stack B\n");
    printf("-------  -------\n");
    
    for (int i = 0; i < max_height; i++) {
        if (i < player->stacks.size_a)
            printf("%-7d ", player->stacks.a[i]);
        else
            printf("        ");
        
        if (i < player->stacks.size_b)
            printf("%-7d", player->stacks.b[i]);
        
        printf("\n");
    }
    printf("-------  -------\n\n");
}

// Print stacks with ranks for radix mode
void print_radix_stacks(Player *player) {
    printf("\n\033[1;%dm%s's stacks (Radix Mode):\033[0m\n", player->color_code, player->name);
    printf("Time remaining: %.1f seconds\n", player->time_remaining);

    int max_height = (player->stacks.size_a > player->stacks.size_b) ?
                     player->stacks.size_a : player->stacks.size_b;

    printf("\nStack A  Rank    Stack B  Rank\n");
    printf("-------  ----    -------  ----\n");

    for (int i = 0; i < max_height; i++) {
        if (i < player->stacks.size_a) {
            int rank_a = get_number_rank(player->stacks.a[i], player->initial_stack, player->initial_stack_size);
            printf("%-7d  %-4d   ", player->stacks.a[i], rank_a);
        } else {
            printf("                 ");
        }

        if (i < player->stacks.size_b) {
            int rank_b = get_number_rank(player->stacks.b[i], player->initial_stack, player->initial_stack_size);
            printf("%-7d  %-4d", player->stacks.b[i], rank_b);
        }

        printf("\n");
    }
    printf("-------  ----    -------  ----\n\n");
}

// Modified generate_random_number to include negative numbers
int generate_random_number(int min, int max) {
    int range = max - min + 1;
    int num = min + rand() % range;
    // 50% chance of being negative
    return (rand() % 2) ? num : -num;
}

// Check if number already exists in array
int number_exists(int *arr, int size, int num) {
    for (int i = 0; i < size; i++) {
        if (arr[i] == num) return 1;
    }
    return 0;
}

// Generate the common set of random numbers
int* generate_common_numbers(int size) {
    int* numbers = (int*)malloc(size * sizeof(int));
    
    for (int i = 0; i < size; i++) {
        int num;
        do {
            num = generate_random_number(1, size * 100);
        } while (number_exists(numbers, i, num));
        numbers[i] = num;
    }
    
    return numbers;
}

// Initialize player's stacks with common numbers
void initialize_player_stacks(Player *player, int size, int *common_numbers) {
    player->stacks.max_size = size;
    player->stacks.size_a = size;
    player->stacks.size_b = 0;
    player->moves = 0;
    player->has_won = 0;
    
    player->stacks.a = (int*)malloc(size * sizeof(int));
    player->stacks.b = (int*)malloc(size * sizeof(int));
    
    player->initial_stack_size = size;
    for (int i = 0; i < size; i++) {
        player->stacks.a[i] = common_numbers[i];
	player->initial_stack[i] = common_numbers[i];
    }
}

// New function to display initial stack
void display_initial_stack(Player *players, int num_players) {
    printf("\nInitial Stack A: |");
    for (int i = 0; i < players[0].initial_stack_size; i++) {
        printf("%d|", players[0].initial_stack[i]);
    }
    printf("\n");
}

// Shuffle array using Fisher-Yates algorithm
void shuffle_players(Player *players, int num_players) {
    for (int i = num_players - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Player temp = players[i];
        players[i] = players[j];
        players[j] = temp;
    }
}

// Compare function for qsort
int compare_players(const void *a, const void *b) {
    const Player *p1 = (const Player *)a;
    const Player *p2 = (const Player *)b;
    
    // First compare by success/failure
    int status_comp = strcmp(p2->status, p1->status);  // Reverse order to put "SUCCESS" first
    if (status_comp != 0) return status_comp;
    
    // If both succeeded or both failed, compare by moves
    if (p1->moves != p2->moves)
        return p1->moves - p2->moves;
    
    // If moves are equal, compare by remaining time
    if (p1->time_remaining > p2->time_remaining) return -1;
    if (p1->time_remaining < p2->time_remaining) return 1;
    return 0;
}

// Stack operations
void swap_a(Player *p) {
    if (p->stacks.size_a >= 2) {
        swap(&p->stacks.a[0], &p->stacks.a[1]);
    }
}

void swap_b(Player *p) {
    if (p->stacks.size_b >= 2) {
        swap(&p->stacks.b[0], &p->stacks.b[1]);
    }
}

void swap_a_b(Player *p) {
    swap_a(p);
    swap_b(p);
}

void push_a(Player *p) {
    if (p->stacks.size_b > 0) {
        for (int i = p->stacks.size_a; i > 0; i--) {
            p->stacks.a[i] = p->stacks.a[i-1];
        }
        p->stacks.a[0] = p->stacks.b[0];
        p->stacks.size_a++;
        
        for (int i = 0; i < p->stacks.size_b - 1; i++) {
            p->stacks.b[i] = p->stacks.b[i+1];
        }
        p->stacks.size_b--;
    }
}

void push_b(Player *p) {
    if (p->stacks.size_a > 0) {
        for (int i = p->stacks.size_b; i > 0; i--) {
            p->stacks.b[i] = p->stacks.b[i-1];
        }
        p->stacks.b[0] = p->stacks.a[0];
        p->stacks.size_b++;
        
        for (int i = 0; i < p->stacks.size_a - 1; i++) {
            p->stacks.a[i] = p->stacks.a[i+1];
        }
        p->stacks.size_a--;
    }
}

void rotate_a(Player *p) {
    if (p->stacks.size_a >= 2) {
        int temp = p->stacks.a[0];
        for (int i = 0; i < p->stacks.size_a - 1; i++) {
            p->stacks.a[i] = p->stacks.a[i + 1];
        }
        p->stacks.a[p->stacks.size_a - 1] = temp;
    }
}

void rotate_b(Player *p) {
    if (p->stacks.size_b >= 2) {
        int temp = p->stacks.b[0];
        for (int i = 0; i < p->stacks.size_b - 1; i++) {
            p->stacks.b[i] = p->stacks.b[i + 1];
        }
        p->stacks.b[p->stacks.size_b - 1] = temp;
    }
}

void rotate_a_b(Player *p) {
    rotate_a(p);
    rotate_b(p);
}

void reverse_rotate_a(Player *p) {
    if (p->stacks.size_a >= 2) {
        int temp = p->stacks.a[p->stacks.size_a - 1];
        for (int i = p->stacks.size_a - 1; i > 0; i--) {
            p->stacks.a[i] = p->stacks.a[i - 1];
        }
        p->stacks.a[0] = temp;
    }
}

void reverse_rotate_b(Player *p) {
    if (p->stacks.size_b >= 2) {
        int temp = p->stacks.b[p->stacks.size_b - 1];
        for (int i = p->stacks.size_b - 1; i > 0; i--) {
            p->stacks.b[i] = p->stacks.b[i - 1];
        }
        p->stacks.b[0] = temp;
    }
}

void reverse_rotate_a_b(Player *p) {
    reverse_rotate_a(p);
    reverse_rotate_b(p);
}

int is_player_won(Player *p) {
    if (p->stacks.size_b != 0) return 0;
    
    for (int i = 0; i < p->stacks.size_a - 1; i++) {
        if (p->stacks.a[i] > p->stacks.a[i + 1]) return 0;
    }
    return 1;
}

void process_move(Player *p, char *op) {
    if (p->moves < MAX_MOVES) {
        strncpy(p->move_history[p->moves], op, MAX_OP_LEN - 1);
        p->move_history[p->moves][MAX_OP_LEN - 1] = '\0';
    }

    if (!strcmp(op, "sa")) {
        swap_a(p);
    } else if (!strcmp(op, "sb")) {
        swap_b(p);
    } else if (!strcmp(op, "ss")) {
        swap_a_b(p);
    } else if (!strcmp(op, "pa")) {
        push_a(p);
    } else if (!strcmp(op, "pb")) {
        push_b(p);
    } else if (!strcmp(op, "ra")) {
        rotate_a(p);
    } else if (!strcmp(op, "rb")) {
        rotate_b(p);
    } else if (!strcmp(op, "rr")) {
        rotate_a_b(p);
    } else if (!strcmp(op, "rra")) {
        reverse_rotate_a(p);
    } else if (!strcmp(op, "rrb")) {
        reverse_rotate_b(p);
    } else if (!strcmp(op, "rrr")) {
        reverse_rotate_a_b(p);
    } else {
        printf("Unknown operation.\n");
        return;
    }
    p->moves++;
}

// Modified display_moves_history to only show current player's moves
void display_moves_history(Player *player) {
    printf("\n=== Move History for \033[1;%dm%s\033[0m ===\n", 
           player->color_code, player->name);
    printf("Total moves so far: %d\n", player->moves);
    for (int i = 0; i < player->moves; i++) {
        printf("%s ", player->move_history[i]);
        if ((i + 1) % 10 == 0) printf("\n");
    }
    printf("\n\n");
}

void display_operations() {
    printf("\nAvailable Operations | Opérations Disponibles:\n");
    printf("--------------------------------------------------------\n");
    printf("sa  - swap first two elements of stack A | échanger les deux premiers éléments de la pile A\n");
    printf("sb  - swap first two elements of stack B | échanger les deux premiers éléments de la pile B\n");
    printf("ss  - sa and sb at the same time | sa et sb en même temps\n");
    printf("pa  - push top element from B to A | pousser le premier élément de B vers A\n");
    printf("pb  - push top element from A to B | pousser le premier élément de A vers B\n");
    printf("ra  - rotate stack A upwards | rotation vers le haut de la pile A\n");
    printf("rb  - rotate stack B upwards | rotation vers le haut de la pile B\n");
    printf("rr  - ra and rb at the same time | ra et rb en même temps\n");
    printf("rra - reverse rotate stack A downwards | rotation vers le bas de la pile A\n");
    printf("rrb - reverse rotate stack B downwards | rotation vers le bas de la pile B\n");
    printf("rrr - rra and rrb at the same time | rra et rrb en même temps\n");
    printf("--------------------------------------------------------\n\n");
}

int main()
{   
    srand(time(NULL));
    
    int num_players;
    printf("Enter number of players (max %d): ", MAX_PLAYERS);
    scanf("%d", &num_players);
    if (num_players < 1 || num_players > MAX_PLAYERS) {
        printf("Invalid number of players!\n");
        return 1;
    }
    int game_mode;
    printf("\nSelect game mode:\n");
    printf("1. Classic Mode (1 move per round)\n");
    printf("2. Dynamic Move Mode (moves based on array size)\n");
    printf("3. Radix Mode (with number rankings)\n");
    printf("Choice: ");
    scanf("%d", &game_mode);
     // Get moves per turn for Custom Move Mode
    int moves_per_turn = 1;
    if (game_mode == 2 || game_mode == 3) {
        printf("Enter number of moves per turn: ");
        scanf("%d", &moves_per_turn);
        if (moves_per_turn < 1) {
            printf("Invalid number of moves!\n");
            return 1;
        }
    }

    double time_limit;
    printf("Enter time limit per player per round (in minutes): ");
    scanf("%lf", &time_limit);
    time_limit *= 60;  // Convert to seconds
    
    int array_size;
    printf("Enter size of arrays: ");
    scanf("%d", &array_size);
    // Calculate dynamic moves for mode 2
    int max_moves_dynamic;
    if (array_size % 2 == 0) {
        max_moves_dynamic = array_size / 2;
    } else {
        max_moves_dynamic = array_size / 3;
    }

    int *common_numbers = generate_common_numbers(array_size);
    printf("\nInitial numbers for all players: |");
    for (int i = 0; i < array_size; i++) {
        printf("%d|", common_numbers[i]);
    }
    printf("\n");
    
    Player *players = (Player*)malloc(num_players * sizeof(Player));
    for (int i = 0; i < num_players; i++) {
        printf("Enter name for Player %d: ", i + 1);
        scanf("%s", players[i].name);
        initialize_player_stacks(&players[i], array_size, common_numbers);
        players[i].moves = 0;
        players[i].time_remaining = time_limit;

	strcpy(players[i].status, "PENDING");  // Initialize status
        assign_player_color(&players[i]);      // Assign random color
    }
    
    free(common_numbers);
    
    shuffle_players(players, num_players);
    
    printf("\nRandomized player order:\n");
    for (int i = 0; i < num_players; i++) {
        printf("%d. \033[1;%dm%s\033[0m\n", i + 1, players[i].color_code, players[i].name);
    }

    if (game_mode != 1 && game_mode != 2 && game_mode != 3) {
        printf("Invalid game mode!\n");
        return 1;
    }
    int game_over = 0;
    int players_remaining = num_players;
    int failed_players = 0;
    int first_win_moves = MAX_MOVES; // Track moves of first winner
    int round = 1;

     while (!game_over && players_remaining > 0) {
        for (int i = 0; i < num_players && !game_over; i++) {
            if (!players[i].has_won && players[i].time_remaining > 0) {
                display_operations();
                display_moves_history(&players[i]);// Only show current player's moves
                // Use appropriate display function based on game mode
                if (game_mode == 3) {
                    print_radix_stacks(&players[i]);
                } else {
                    print_standard_stacks(&players[i]);
                }

                int moves_this_round = 0;
                int max_moves = (game_mode == 1) ? 1 : moves_per_turn;
                
                while (moves_this_round < max_moves) {
                    double start_time = get_current_time();
                    
                    char op[4];
                    printf("\033[1;%dm%s\033[0m's turn - Move %d/%d (stop to quit, next for next player): ", 
                           players[i].color_code, players[i].name, 
                           moves_this_round + 1, max_moves);
                    scanf("%3s", op);
                    
                    double time_taken = get_current_time() - start_time;
                    players[i].time_remaining -= time_taken;

                    if (strcmp(op, "stop") == 0) {
                        printf("\n\033[1;%dm%s\033[0m has quit the game.\n", 
                               players[i].color_code, players[i].name);
                        strcpy(players[i].status, "QUIT");
                        players[i].has_won = 1;
                        players_remaining--;
                        failed_players++;
                        break;
                    }

                    if (strcmp(op, "next") == 0 && game_mode == 2) {
                        break;
                    }

                    if (players[i].time_remaining <= 0) {
                        printf("\n\033[1;31m%s ran out of time!\033[0m\n", players[i].name);
                        strcpy(players[i].status, "TIMEOUT");
                        players[i].has_won = 1;
                        players_remaining--;
                        failed_players++;
                        break;
                    }

                    process_move(&players[i], op);
                    if (game_mode == 3) {
                        print_radix_stacks(&players[i]);
                    } else {
                        print_standard_stacks(&players[i]);
                    }
                    moves_this_round++;

                    if (is_player_won(&players[i])) {
                        printf("\n\033[1;32mCongratulations! %s has won with %d moves!\033[0m\n", 
                               players[i].name, players[i].moves);
                        strcpy(players[i].status, "SUCCESS");
                        players[i].has_won = 1;
                        players_remaining--;
			
			if (first_win_moves == MAX_MOVES) {
                            first_win_moves = players[i].moves;
                            printf("\nOther players have a chance to complete their moves and try to win with %d moves or fewer!\n",
                                   first_win_moves);
                        }
			break;
                                }
                            }
                        }
                    }
	// Check if all players have either won or failed
        int all_players_done = 1;
        for (int i = 0; i < num_players; i++) {
            if (!players[i].has_won && players[i].time_remaining > 0) {
                all_players_done = 0;
                break;
            }
        }

        if (all_players_done) {
            game_over = 1;
        }
    }
    // Modified ranking display to show time consumed
    qsort(players, num_players, sizeof(Player), compare_players);
    display_initial_stack(players, num_players);
    printf("\n=== \033[1;36mFinal Rankings\033[0m ===\n");
    for (int i = 0; i < num_players; i++) {
        printf("\n%d. \033[1;%dm%s\033[0m:\n", i + 1, players[i].color_code, players[i].name);
        printf("Status: \033[1;%dm%s\033[0m\n", 
               strcmp(players[i].status, "SUCCESS") == 0 ? 32 : 31, 
               players[i].status);
        printf("Total moves: %d\n", players[i].moves);
        printf("Time consumed: %.1f seconds\n", time_limit - players[i].time_remaining);
        printf("Move sequence: ");
        for (int j = 0; j < players[i].moves; j++) {
            printf("%s ", players[i].move_history[j]);
        }
        printf("\n");
    }
    
    for (int i = 0; i < num_players; i++) {
        free(players[i].stacks.a);
        free(players[i].stacks.b);
    }
    free(players);
    
    return 0;
}

