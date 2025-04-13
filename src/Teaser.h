#ifndef TEASER_H
#define TEASER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

#define SIZE 3
#define MAX_ACTIONS 4

typedef struct Node {
    int board[SIZE][SIZE];
    int g;
    int h;
    struct Node* parent;
    char action;
} Node;

typedef struct {
    Node** nodes;
    int size;
    int capacity;
} PriorityQueue;

// Fonctions utilitaires
void initPuzzleState(int board[SIZE][SIZE], const char* state);
void displayBoard(const int board[SIZE][SIZE]);
int isGoal(const int board[SIZE][SIZE]);
void findTile(const int board[SIZE][SIZE], int tile, int* row, int* col);
char* actions(const int board[SIZE][SIZE]);
Node* getSuccessor(Node* node, char action);
void expand(Node* node, Node** children, int* childCount);
void displayActionsPlan(Node* node);
int manhattanDistance(const int board[SIZE][SIZE]);
int hammingDistance(const int board[SIZE][SIZE]);

// Algorithmes
Node* depth_limited_search(Node* initial_node, int depth_limit);
Node* recursive_dls(Node* node, int depth_limit);
Node* a_star(Node* initial_state);

// File de priorité
void push(PriorityQueue* pq, Node* state);
Node* pop(PriorityQueue* pq);
void freePriorityQueue(PriorityQueue* pq);

// Menu
void menu();

#endif