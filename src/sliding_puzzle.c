#include "sliding_puzzle.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

void menu() {
    printf("\n\n");
    printf(" .-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+. \n");
    printf("| .*********************************. |\n");
    printf("| |     Utilisez les lettres        | |\n");
    printf("| |   A pour resolution automatique | |\n");
    printf("| |      H pour l'aide (help)       | |\n");
    printf("| |   U, D, L et R pour jouer.      | |\n"); 
    printf("* *    Votre deplacement doit       * *\n");
    printf("* *    etre valide et faisable      * *\n");
    printf("| |                                 | |\n"); 
    printf("| |                                 | |\n");
    printf("| |    Taper sur la touche 1        | |\n");
    printf("| |  pour commencer la partie       | |\n");
    printf("| |                                 | |\n");
    printf("| '*********************************' |\n");
    printf("'-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+' \n\n\n");
}

void print_menu() {
    printf("\n=== Sliding Puzzle Game ===\n");
    printf("1. Solve manually\n");
    printf("2. Solve automatically\n");
    printf("3. Exit game\n");
    printf("Choose an option: ");
}

void initialize_puzzle(PuzzleState *puzzle, int shuffle) {
    int value = 1;
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            puzzle->board[i][j] = value++;
    puzzle->board[SIZE-1][SIZE-1] = EMPTY;
    puzzle->empty_row = SIZE - 1;
    puzzle->empty_col = SIZE - 1;

    if (shuffle) {
        shuffle_puzzle(puzzle, 100);
        while (!is_solvable(puzzle))
            shuffle_puzzle(puzzle, 100);
    }
}

void print_puzzle(const PuzzleState *puzzle) {
    printf("\n");
    for (int i = 0; i < SIZE; i++) {
        printf("+---+---+---+\n");
        for (int j = 0; j < SIZE; j++) {
            printf("|");
            if (puzzle->board[i][j] == EMPTY) printf("   ");
            else printf("%2d ", puzzle->board[i][j]);
        }
        printf("|\n");
    }
    printf("+---+---+---+\n");
}

int is_solved(const PuzzleState *puzzle) {
    int value = 1;
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++) {
            if (i == SIZE-1 && j == SIZE-1) {
                if (puzzle->board[i][j] != EMPTY) return 0;
            } else {
                if (puzzle->board[i][j] != value++) return 0;
            }
        }
    return 1;
}

int move_empty(PuzzleState *puzzle, char direction) {
    int new_row = puzzle->empty_row;
    int new_col = puzzle->empty_col;

    switch (direction) {
        case 'U': new_row--; break;
        case 'D': new_row++; break;
        case 'L': new_col--; break;
        case 'R': new_col++; break;
        default: return 0;
    }

    if (new_row < 0 || new_row >= SIZE || new_col < 0 || new_col >= SIZE)
        return 0;

    puzzle->board[puzzle->empty_row][puzzle->empty_col] = puzzle->board[new_row][new_col];
    puzzle->board[new_row][new_col] = EMPTY;
    puzzle->empty_row = new_row;
    puzzle->empty_col = new_col;

    return 1;
}

void shuffle_puzzle(PuzzleState *puzzle, int moves) {
    srand(time(NULL));
    char directions[] = {'U', 'D', 'L', 'R'};

    for (int i = 0; i < moves; i++) {
        char dir = directions[rand() % 4];
        if (is_valid_move(puzzle, dir)) {
            move_empty(puzzle, dir);
        }
    }
}

int is_valid_move(const PuzzleState *puzzle, char direction) {
    switch (direction) {
        case 'U': return puzzle->empty_row > 0;
        case 'D': return puzzle->empty_row < SIZE-1;
        case 'L': return puzzle->empty_col > 0;
        case 'R': return puzzle->empty_col < SIZE-1;
        default: return 0;
    }
}

void manual_solve_mode() {
    PuzzleState puzzle;
    initialize_puzzle(&puzzle, 1);

    printf("\n=== Manual Solving Mode ===\n");
    printf("Use U (Up), D (Down), L (Left), R (Right) to move the empty space.\n");
    printf("Enter 'Q' to quit to main menu.\n");

    while (1) {
        print_puzzle(&puzzle);

        if (is_solved(&puzzle)) {
            printf("\nCongratulations! You solved the puzzle!\n");
            return;
        }

        printf("Your move (U/D/L/R/Q): ");
        char input[10];
        scanf("%s", input);

        if (input[0] == 'Q' || input[0] == 'q') return;
        if (strlen(input) != 1 || !strchr("UDLRudlr", input[0])) {
            printf("Invalid input. Please enter U, D, L, or R.\n");
            continue;
        }

        char direction = toupper(input[0]);
        if (!is_valid_move(&puzzle, direction)) {
            printf("Invalid move. Try another direction.\n");
            continue;
        }

        move_empty(&puzzle, direction);
    }
}

void automatic_solve_mode() {
    PuzzleState puzzle;
    initialize_puzzle(&puzzle, 1);

    printf("\n=== Automatic Solving Mode ===\n");
    print_puzzle(&puzzle);

    printf("\nChoose solver:\n");
    printf("1. Depth-Limited Search (DLS)\n");
    printf("2. A* Search (recommended)\n");
    printf("Choice: ");

    int choice;
    scanf("%d", &choice);

    if (choice != 1 && choice != 2) {
        printf("Invalid choice.\n");
        return;
    }

    int nodes_expanded = 0;
    Node* solved_node = NULL;
    clock_t start = clock();

    if (choice == 1) {
        printf("\nSolving with Depth-Limited Search...\n");
        for (int limit = 1; limit <= 31; limit++) {
            printf("Trying depth limit %d...\r", limit);
            fflush(stdout);
            solved_node = depth_limited_search(&puzzle, limit, &nodes_expanded);
            if (solved_node && solved_node->action != 'C' && solved_node->action != 'F')
                break;
        }
    } else {
        printf("\nSolving with A* Search...\n");
        solved_node = a_star_search(&puzzle, &nodes_expanded);
    }

    clock_t end = clock();
    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;

    if (solved_node && solved_node->action != 'C' && solved_node->action != 'F') {
        printf("\nPuzzle solved successfully!\n");
        printf("Nodes expanded: %d\n", nodes_expanded);
        printf("Time taken: %.2f seconds\n", time_taken);
        print_puzzle(&solved_node->state);
        displayActionsPlan(solved_node);
        free_nodes(solved_node);
    } else {
        printf("\nFailed to solve the puzzle.\n");
    }
}

int count_inversions(const PuzzleState *puzzle) {
    int flat[SIZE*SIZE];
    int k = 0;
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            flat[k++] = puzzle->board[i][j];

    int inversions = 0;
    for (int i = 0; i < SIZE*SIZE - 1; i++) {
        for (int j = i + 1; j < SIZE*SIZE; j++) {
            if (flat[i] != EMPTY && flat[j] != EMPTY && flat[i] > flat[j]) {
                inversions++;
            }
        }
    }
    return inversions;
}

int is_solvable(const PuzzleState *puzzle) {
    int inversions = count_inversions(puzzle);
    return (inversions % 2) == 0;
}

int main() {
    menu();
    while (1) {
        print_menu();
        int choice;
        scanf("%d", &choice);

        switch (choice) {
            case 1: manual_solve_mode(); break;
            case 2: automatic_solve_mode(); break;
            case 3: printf("Exiting game. Goodbye!\n"); return 0;
            default: printf("Invalid choice. Please try again.\n");
        }
    }
    return 0;
}
