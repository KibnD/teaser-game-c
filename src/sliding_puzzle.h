#ifndef SLIDING_PUZZLE_H
#define SLIDING_PUZZLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <math.h>

#define SIZE 3
#define EMPTY 0

// Structure représentant un état du puzzle
typedef struct {
    int board[SIZE][SIZE];
    int empty_row;
    int empty_col;
} PuzzleState;

// Structure représentant un noeud de l'arbre de recherche
typedef struct Node {
    PuzzleState state;
    int g; // coût depuis l’état initial
    int h; // coût heuristique
    int f; // f = g + h
    char action; // action qui a mené à cet état
    struct Node* parent;
    struct Node* next;
} Node;

// Structure de file de priorité pour A*
typedef struct {
    Node** nodes;
    int size;
    int capacity;
} PriorityQueue;

// Fonctions du jeu
void initialize_puzzle(PuzzleState *puzzle, int shuffle);
void print_puzzle(const PuzzleState *puzzle);
int is_solved(const PuzzleState *puzzle);
int move_empty(PuzzleState *puzzle, char direction);
void shuffle_puzzle(PuzzleState *puzzle, int moves);

// Modes de jeu
void manual_solve_mode();
void automatic_solve_mode();

// Fonctions de résolution (fix here)
Node* depth_limited_search(PuzzleState *puzzle, int limit, int *nodes_expanded);
Node* a_star_search(PuzzleState *puzzle, int *nodes_expanded);

// Fonctions utilitaires
int is_valid_move(const PuzzleState *puzzle, char direction);
int manhattan_distance(const PuzzleState *puzzle);
int count_inversions(const PuzzleState *puzzle);
int is_solvable(const PuzzleState *puzzle);

// Manipulation de noeuds
Node* create_node(PuzzleState state, Node* parent);
void free_nodes(Node *node);
void print_solution(Node *node);
int count_nodes(Node *node);

// Fonctions demandées dans le sujet
void initPuzzleState(PuzzleState *puzzle, const char* state);
void displayBoard(const int* board);
int isGoal(const int board[SIZE][SIZE]);
void findTile(const int board[SIZE][SIZE], int tile, int *row, int *col);
void getActions(const int board[SIZE][SIZE], char actions[4]);
Node* getSuccessor(Node* node, char action);
void expand(Node* node, Node** children, int* childCount);
void displayActionsPlan(Node* node);

// File de priorité
void push(PriorityQueue* pq, Node* state);
Node* pop(PriorityQueue* pq);

#endif