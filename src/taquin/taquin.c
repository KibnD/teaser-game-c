#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <ctype.h>
#include <limits.h>

#define SIZE 3
#define MAX_ACTIONS 4

typedef struct Node {
    int board[SIZE][SIZE];
    int g;
    int h;
    int f;
    struct Node* parent;
    char action;
} Node;

typedef struct PriorityQueue {
    Node** nodes;
    int size;
    int capacity;
} PriorityQueue;

// Fonctions de base
void initPuzzleState(int board[SIZE][SIZE], const char* state) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            char c = state[i * SIZE + j];
            board[i][j] = (c == ' ') ? 0 : c - '0';
        }
    }
}

void displayBoard(const int board[SIZE][SIZE]) {
    printf(" +---+---+---+\n");
    for (int i = 0; i < SIZE; i++) {
        printf(" |");
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == 0) {
                printf("   |");
            } else {
                printf(" %d |", board[i][j]);
            }
        }
        printf("\n +---+---+---+\n");
    }
    printf("\n");
}

bool isGoal(const int board[SIZE][SIZE]) {
    int goal[SIZE][SIZE] = {{1, 2, 3}, {4, 5, 6}, {7, 8, 0}};
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] != goal[i][j]) {
                return false;
            }
        }
    }
    return true;
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
}

// Fonctions pour les actions
char* getPossibleActions(const int board[SIZE][SIZE]) {
    static char actions[MAX_ACTIONS] = {'U', 'D', 'L', 'R'};
    int emptyRow, emptyCol;
    findTile(board, 0, &emptyRow, &emptyCol);

    if (emptyRow == 0) actions[0] = ' ';
    if (emptyRow == SIZE - 1) actions[1] = ' ';
    if (emptyCol == 0) actions[2] = ' ';
    if (emptyCol == SIZE - 1) actions[3] = ' ';

    return actions;
}

Node* createNode(const int board[SIZE][SIZE], int g, Node* parent, char action) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    memcpy(newNode->board, board, sizeof(int) * SIZE * SIZE);
    newNode->g = g;
    newNode->h = 0;
    newNode->f = 0;
    newNode->parent = parent;
    newNode->action = action;
    return newNode;
}

// Heuristiques
int manhattanDistance(const Node* node) {
    int distance = 0;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            int value = node->board[i][j];
            if (value != 0) {
                int targetRow = (value - 1) / SIZE;
                int targetCol = (value - 1) % SIZE;
                distance += abs(i - targetRow) + abs(j - targetCol);
            }
        }
    }
    return distance;
}

// Génération des successeurs
Node* getSuccessor(Node* node, char action) {
    if (action == ' ') return NULL;

    int emptyRow, emptyCol;
    findTile(node->board, 0, &emptyRow, &emptyCol);

    int newRow = emptyRow, newCol = emptyCol;
    switch (action) {
        case 'U': newRow--; break;
        case 'D': newRow++; break;
        case 'L': newCol--; break;
        case 'R': newCol++; break;
        default: return NULL;
    }

    if (newRow < 0 || newRow >= SIZE || newCol < 0 || newCol >= SIZE) {
        return NULL;
    }

    int newBoard[SIZE][SIZE];
    memcpy(newBoard, node->board, sizeof(int) * SIZE * SIZE);

    newBoard[emptyRow][emptyCol] = newBoard[newRow][newCol];
    newBoard[newRow][newCol] = 0;

    Node* successor = createNode(newBoard, node->g + 1, node, action);
    successor->h = manhattanDistance(successor);
    successor->f = successor->g + successor->h;

    return successor;
}

void expand(Node* node, Node** children, int* childCount) {
    if (node == NULL) return;

    char* actions = getPossibleActions(node->board);
    *childCount = 0;

    for (int i = 0; i < MAX_ACTIONS; i++) {
        if (actions[i] != ' ') {
            Node* child = getSuccessor(node, actions[i]);
            if (child != NULL) {
                children[(*childCount)++] = child;
            }
        }
    }
}

// Algorithme DLS
Node cutoffNode = { .action = 'C' };
Node failureNode = { .action = 'F' };

Node* recursiveDLS(Node* node, int depthLimit) {
    if (isGoal(node->board)) {
        return node;
    }
    if (depthLimit <= 0) {
        return &cutoffNode;
    }

    Node* children[MAX_ACTIONS];
    int childCount = 0;
    expand(node, children, &childCount);

    for (int i = 0; i < childCount; i++) {
        Node* result = recursiveDLS(children[i], depthLimit - 1);
        if (result == &cutoffNode) {
            continue;
        }
        if (result != &failureNode) {
            return result;
        }
    }
    return &failureNode;
}

bool isSolvable(const int board[SIZE][SIZE]) {
    int inversions = 0;
    int tiles[SIZE*SIZE];
    int k = 0;
    int blankRow = 0;
    
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == 0) {
                blankRow = SIZE - i;
            } else {
                tiles[k++] = board[i][j];
            }
        }
    }

    for (int i = 0; i < k-1; i++) {
        for (int j = i+1; j < k; j++) {
            if (tiles[i] > tiles[j]) {
                inversions++;
            }
        }
    }

    if (SIZE % 2 == 1) {
        return (inversions % 2 == 0);
    } else {
        return ((inversions + blankRow) % 2 == 1);
    }
}

Node* depthLimitedSearch(Node* initialNode, int depthLimit) {
    if (!isSolvable(initialNode->board)) {
        printf("Ce puzzle n'a pas de solution!\n");
        return &failureNode;
    }

    if (depthLimit > 50) {
        depthLimit = 50;
    }

    return recursiveDLS(initialNode, depthLimit);
}

Node* IDDFS(Node* initialNode) {
    for (int depth = 0; depth <= 50; depth++) {
        Node* result = depthLimitedSearch(initialNode, depth);
        if (result != &cutoffNode && result != &failureNode) {
            return result;
        }
    }
    return &failureNode;
}

// Algorithme A*
void push(PriorityQueue* pq, Node* node) {
    if (pq->size >= pq->capacity) {
        pq->capacity *= 2;
        pq->nodes = realloc(pq->nodes, sizeof(Node*) * pq->capacity);
    }

    int i = pq->size;
    while (i > 0 && node->f < pq->nodes[(i - 1) / 2]->f) {
        pq->nodes[i] = pq->nodes[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    pq->nodes[i] = node;
    pq->size++;
}

Node* pop(PriorityQueue* pq) {
    if (pq->size == 0) return NULL;

    Node* result = pq->nodes[0];
    pq->size--;

    Node* last = pq->nodes[pq->size];
    int i = 0;
    while (2 * i + 1 < pq->size) {
        int child = 2 * i + 1;
        if (child + 1 < pq->size && pq->nodes[child + 1]->f < pq->nodes[child]->f) {
            child++;
        }

        if (last->f <= pq->nodes[child]->f) break;

        pq->nodes[i] = pq->nodes[child];
        i = child;
    }
    pq->nodes[i] = last;

    return result;
}

Node* aStar(Node* initialNode) {
    PriorityQueue pq = {
        .nodes = malloc(sizeof(Node*) * 1000),
        .size = 0,
        .capacity = 1000
    };
    push(&pq, initialNode);

    while (pq.size > 0) {
        Node* current = pop(&pq);

        if (isGoal(current->board)) {
            free(pq.nodes);
            return current;
        }

        Node* children[MAX_ACTIONS];
        int childCount = 0;
        expand(current, children, &childCount);

        for (int i = 0; i < childCount; i++) {
            push(&pq, children[i]);
        }
    }

    free(pq.nodes);
    return NULL;
}

void displaySolutionPath(Node* node) {
    if (node == NULL || node == &failureNode) {
        printf("Aucune solution trouvée.\n");
        return;
    }

    // Compter le nombre d'étapes
    int count = 0;
    Node* current = node;
    while (current->parent != NULL) {
        count++;
        current = current->parent;
    }

    // Stocker le chemin (nœuds et actions)
    Node** path = malloc(count * sizeof(Node*));
    char* actions = malloc(count * sizeof(char));
    
    current = node;
    for (int i = count - 1; i >= 0; i--) {
        path[i] = current;
        actions[i] = current->action;
        current = current->parent;
    }

    // Afficher chaque étape
    printf("\nSolution en %d coups:\n", count);
    for (int i = 0; i < count; i++) {
        printf("Étape %d: Action '%c'\n", i+1, actions[i]);
        displayBoard(path[i]->board);
    }

    // Afficher le résultat final
    printf("Résultat final:\n");
    displayBoard(node->board);

    free(path);
    free(actions);
}

void playManually(int board[SIZE][SIZE]) {
    char input;
    while (true) {
        displayBoard(board);
        printf("Entrez une action (U/D/L/R) ou Q pour quitter: ");
        scanf(" %c", &input);

        if (toupper(input) == 'Q') break;

        int emptyRow, emptyCol;
        findTile(board, 0, &emptyRow, &emptyCol);

        int newRow = emptyRow, newCol = emptyCol;
        switch (toupper(input)) {
            case 'U': newRow--; break;
            case 'D': newRow++; break;
            case 'L': newCol--; break;
            case 'R': newCol++; break;
            default: printf("Action invalide!\n"); continue;
        }

        if (newRow < 0 || newRow >= SIZE || newCol < 0 || newCol >= SIZE) {
            printf("Mouvement impossible!\n");
            continue;
        }

        board[emptyRow][emptyCol] = board[newRow][newCol];
        board[newRow][newCol] = 0;

        if (isGoal(board)) {
            displayBoard(board);
            printf("Félicitations! Vous avez résolu le puzzle!\n");
            break;
        }
    }
}

void menu() {
    printf("\n");
    printf(" +-----------------------------+\n");
    printf("|         MENU PRINCIPAL       |\n");
    printf("| 1. Jouer manuellement        |\n");
    printf("| 2. Résolution automatique DLS|\n");
    printf("| 3. Résolution automatique A* |\n");
    printf("| 4. Quitter                   |\n");
    printf(" +-----------------------------+\n");
    printf("\n");
}

int main() {
    int initialBoard[SIZE][SIZE];
    // Configuration initiale soluble
    initPuzzleState(initialBoard, "123456708");  // Essayez aussi "134862705" pour plus de difficulté

    printf("Configuration initiale:\n");
    displayBoard(initialBoard);

    if (!isSolvable(initialBoard)) {
        printf("Attention: Cette configuration n'a pas de solution!\n");
        return 1;
    }

    int choice;
    do {
        menu();
        printf("Choix: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: {
                int board[SIZE][SIZE];
                memcpy(board, initialBoard, sizeof(int) * SIZE * SIZE);
                playManually(board);
                break;
            }
            case 2: {
                Node* initialNode = createNode(initialBoard, 0, NULL, ' ');
                printf("\nRecherche en cours avec DLS...\n");
                Node* solution = IDDFS(initialNode);
                displaySolutionPath(solution);
                break;
            }
            case 3: {
                Node* initialNode = createNode(initialBoard, 0, NULL, ' ');
                initialNode->h = manhattanDistance(initialNode);
                initialNode->f = initialNode->g + initialNode->h;
                
                printf("\nRecherche en cours avec A*...\n");
                Node* solution = aStar(initialNode);
                displaySolutionPath(solution);
                break;
            }
            case 4:
                printf("Au revoir!\n");
                break;
            default:
                printf("Choix invalide!\n");
        }
    } while (choice != 4);

    return 0;
}