#include <stdio.h>      // Standard input/output library
#include <time.h>       // For handling time-related functions
#include <windows.h>    // For Windows-specific functions (e.g., console handling, system info)
#include <string.h>     // For string manipulation functions
#define MAX_LINE_LENGTH 256 // Define a constant for maximum line length

// Function declarations
void centerText(const char* text); // Function to print text centered on the console
void getDateTime(char* dateStr, size_t size); // Function to store the current date in a string
void header();            // Function to display the header, including hostname and date
void showMenu();          // Function to display the menu and handle user input
void addTodo();           // Function to add a new to-do item
void deleteTodo();        // Function to delete a to-do item
void markAsDone();        // Function to mark a to-do item as done
void showPreviousDays();  // Function to show previous days' to-do lists

int main(void) {
    // Entry point of the program
    header();   // Display header information
    showMenu(); // Display the menu
    return 0;   // Return 0 to indicate successful execution
}


void getDateTime(char* dateStr, size_t size) {
    // Function to store the current date in a string in "Today YYYY-MM-DD" format
    time_t t = time(NULL);          // Get the current time
    struct tm tm = *localtime(&t);  // Convert it to local time structure
    snprintf(dateStr, size, "Today %d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}

void centerText(const char* text) {
    // Function to print text centered on the console screen
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // Get console handle
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo); // Get console info
    int consoleWidth = consoleInfo.dwSize.X;            // Get console width

    int textLength = strlen(text);     // Get the length of the text
    int padding = (consoleWidth - textLength) / 2; // Calculate padding for centering

    if (padding > 0) {
        printf("%*s%s\n", padding, "", text); // Print padded text
    } else {
        printf("%s\n", text);  // If text is wider than console, print as-is
    }
}

void header() {
    // Function to display the header, including hostname and current date

    // Get and print the hostname
    char hostname[MAX_COMPUTERNAME_LENGTH + 1]; // Buffer for hostname
    DWORD size = sizeof(hostname) / sizeof(hostname[0]);
    char helloStr[MAX_LINE_LENGTH]; // Buffer for greeting message

    if (GetComputerName(hostname, &size)) {
        snprintf(helloStr, MAX_LINE_LENGTH, "Hey %s", hostname); // If hostname is available
    } else {
        snprintf(helloStr, MAX_LINE_LENGTH, "Hey"); // Default greeting if hostname is unavailable
    }
    centerText(helloStr); // Print the greeting message centered

    // Get and print the current date
    char dateStr[MAX_LINE_LENGTH];
    getDateTime(dateStr, MAX_LINE_LENGTH); // Get current date as a string
    centerText(dateStr); // Print the date centered

    // Show today's todo (if any)
    time_t t = time(NULL);          // Get the current time
    struct tm tm = *localtime(&t);  // Convert to local time
    char filename[50];
    snprintf(filename, sizeof(filename), "lists\\%04d-%02d-%02d.txt", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);

    FILE *file = fopen(filename, "r"); // Open the todo file for today
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (file == NULL) {
        // If no file is found, print a message
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
        centerText("No to-do list found for today!");
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        return;
    }

    // If the file exists, read and display the todo items
    char buffer[MAX_LINE_LENGTH];
    int currentLine = 1;
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0'; // Remove newline character from buffer
        }

        // Determine priority from the end of the line
        char *priorityPtr = strrchr(buffer, ':');
        char priority = ' ';
        if (priorityPtr != NULL && strlen(priorityPtr) == 2) {
            priority = priorityPtr[1]; // Extract priority
            *priorityPtr = '\0';  // Remove priority from buffer
        }

        // Set console color based on priority
        if (priority == 'A') {
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
        } else if (priority == 'B') {
            SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
        } else if (priority == 'C') {
            SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        } else {
            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        }

        // Create and center the to-do item string
        char todoStr[MAX_LINE_LENGTH];
        snprintf(todoStr, sizeof(todoStr), "%d. %s", currentLine, buffer);
        centerText(todoStr); // Print the to-do item centered

        // Reset console color to default
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        currentLine++;
    }

    fclose(file); // Close the file
}

void showMenu() {
    // Function to display the main menu and handle user input
    printf("1. Add todo For today\n");
    printf("2. Delete\n");
    printf("3. Mark as Done\n"); // Add this option to the menu
    printf("4. Show Previous Days\n");
    printf("5. Exit\n");
    int choice;
    do {
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); // Consume newline character left in the input buffer

        // Call the appropriate function based on user's choice
        switch (choice) {
            case 1:
                addTodo();
                break;
            case 2:
                deleteTodo();
                break;
            case 3:
                markAsDone();
                break;
            case 4:
                showPreviousDays();
                break;
            case 5:
                printf("Goodbye!\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 5); // Loop until user chooses to exit
}

void addTodo() {
    // Function to add a new todo item

    // Get the current date
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    // Create the filename based on the current date
    char filename[50];
    snprintf(filename, sizeof(filename), "lists\\%04d-%02d-%02d.txt", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);

    // Open the file for appending
    FILE* file = fopen(filename, "a");
    if (file == NULL) {
        printf("Error opening file!\n");
        exit(1); // Exit if the file cannot be opened
    }

    // Get the todo item from the user
    char todo[100];
    printf("Enter your todo: ");
    fgets(todo, sizeof(todo), stdin); // Use fgets to read a line of text including spaces

    // Remove the newline character from the to-do string if present
    size_t len = strlen(todo);
    if (len > 0 && todo[len - 1] == '\n') {
        todo[len - 1] = '\0';
    }


    // Get the priority from the user
    printf("Enter Priority A/B/C: ");
    char priority;
    scanf("%c", &priority);

    // Write the to-do item and priority to the file
    fprintf(file, "%s:%c\n", todo, priority);

    // Close the file
    fclose(file);
    system("cls"); // Clear the console screen
    main(); // Restart the main function
}

void deleteTodo() {
    // Function to delete a to-do item
    printf("Enter Number of todo you want to delete: ");
    int todoNumber;
    scanf("%d", &todoNumber);
    getchar(); // Consume newline character left in the input buffer

    // Get the current date
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    // Create the filename based on the current date
    char filename[50];
    snprintf(filename, sizeof(filename), "lists\\%04d-%02d-%02d.txt", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);

    // Open the file for reading
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("No to-do list found for today!\n");
        return;
    }
    
    // Open a temporary file for writing
    char tempFilename[] = "lists\\temp.txt";
    FILE *tempFile = fopen(tempFilename, "w");
    if (tempFile == NULL) {
        printf("Error opening temporary file!\n");
        fclose(file);
        return;
    }
    
    // Read and write lines except the one to delete
    char buffer[256];
    int currentLine = 1;
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        if (currentLine != todoNumber) {
            fputs(buffer, tempFile); // Write all lines except the one to delete
        }
        currentLine++;
    }

    // Close both files
    fclose(file);
    fclose(tempFile);

    // Remove the original file
    if (remove(filename) != 0) {
        printf("Error deleting the original file!\n");
        return;
    }

    // Rename the temporary file to the original file name
    if (rename(tempFilename, filename) != 0) {
        printf("Error renaming the temporary file!\n");
        return;
    }

    printf("Todo %d deleted successfully from %s.\n", todoNumber, filename);
    system("cls"); // Clear the console screen
    main(); // Restart the main function
}

void markAsDone() {
    // Function to mark a to-do item as done
    printf("Enter Number of todo you want to mark as done: ");
    int todoNumber;
    scanf("%d", &todoNumber);
    getchar(); // Consume newline character left in the input buffer

    // Get the current date
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    // Create the filename based on the current date
    char filename[50];
    snprintf(filename, sizeof(filename), "lists\\%04d-%02d-%02d.txt", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);

    // Open the file for reading
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("No to-do list found for today!\n");
        return;
    }

    // Open a temporary file for writing
    char tempFilename[] = "lists\\temp.txt";
    FILE *tempFile = fopen(tempFilename, "w");
    if (tempFile == NULL) {
        printf("Error opening temporary file!\n");
        fclose(file);
        return;
    }

    // Read and write lines, marking the specified item as done
    char buffer[256];
    int currentLine = 1;
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        if (currentLine == todoNumber) {
            // Add a check mark to the line to indicate it is done
            char lineContent[256];
            strcpy(lineContent, buffer); // Copy the buffer content
            char str[] = "*";            // Prefix to mark as done
            strcat(str, lineContent);    // Concatenate the check mark with the original line
            fprintf(tempFile, "%s", str); // Write the modified line
        } else {
            fputs(buffer, tempFile); // Write the original line
        }
        currentLine++;
    }

    // Close both files
    fclose(file);
    fclose(tempFile);

    // Remove the original file
    if (remove(filename) != 0) {
        printf("Error deleting the original file!\n");
        return;
    }

    // Rename the temporary file to the original file name
    if (rename(tempFilename, filename) != 0) {
        printf("Error renaming the temporary file!\n");
        return;
    }

    printf("Todo %d marked as done successfully from %s.\n", todoNumber, filename);
    system("cls"); // Clear the console screen
    main(); // Restart the main function
}

void showPreviousDays() {
    // Function to show previous days' to-do lists
    system("cls"); // Clear the console screen
    printf("Previous Days\n");

    // Get the list of files in the "lists" folder
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile("lists\\*", &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        printf("No files found in the list folder!\n");
        return;
    }

    // List all files found
    int count_files = 0;
    do {
        if (findFileData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY) {
            count_files++;
            printf("%d. %s\n", count_files, findFileData.cFileName); // Print file name
        }
    } while (FindNextFile(hFind, &findFileData) != 0);
    FindClose(hFind); // Close the file handle

    // Prompt user to choose a file to view
    printf("Enter the number of the file you want to view (0 to go back to the main menu): \n");
    int choice;
    do {
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); // Consume newline character left in the input buffer

        if (choice == 0) {
            system("cls"); // Clear the console screen
            main(); // Restart the main function
        } else if (choice >= 1 && choice <= count_files) {
            // Display the chosen file's content
            char filename[50];
            snprintf(filename, sizeof(filename), "lists\\%s", findFileData.cFileName);
            FILE *file = fopen(filename, "r");
            if (file == NULL) {
                printf("Error opening file!\n");
                exit(1);
            }

            // Read and print each line with priority-based coloring
            char buffer[256];
            int currentLine = 0;
            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            while (fgets(buffer, sizeof(buffer), file) != NULL) {
                // Remove newline character from the end of the buffer
                size_t len = strlen(buffer);
                if (len > 0 && buffer[len - 1] == '\n') {
                    buffer[len - 1] = '\0';
                }

                // Extract the priority character and remove it from display
                char *priorityPtr = strrchr(buffer, ':');
                char priority = ' ';
                if (priorityPtr != NULL && strlen(priorityPtr) == 2) {
                    priority = priorityPtr[1];
                    *priorityPtr = '\0';  // Remove priority from buffer
                }

                // Set console color based on priority
                if (priority == 'A') {
                    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
                } else if (priority == 'B') {
                    SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
                } else if (priority == 'C') {
                    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                } else {
                    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                }

                // Print the todo item
                printf("\t\t%d. %s", currentLine, buffer);
                currentLine++;
                // Only go to the next line after two todos
                if (currentLine % 2 == 0) {
                    printf("\n");
                } else {
                    printf("\t");
                }
                // Reset the console color
                SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            }
            printf("\n"); // Print a new line at the end
        }
    } while (choice != 0);
}
