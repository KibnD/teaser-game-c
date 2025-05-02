#include "sliding_puzzle.h"

#define MAX_NODES 100000

// Crée un nouveau noeud
Node* create_node(PuzzleState state, Node* parent) {
    Node* node = (Node*)malloc(sizeof(Node));
    if (!node) return NULL;

    memcpy(&node->state, &state, sizeof(PuzzleState));
    node->parent = parent;
    node->next = NULL;

    node->action = '?';
    if (parent) node->g = parent->g + 1;
    else node->g = 0;

    node->h = manhattan_distance(&state);
    node->f = node->g + node->h;

    return node;
}

// Libère la mémoire de tous les noeuds d'une liste
void free_nodes(Node *node) {
    while (node) {
        Node *temp = node;
        node = node->next;
        free(temp);
    }
}

// Heuristique de Manhattan
int manhattan_distance(const PuzzleState *puzzle) {
    int distance = 0;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            int value = puzzle->board[i][j];
            if (value != EMPTY) {
                int target_row = (value - 1) / SIZE;
                int target_col = (value - 1) % SIZE;
                distance += abs(i - target_row) + abs(j - target_col);
            }
        }
    }
    return distance;
}

// Génère un successeur à partir d'un noeud et d'une action
Node* getSuccessor(Node* node, char action) {
    if (action == ' ') return NULL;

    int i = node->state.empty_row;
    int j = node->state.empty_col;

    PuzzleState next = node->state;

    switch (action) {
        case 'U':
            next.board[i][j] = next.board[i - 1][j];
            next.board[i - 1][j] = EMPTY;
            next.empty_row = i - 1;
            break;
        case 'D':
            next.board[i][j] = next.board[i + 1][j];
            next.board[i + 1][j] = EMPTY;
            next.empty_row = i + 1;
            break;
        case 'L':
            next.board[i][j] = next.board[i][j - 1];
            next.board[i][j - 1] = EMPTY;
            next.empty_col = j - 1;
            break;
        case 'R':
            next.board[i][j] = next.board[i][j + 1];
            next.board[i][j + 1] = EMPTY;
            next.empty_col = j + 1;
            break;
        default:
            return NULL;
    }

    Node* child = create_node(next, node);
    child->action = action;
    return child;
}

// Génère les enfants d'un noeud (successeurs)
void expand(Node* node, Node** children, int* childCount) {
    if (!node) return;

    char actions[4];
    getActions(node->state.board, actions);
    *childCount = 0;

    for (int i = 0; i < 4; i++) {
        if (actions[i] != ' ') {
            Node* child = getSuccessor(node, actions[i]);
            if (child) {
                children[*childCount] = child;
                (*childCount)++;
            }
        }
    }
}

// Affiche le plan d’actions depuis la racine
void displayActionsPlan(Node* node) {
    if (!node) return;

    char stack[100];
    int top = -1;

    while (node && node->parent) {
        stack[++top] = node->action;
        node = node->parent;
    }

    printf("\nLe plan d'action est :\n");
    while (top >= 0) {
        printf("%c ", stack[top--]);
    }
    printf("\n");
}

// Compare deux états
int states_equal(const PuzzleState *a, const PuzzleState *b) {
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            if (a->board[i][j] != b->board[i][j])
                return 0;
    return 1;
}

// Implémentation de Depth Limited Search (DLS)
Node* recursive_dls(Node* node, int limit) {
    if (is_solved(&node->state)) return node;
    else if (limit == 0) {
        Node* cutoff = malloc(sizeof(Node));
        cutoff->action = 'C';
        return cutoff;
    }

    Node* children[4];
    int childCount = 0;
    expand(node, children, &childCount);

    int cutoff_occurred = 0;
    for (int i = 0; i < childCount; i++) {
        Node* result = recursive_dls(children[i], limit - 1);
        if (result && result->action == 'C') {
            cutoff_occurred = 1;
        } else if (result && result->action != 'F') {
            return result;
        }
    }

    Node* ret = malloc(sizeof(Node));
    ret->action = cutoff_occurred ? 'C' : 'F';
    return ret;
}

Node* depth_limited_search(PuzzleState *puzzle, int limit, int *nodes_expanded) {
    *nodes_expanded = 0;
    Node* start = create_node(*puzzle, NULL);
    return recursive_dls(start, limit);
}

// A* search helpers
Node* find_node_in_list(Node *list, PuzzleState *state) {
    while (list) {
        if (states_equal(&list->state, state)) {
            return list;
        }
        list = list->next;
    }
    return NULL;
}

void add_to_open_list(Node **open_list, Node *node) {
    if (!*open_list || node->f < (*open_list)->f) {
        node->next = *open_list;
        *open_list = node;
    } else {
        Node *current = *open_list;
        while (current->next && current->next->f <= node->f) {
            current = current->next;
        }
        node->next = current->next;
        current->next = node;
    }
}

Node* a_star_search(PuzzleState *puzzle, int *nodes_expanded) {
    Node *open_list = NULL;
    Node *closed_list = NULL;
    *nodes_expanded = 0;

    open_list = create_node(*puzzle, NULL);
    if (!open_list) return NULL;

    while (open_list) {
        Node *current = open_list;
        open_list = open_list->next;

        if (is_solved(&current->state)) {
            free_nodes(open_list);
            free_nodes(closed_list);
            return current;
        }

        (*nodes_expanded)++;

        char directions[] = {'U', 'D', 'L', 'R'};
        for (int i = 0; i < 4; i++) {
            if (is_valid_move(&current->state, directions[i])) {
                PuzzleState next_state = current->state;
                move_empty(&next_state, directions[i]);
                if (find_node_in_list(closed_list, &next_state)) continue;

                Node *successor = create_node(next_state, current);
                successor->action = directions[i];
                if (!successor) {
                    free_nodes(open_list);
                    free_nodes(closed_list);
                    return NULL;
                }

                Node *existing = find_node_in_list(open_list, &next_state);
                if (existing) {
                    if (successor->g < existing->g) {
                        existing->g = successor->g;
                        existing->f = existing->g + existing->h;
                        existing->parent = current;
                    }
                    free(successor);
                } else {
                    add_to_open_list(&open_list, successor);
                }
            }
        }

        current->next = closed_list;
        closed_list = current;

        if (*nodes_expanded > MAX_NODES) {
            free_nodes(open_list);
            free_nodes(closed_list);
            return NULL;
        }
    }

    free_nodes(closed_list);
    return NULL;
}
