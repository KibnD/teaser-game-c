#include "Teaser.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

int goal[SIZE][SIZE] = {
    {1, 2, 3},
    {4, 5, 6},
    {7, 8, 0}
};

Node cutoff = { .action = 'C' };
Node failure = { .action = 'F' };

// Utility functions
void copyBoard(int dest[SIZE][SIZE], const int src[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            dest[i][j] = src[i][j];
        }
    }
}

int compareBoards(const int board1[SIZE][SIZE], const int board2[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board1[i][j] != board2[i][j]) return 0;
        }
    }
    return 1;
}

Node* createNode(int board[SIZE][SIZE], Node* parent, int g, int h, char action) {
    Node* node = (Node*)malloc(sizeof(Node));
    if (!node) return NULL;
    copyBoard(node->board, board);
    node->parent = parent;
    node->g = g;
    node->h = h;
    node->action = action;
    return node;
}

void initPuzzleState(int board[SIZE][SIZE], const char* state) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            int index = i * SIZE + j;
            board[i][j] = (state[index] == '0') ? 0 : state[index] - '0';
        }
    }
}

void displayBoard(const int board[SIZE][SIZE]) {
    printf(" +-----+\n");
    for (int i = 0; i < SIZE; i++) {
        printf(" |");
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == 0) {
                printf("  ");
            } else {
                printf("%d ", board[i][j]);
            }
        }
        printf("|\n");
    }
    printf(" +-----+\n");
}

int isGoal(const int board[SIZE][SIZE]) {
    return compareBoards(board, goal);
}

void findTile(const int board[SIZE][SIZE], int tile, int* row, int* col) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == tile) {
                *row = i;
                *col = j;
                return;
            }
        }
    }
    *row = -1;
    *col = -1;
}

char* actions(const int board[SIZE][SIZE]) {
    static char actions[MAX_ACTIONS] = {'U', 'D', 'L', 'R'};
    int row, col;
    findTile(board, 0, &row, &col);

    if (row == 0) actions[0] = ' ';
    if (row == SIZE-1) actions[1] = ' ';
    if (col == 0) actions[2] = ' ';
    if (col == SIZE-1) actions[3] = ' ';

    return actions;
}

Node* getSuccessor(Node* node, char action) {
    if (action == ' ') return NULL;

    int row, col;
    findTile(node->board, 0, &row, &col);

    Node* successor = createNode(node->board, node, node->g + 1, 0, action);
    if (!successor) return NULL;

    switch (action) {
        case 'U':
            successor->board[row][col] = successor->board[row-1][col];
            successor->board[row-1][col] = 0;
            break;
        case 'D':
            successor->board[row][col] = successor->board[row+1][col];
            successor->board[row+1][col] = 0;
            break;
        case 'L':
            successor->board[row][col] = successor->board[row][col-1];
            successor->board[row][col-1] = 0;
            break;
        case 'R':
            successor->board[row][col] = successor->board[row][col+1];
            successor->board[row][col+1] = 0;
            break;
    }

    successor->h = manhattanDistance(successor->board);
    return successor;
}

void expand(Node* node, Node** children, int* childCount) {
    if (!node) return;

    char* possibleActions = actions(node->board);
    *childCount = 0;

    for (int i = 0; i < MAX_ACTIONS; i++) {
        if (possibleActions[i] != ' ') {
            children[*childCount] = getSuccessor(node, possibleActions[i]);
            if (children[*childCount]) {
                (*childCount)++;
            }
        }
    }
}

int manhattanDistance(const int board[SIZE][SIZE]) {
    int distance = 0;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            int value = board[i][j];
            if (value != 0) {
                int target_row = (value - 1) / SIZE;
                int target_col = (value - 1) % SIZE;
                distance += abs(i - target_row) + abs(j - target_col);
            }
        }
    }
    return distance;
}

int hammingDistance(const int board[SIZE][SIZE]) {
    int distance = 0;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] != 0 && board[i][j] != goal[i][j]) {
                distance++;
            }
        }
    }
    return distance;
}

void displayActionsPlan(Node* node) {
    if (!node) return;

    int count = 0;
    Node* current = node;
    while (current->parent) {
        count++;
        current = current->parent;
    }

    char* actions = (char*)malloc(count * sizeof(char));
    current = node;
    for (int i = count - 1; i >= 0; i--) {
        actions[i] = current->action;
        current = current->parent;
    }

    printf("Solution path (%d moves): ", count);
    for (int i = 0; i < count; i++) {
        printf("%c ", actions[i]);
    }
    printf("\n");

    free(actions);
}

// Algorithms
Node* recursive_dls(Node* node, int depth_limit) {
    if (isGoal(node->board)) return node;
    if (depth_limit == 0) return &cutoff;

    Node* children[MAX_ACTIONS];
    int childCount = 0;
    expand(node, children, &childCount);

    for (int i = 0; i < childCount; i++) {
        Node* result = recursive_dls(children[i], depth_limit - 1);
        if (result != &cutoff && result != &failure) {
            for (int j = i + 1; j < childCount; j++) free(children[j]);
            return result;
        }
        free(children[i]);
    }
    return &failure;
}

Node* depth_limited_search(Node* initial_node, int depth_limit) {
    return recursive_dls(initial_node, depth_limit);
}

// Priority Queue
void push(PriorityQueue* pq, Node* state) {
    if (!pq || !state) return;

    if (pq->size >= pq->capacity) {
        pq->capacity *= 2;
        Node** new_nodes = (Node**)realloc(pq->nodes, pq->capacity * sizeof(Node*));
        if (!new_nodes) return;
        pq->nodes = new_nodes;
    }

    int i = pq->size;
    while (i > 0 && (pq->nodes[i-1]->g + pq->nodes[i-1]->h) > (state->g + state->h)) {
        pq->nodes[i] = pq->nodes[i-1];
        i--;
    }
    pq->nodes[i] = state;
    pq->size++;
}

Node* pop(PriorityQueue* pq) {
    if (!pq || pq->size == 0) return NULL;
    return pq->nodes[--pq->size];
}

void freePriorityQueue(PriorityQueue* pq) {
    if (pq) {
        free(pq->nodes);
        pq->nodes = NULL;
        pq->size = pq->capacity = 0;
    }
}

Node* a_star(Node* initial_state) {
    if (!initial_state) return NULL;

    PriorityQueue pq = { .nodes = malloc(10 * sizeof(Node*)), .size = 0, .capacity = 10 };
    if (!pq.nodes) return NULL;

    push(&pq, initial_state);

    while (pq.size > 0) {
        Node* current = pop(&pq);
        if (isGoal(current->board)) {
            freePriorityQueue(&pq);
            return current;
        }

        Node* children[MAX_ACTIONS];
        int childCount = 0;
        expand(current, children, &childCount);

        for (int i = 0; i < childCount; i++) {
            if (children[i]) push(&pq, children[i]);
        }
        free(current);
    }

    freePriorityQueue(&pq);
    return NULL;
}

// Game functions
void playManualMode(int board[SIZE][SIZE]) {
    char move;
    while (!isGoal(board)) {
        displayBoard(board);
        printf("Enter move (U/D/L/R): ");
        scanf(" %c", &move);

        int row, col;
        findTile(board, 0, &row, &col);

        bool valid = false;
        switch (move) {
            case 'U': valid = (row > 0); break;
            case 'D': valid = (row < SIZE-1); break;
            case 'L': valid = (col > 0); break;
            case 'R': valid = (col < SIZE-1); break;
            default: printf("Invalid move!\n");
        }

        if (valid) {
            Node temp = { .board = {{0}} };
            copyBoard(temp.board, board);
            Node* successor = getSuccessor(&temp, move);
            if (successor) {
                copyBoard(board, successor->board);
                free(successor);
            }
        }
    }
    displayBoard(board);
    printf("Congratulations! You solved the puzzle!\n");
}

void playAutoMode(int board[SIZE][SIZE]) {
    Node* initial_node = createNode(board, NULL, 0, manhattanDistance(board), 'S');
    if (!initial_node) {
        printf("Failed to create initial node.\n");
        return;
    }

    Node* solution = a_star(initial_node);
    if (!solution) {
        printf("No solution found.\n");
        return;
    }

    printf("Automatic solution found!\n");
    displayActionsPlan(solution);
    printf("Final board:\n");
    displayBoard(solution->board);

    // Free the solution path
    Node* current = solution;
    while (current) {
        Node* temp = current->parent;
        free(current);
        current = temp;
    }
}
void menu() {
    printf("\n=== 8 Puzzle Menu ===\n");
    printf("1. Play manually\n");
    printf("2. Solve automatically\n");
    printf("3. Exit\n");
    printf("Enter your choice: ");
}

