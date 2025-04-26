#include "Teaser.h"

int main() {
    int board[SIZE][SIZE];
    
    // Initialize the board with a default puzzle state
    initPuzzleState(board, "283164705");

    int choice;
    do {
        menu();
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Exiting...\n");
            break;
        }

        switch (choice) {
            case 1:
                playManualMode(board);
                break;
            case 2:
                playAutoMode(board);
                break;
            case 3:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice! Please select 1, 2, or 3.\n");
        }
    } while (choice != 3);

    return 0;
}
