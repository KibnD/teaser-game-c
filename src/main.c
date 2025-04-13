#include "Teaser.h"

int main() {
    // Initialisation
    char initial_state_str[] = "283164705";
    Node initial_node;
    initPuzzleState(initial_node.board, initial_state_str);
    initial_node.g = 0;
    initial_node.h = manhattanDistance(initial_node.board);
    initial_node.parent = NULL;
    initial_node.action = '\0';

    // Affichage initial
    printf("État initial:\n");
    displayBoard(initial_node.board);

    // Menu interactif
    menu();

    // Tests algorithmes
    Node* solution = a_star(&initial_node);
    if (solution) {
        displayActionsPlan(solution);
        free(solution);
    }

    return 0;
}