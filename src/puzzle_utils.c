#include "sliding_puzzle.h"

// Affiche récursivement les états d'une solution depuis le début jusqu'à la fin
void print_solution(Node *node) {
    if (!node) return;
    print_solution(node->parent);
    print_puzzle(&node->state);
    printf("\n");
}

// Compte le nombre de noeuds d'une liste chaînée
int count_nodes(Node *node) {
    int count = 0;
    while (node) {
        count++;
        node = node->next;
    }
    return count;
}

// Initialise un état de puzzle à partir d'une chaîne de caractères
void initPuzzleState(PuzzleState *puzzle, const char* state) {
    int k = 0;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            char ch = state[k++];
            if (ch == ' ' || ch == '0') {
                puzzle->board[i][j] = EMPTY;
                puzzle->empty_row = i;
                puzzle->empty_col = j;
            } else {
                puzzle->board[i][j] = ch - '0';
            }
        }
    }
}

// Affiche le tableau comme spécifié dans le document
void displayBoard(const int* board) {
    printf(" +-----+\n");
    for (int i = 0; i < 9; i++) {
        if (i % 3 == 0) printf(" |");
        printf("%d ", board[i]);
        if ((i + 1) % 3 == 0) printf("|\n");
    }
    printf(" +-----+\n\n");
}

// Vérifie si un état est l'état but (objectif)
int isGoal(const int board[SIZE][SIZE]) {
    int value = 1;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (i == SIZE - 1 && j == SIZE - 1) {
                if (board[i][j] != EMPTY) return 0;
            } else {
                if (board[i][j] != value++) return 0;
            }
        }
    }
    return 1;
}

// Trouve la position (ligne, colonne) d'une tuile
void findTile(const int board[SIZE][SIZE], int tile, int *row, int *col) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == tile) {
                *row = i;
                *col = j;
                return;
            }
        }
    }
}

// Retourne les actions possibles (U, D, L, R) pour un état donné
void getActions(const int board[SIZE][SIZE], char actions[4]) {
    actions[0] = 'U';
    actions[1] = 'D';
    actions[2] = 'L';
    actions[3] = 'R';

    int i, j;
    findTile(board, EMPTY, &i, &j);

    if (i == 0) actions[0] = ' ';
    if (i == SIZE - 1) actions[1] = ' ';
    if (j == 0) actions[2] = ' ';
    if (j == SIZE - 1) actions[3] = ' ';
}
