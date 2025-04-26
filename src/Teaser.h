#ifndef TEASER_H
#define TEASER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <stdbool.h>

#define SIZE 3
#define MAX_ACTIONS 4

// Node representing a puzzle state
typedef struct Node {
    int board[SIZE][SIZE];
    int g;                  // Cost from start node
    int h;                  // Heuristic cost to goal
    struct Node* parent;    // Pointer to parent node
    char action;            // Action taken to reach this node
} Node;

// Priority Queue for A* algorithm
typedef struct {
    Node** nodes;
    int size;
    int capacity;
} PriorityQueue;

// ----------- Utility functions -----------
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
void copyBoard(int dest[SIZE][SIZE], const int src[SIZE][SIZE]);
int compareBoards(const int board1[SIZE][SIZE], const int board2[SIZE][SIZE]);
Node* createNode(int board[SIZE][SIZE], Node* parent, int g, int h, char action);

// ----------- Search Algorithms -----------
Node* depth_limited_search(Node* initial_node, int depth_limit);
Node* recursive_dls(Node* node, int depth_limit);
Node* a_star(Node* initial_state);

// ----------- Priority Queue operations -----------
void push(PriorityQueue* pq, Node* state);
Node* pop(PriorityQueue* pq);
void freePriorityQueue(PriorityQueue* pq);

// ----------- Game modes and menu -----------
void playManualMode(int board[SIZE][SIZE]);
void playAutoMode(int board[SIZE][SIZE]);
void menu();

#endif // TEASER_H
