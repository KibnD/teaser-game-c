#include "Teaser.h"

// État but global
int goal[SIZE][SIZE] = {
    {1, 2, 3},
    {4, 5, 6},
    {7, 8, 0}
};

// Variables globales pour les nœuds spéciaux
Node cutoff = { .action = 'C' };
Node failure = { .action = 'F' };

// Fonctions utilitaires
void initPuzzleState(int board[SIZE][SIZE], const char* state) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            int index = i * SIZE + j;
            board[i][j] = (state[index] == ' ') ? 0 : state[index] - '0';
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
    printf(" +-----+\n\n");
}

int isGoal(const int board[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] != goal[i][j]) {
                return 0;
            }
        }
    }
    return 1;
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

    if (row == 0) actions[0] = ' '; // Supprimer Up
    if (row == SIZE-1) actions[1] = ' '; // Supprimer Down
    if (col == 0) actions[2] = ' '; // Supprimer Left
    if (col == SIZE-1) actions[3] = ' '; // Supprimer Right

    return actions;
}

Node* getSuccessor(Node* node, char action) {
    if (action == ' ') return NULL;

    int row, col;
    findTile(node->board, 0, &row, &col);

    Node* successor = (Node*)malloc(sizeof(Node));
    copyBoard(successor->board, node->board);
    successor->g = node->g + 1;
    successor->parent = node;
    successor->action = action;

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
    if (node == NULL) return;
    
    char* possibleActions = actions(node->board);
    *childCount = 0;
    
    for (int i = 0; i < MAX_ACTIONS; i++) {
        if (possibleActions[i] != ' ') {
            children[*childCount] = getSuccessor(node, possibleActions[i]);
            if (children[*childCount] != NULL) {
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
    if (node == NULL) return;
    
    // Compter le nombre d'actions
    int count = 0;
    Node* current = node;
    while (current->parent != NULL) {
        count++;
        current = current->parent;
    }
    
    // Stocker les actions dans l'ordre inverse
    char* actions = (char*)malloc(count * sizeof(char));
    current = node;
    for (int i = count - 1; i >= 0; i--) {
        actions[i] = current->action;
        current = current->parent;
    }
    
    // Afficher le plan d'actions
    printf("Le plan d'action est:\n");
    for (int i = 0; i < count; i++) {
        printf("%c ", actions[i]);
    }
    printf("\nNombre de mouvements: %d\n", count);
    
    free(actions);
}

void copyBoard(int dest[SIZE][SIZE], const int src[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            dest[i][j] = src[i][j];
        }
    }
}

// Implémentation de DLS
Node* depth_limited_search(Node* initial_node, int depth_limit) {
    return recursive_dls(initial_node, depth_limit);
}

Node* recursive_dls(Node* node, int depth_limit) {
    if (isGoal(node->board)) {
        return node;
    } else if (depth_limit == 0) {
        return &cutoff;
    } else {
        Node* children[4];
        int childCount = 0;
        expand(node, children, &childCount);
        
        for (int i = 0; i < childCount; i++) {
            Node* result = recursive_dls(children[i], depth_limit - 1);
            if (result == &cutoff) {
                continue;
            } else if (result != &failure) {
                // Libérer les autres enfants
                for (int j = i+1; j < childCount; j++) {
                    free(children[j]);
                }
                return result;
            }
        }
        
        // Libérer tous les enfants
        for (int i = 0; i < childCount; i++) {
            free(children[i]);
        }
        return &failure;
    }
}

// Implémentation de A*
void push(PriorityQueue* pq, Node* state) {
    if (pq->size >= pq->capacity) {
        pq->capacity *= 2;
        pq->nodes = (Node**)realloc(pq->nodes, pq->capacity * sizeof(Node*));
    }
    
    // Trouver la position d'insertion
    int i = pq->size;
    while (i > 0 && (pq->nodes[i-1]->g + pq->nodes[i-1]->h) > (state->g + state->h)) {
        pq->nodes[i] = pq->nodes[i-1];
        i--;
    }
    
    pq->nodes[i] = state;
    pq->size++;
}

Node* pop(PriorityQueue* pq) {
    if (pq->size == 0) return NULL;
    pq->size--;
    return pq->nodes[pq->size];
}

void freePriorityQueue(PriorityQueue* pq) {
    free(pq->nodes);
    pq->nodes = NULL;
    pq->size = 0;
    pq->capacity = 0;
}

Node* a_star(Node* initial_state) {
    PriorityQueue pq;
    pq.nodes = (Node**)malloc(10 * sizeof(Node*));
    pq.size = 0;
    pq.capacity = 10;
    
    push(&pq, initial_state);
    
    while (pq.size > 0) {
        Node* current = pop(&pq);
        
        if (isGoal(current->board)) {
            freePriorityQueue(&pq);
            return current;
        }
        
        Node* children[4];
        int childCount = 0;
        expand(current, children, &childCount);
        
        for (int i = 0; i < childCount; i++) {
            push(&pq, children[i]);
        }
        
        free(current);
    }
    
    freePriorityQueue(&pq);
    return NULL;
}

void menu() {
    printf("\n");
    printf("\n"); 
    printf("\n");
    printf(" .-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+. \n");
    printf("| .*********************************. |\n");
    printf("| |     Utilisez les lettres        | |\n");
    printf("| |   A pour resolution automatique | |\n");
    printf("| |      H pour l'aide (help)       | |\n");
    printf("| |   U, D, L et R pour jouer.      | |\n"); 
    printf("* *    Votre deplacement doit       * *\n");
    printf("* *   etre valide et faisable       * *\n");
    printf("| |                                 | |\n"); 
    printf("| |                                 | |\n");
    printf("| |     Taper sur la touche 1       | |\n");
    printf("| |   pour commencer la partie      | |\n");
    printf("| |                                 | |\n");
    printf("| '*********************************' |\n");
    printf("'-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+' \n");
    printf("\n");
    printf("\n"); 
}