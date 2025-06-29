#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int choice = 0;
    char input[10];

    while(1) {
        // Clear the screen and move the cursor to the top left
        printf("\033[2J");  // Clear the entire screen
        printf("\033[H");   // Move the cursor to row 1, column 1

        // Print the menu
        printf("Simple Menu\n");
        printf("===========\n");
        printf("1. Option 1\n");
        printf("2. Option 2\n");
        printf("3. Option 3\n");
        printf("4. Exit\n");
        printf("\nEnter your choice: ");
        fflush(stdout);  // Ensure output is displayed immediately

        // Read user input
        if(fgets(input, sizeof(input), stdin) == NULL) {
            continue;
        }
        choice = atoi(input);

        // Process the user's choice
        if(choice == 4) {
            break;  // Exit the menu loop
        } else if(choice >= 1 && choice <= 3) {
            // Clear screen and show confirmation message
            printf("\033[2J");  // Clear the entire screen

            printf("\033[H");   // Move the cursor to row 1, column 1

            printf("You selected Option %d.\n", choice);
            printf("Press Enter to return to the menu...");
            fflush(stdout);
            fgets(input, sizeof(input), stdin);  // Wait for user to press Enter
        } else {
            // Handle invalid choice
            printf("\033[2J");  // Clear the entire screen
            printf("\033[H");   // Move the cursor to row 1, column 1
            printf("Invalid choice. Press Enter to try again...");
            fflush(stdout);
            fgets(input, sizeof(input), stdin);  // Wait for user to press Enter
        }
    }

    // Clear the screen one last time and exit
    printf("\033[2J");  // Clear the entire screen
    printf("\033[H");   // Move the cursor to row 1, column 1
    printf("Goodbye!\n");

    return 0;
}
