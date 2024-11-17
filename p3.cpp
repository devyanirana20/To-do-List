#include <graphics.h>
#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include<math.h>
#include<dos.h>
#include<ctime>
#include<cstring>
using namespace std;



#define MAX_TASKS 10
#define MAX_TASK_LEN 50
#define MAX_DUE_DATE_LEN 11 // YYYY-MM-DD format
#define FILENAME "tasks.txt" // File to save tasks

#include <iostream>
#include <ctime>
#include <string>
#include <cstring> // For strcpy and strlen

using namespace std;

char* current_date() {
    time_t t = time(nullptr);
    tm* now = localtime(&t);

    if (now == nullptr) {
        cerr << "Failed to get local time." << endl;
        return nullptr; // Return null pointer in case of error
    }

    // Formatting with zero-padding for months and days
    string y = to_string(now->tm_year + 1900);
    string m = now->tm_mon + 1 < 10 ? "0" + to_string(now->tm_mon + 1) : to_string(now->tm_mon + 1);
    string d = now->tm_mday < 10 ? "0" + to_string(now->tm_mday) : to_string(now->tm_mday);

    // Concatenate date in YYYY-MM-DD format
    string date = y + "-" + m + "-" + d;

    // Allocate memory for char* and copy the string content
    char* c_date = new char[date.length() + 1]; // +1 for the null terminator
    strcpy(c_date, date.c_str());

    return c_date; // Caller must free this memory
}



typedef struct {
    char name[MAX_TASK_LEN];
    char dueDate[MAX_DUE_DATE_LEN]; // Due date
    int completed; // Completed status
    
} Task;

Task tasks[MAX_TASK_LEN];
int taskCount = 0;


// Function prototypes
void drawInterface();
void displayTasks();
void addTask(const char *taskName, const char *dueDate);
void removeTask(int index);
void editTask(int index, const char *newTaskName, const char *newDueDate);
void loadTasks();
void saveTasks();
void markTaskCompleted(int index);
void getInput(char *inputBuffer, int maxLength);
void sortTasks();
void clearScreen();
void displayStatistics(); // New function for displaying statistics

void drawSmiley(int x, int y, int color) {
    setcolor(color);
    setfillstyle(SOLID_FILL, color);
    
    // Draw face
    fillellipse(x, y, 30, 30); // Main face

    // Draw eyes
    setfillstyle(SOLID_FILL, BLACK);
    fillellipse(x - 10, y - 10, 5, 5); // Left eye
    fillellipse(x + 10, y - 10, 5, 5); // Right eye

    // Draw smile with an arc
    setcolor(BLACK);
    arc(x, y + 10, 210, 330, 15); // Smile curve
    //line(x - 10, y + 10, x + 10, y + 10); // Connect the ends of the arc to complete the smile
}


void welcomeAnimation() {
    int midX = getmaxx() / 2;
    int midY = getmaxy() / 2;
    int color = 1;
    int angle = 0;
    char message[] = "Welcome to To-Do List";

    for (int i = 0; i < 50; i++) {
        // Clear previous frame
        cleardevice();

        // Display Welcome Message
        setcolor(color % 15 + 1);
        settextstyle(3, HORIZ_DIR, 4);
        outtextxy(midX - textwidth(message) / 2, midY - textheight(message) / 2, message);

        // Calculate floating positions for smileys
        int smiley1X = midX + 100 * cos(angle * 3.14 / 180); // Circular path
        int smiley1Y = midY + 100 * sin(angle * 3.14 / 180);

        int smiley2X = midX + 100 * cos((angle + 180) * 3.14 / 180); // Opposite circular path
        int smiley2Y = midY + 100 * sin((angle + 180) * 3.14 / 180);

        // Draw rotating smiley faces
        drawSmiley(smiley1X, smiley1Y, YELLOW);
        drawSmiley(smiley2X, smiley2Y, YELLOW);

        // Update angle and color for next frame
        angle = (angle + 5) % 360;
        color++;

        delay(30); // Delay for smooth animation
    }
    
    cleardevice();
   
    
   
     // Pause before clearing
     // Clear screen for main interface
}
// Draw the main user interface
void drawInterface() {
    setbkcolor(WHITE); // Set background to white
    cleardevice();
    
    setcolor(BLACK); // Set text color to black
    rectangle(10, 10, 630, 470);
    
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 3);
    outtextxy(220, 20, "TO-DO LIST");
    
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    line(10, 60, 630, 60);
    outtextxy(30, 70, "Tasks:");

    // Button dimensions and positions
    int buttonWidth = 150;
    int buttonHeight = 50; // Increased height for better visibility
    int buttonGap = 20;

    // Adjusted Y position to move buttons upward
    int buttonY = 360; // New Y position for buttons

    // Button colors (3D effect)
    setfillstyle(SOLID_FILL, LIGHTGRAY);
    
    // Draw buttons
    rectangle(30, buttonY, 30 + buttonWidth, buttonY + buttonHeight); // Add Task
    floodfill(105, buttonY + 20, BLACK);
    
    rectangle(30 + buttonWidth + buttonGap, buttonY, 30 + 2 * buttonWidth + buttonGap, buttonY + buttonHeight); // Task Completed
    floodfill(210, buttonY + 20, BLACK);
    
    setfillstyle(SOLID_FILL, LIGHTGRAY);
    rectangle(30 + 2 * buttonWidth + 2 * buttonGap, buttonY, 30 + 3 * buttonWidth + 2 * buttonGap, buttonY + buttonHeight); // Remove Task
    floodfill(380, 380, BLACK);

    
    rectangle(530, 360, 610, 410); // Button rectangle based on click logic
    setfillstyle(SOLID_FILL, RED);
    floodfill(550, 395, BLACK);    // Fill inside the button   // Exit Button
                // Set text color

    setfillstyle(SOLID_FILL, LIGHTGRAY);
    

    // rectangle(30, buttonY + buttonHeight + buttonGap, 30 + buttonWidth, buttonY + 2 * buttonHeight + buttonGap); // Edit Task
    // floodfill(105, buttonY + buttonHeight + 20, BLACK);
    rectangle(90, 420, 260, 460);
    floodfill(150, 435, BLACK);

    // rectangle(30 + buttonWidth + buttonGap, buttonY + buttonHeight + buttonGap, 30 + 2 * buttonWidth + buttonGap, buttonY + 2 * buttonHeight + buttonGap); // Sort Tasks
    // floodfill(210, buttonY + buttonHeight + 20, BLACK);
   rectangle(300,420, 450, 460);
    floodfill(350, 435, BLACK);

    rectangle(490,420, 605, 460);
    floodfill(510, 440, BLACK);    //Edit Button


     //rectangle(100, 400, 300, 460); // Updated dimensions for the Statistics button
     //floodfill(150, 435, BLACK);
     // rectangle(100, 420, 250, 440); // Statistics button
        

    // Set text color (Red)
    setcolor(RED);

    // Centering the text for each button
    int addTaskTextX = 40 + (buttonWidth - textwidth("Add")) / 2;
    outtextxy(addTaskTextX, buttonY + 15, "Add");

    int taskCompletedTextX = (30 + buttonWidth + buttonGap) + (buttonWidth - textwidth("Completed")) / 2;
    outtextxy(taskCompletedTextX, buttonY + 15, "Completed");

    int removeTaskTextX = (30 + 2 * buttonWidth + 2 * buttonGap) + (buttonWidth - textwidth("Remove")) / 2;
    outtextxy(removeTaskTextX, buttonY + 15, "Remove");

    // int editTaskTextX = 30 + (buttonWidth - textwidth("Edit Task")) / 2;
    // outtextxy(editTaskTextX, buttonY + buttonHeight + 65, "Edit Task");
    

    outtextxy(500, 430, "Edit");

    // int sortTasksTextX = (30 + buttonWidth + buttonGap) + (buttonWidth - textwidth("Sort Tasks")) / 2;
    // outtextxy(sortTasksTextX, buttonY + buttonHeight + 65, "Sort Tasks");
    outtextxy(340, 430, "Sort");
    // Instructions
    // setcolor(BLACK);
    // outtextxy(30, 310, "Instructions: Click buttons to interact");
    // outtextxy(60, 330, " with the tasks.");

    // Position for the "Exit" text (centered within the button)
    int x = 540;        // X-coordinate for the text (approximately centered horizontally)
    int y = 380;   
     
        // Starting Y-coordinate of the button
    outtextxy(x,y, "Exit"); // Text with 15px padding below top edge

setcolor(RED);
outtextxy(100, 430, "Stastistics");

}

// Display the list of tasks
void displayTasks() {
    int y = 100;
    setcolor(BLACK);
    
    for (int i = 0; i < taskCount; i++) {
        char buffer[100];
                sprintf(buffer, "%d. %s (Due: %s)", i + 1, tasks[i].name, tasks[i].dueDate);
        
        outtextxy(30, y, buffer);
        if(tasks[i].completed ==1){
            setcolor(RED);
            line(10,y+7,450,y+7);
            setcolor(BLACK);
        }
        y += 30;}
    }


// Add a new task to the list
void addTask(const char *taskName, const char *dueDate) {
       
    char* date=current_date();

    if(strcmp(date,dueDate) <0){
        outtextxy(30, 320, "Invalid due Date !"); 
    }
    else{

    if (taskCount < MAX_TASKS) {
        strcpy(tasks[taskCount].name, taskName);
        strcpy(tasks[taskCount].dueDate, dueDate);
        tasks[taskCount].completed = 0; // Mark as not completed
        taskCount++;
        outtextxy(30, 320, "Task added successfully!"); // Confirmation message
        
    } else {
        outtextxy(30, 320, "Task list full!");
    }}

}

// Remove a task from the list
void removeTask(int index) {
   
    if (index >= 0 && index < taskCount) {
        for (int i = index; i < taskCount - 1; i++) {
            tasks[i] = tasks[i + 1]; // Shift tasks
        }
        taskCount--;
        outtextxy(30, 320, "Task removed successfully!"); // Confirmation message
    } else {
        outtextxy(30, 320, "Invalid task number!");
    }
}

// Edit an existing task
void editTask(int index, const char *newTaskName, const char *newDueDate) {
    if (index >= 0 && index < taskCount) {
        strcpy(tasks[index].name, newTaskName);
        strcpy(tasks[index].dueDate, newDueDate);
        outtextxy(30, 320, "Task edited successfully!"); // Confirmation message
    } else {
        outtextxy(30, 320, "Invalid task number!");
    }
}
// Load tasks from a file
// Save tasks to a file
void loadTasks() {
    FILE *file = fopen(FILENAME, "r");
    if (file) {
        char buffer[MAX_TASK_LEN + MAX_DUE_DATE_LEN + 2]; // Buffer to read a line
        
        while (taskCount < MAX_TASKS && fgets(buffer, sizeof(buffer), file)) {
            // Trim leading and trailing whitespace
            char *line = buffer;
            while (*line == ' ' || *line == '\t') line++; // Skip leading spaces/tabs
            
            // Skip empty lines or lines with only whitespace
            if (*line == '\0' || *line == '\n') {
                continue;
            }

            // Find the comma separator
            char *commaPos = strchr(line, ',');
            if (commaPos) {
                // Extract task name
                size_t nameLen = commaPos - line;
                strncpy(tasks[taskCount].name, line, nameLen);
                tasks[taskCount].name[nameLen] = '\0'; // Null-terminate the name

                // Extract due date
                char *dueDate = commaPos + 1; // Move to the character after the comma
                while (*dueDate == ' ' || *dueDate == '\t') dueDate++; // Skip spaces/tabs
                strncpy(tasks[taskCount].dueDate, dueDate, MAX_DUE_DATE_LEN - 1);
                tasks[taskCount].dueDate[MAX_DUE_DATE_LEN - 1] = '\0'; // Null-terminate the due date
                tasks[taskCount].dueDate[strcspn(tasks[taskCount].dueDate, "\n")] = '\0'; // Remove newline

                tasks[taskCount].completed = 0; // Mark as not completed
                taskCount++;
            }
        }
        fclose(file);
    }
}



// Save tasks to a file
void saveTasks() {
    FILE *file = fopen(FILENAME, "w");
    if (file) {
        for (int i = 0; i < taskCount; i++) {
            fprintf(file, "%s,%s\n", tasks[i].name, tasks[i].dueDate); // Save task and due date
        }
        fclose(file);
    }
}



// Mark a task as completed
void markTaskCompleted(int index) {
    if (index >= 0 && index < taskCount) {
        tasks[index].completed = 1; // Mark as completed
        outtextxy(30, 320, "Task marked as completed!"); // Confirmation message
    } else {
        outtextxy(30, 320, "Invalid task number!");
    }
}

// Get input from the user
void getInput(char *inputBuffer, int maxLength) {
    int i = 0;
    char ch;
    while ((ch = getch()) != 13) { // Enter key
        if (ch == 8) { // Backspace
            if (i > 0) {
                i--;
                inputBuffer[i] = '\0';
                outtextxy(360, 280, "                           "); // Clear previous text
                outtextxy(360, 280, inputBuffer);
            }
        } else if (i < maxLength - 1) {
            inputBuffer[i] = ch;
            inputBuffer[i + 1] = '\0';
            outtextxy(360,280, inputBuffer);
            i++;
        }
    }
}
void getInput1(char *inputBuffer, int maxLength) {
    int i = 0;
    char ch;
    while ((ch = getch()) != 13) { // Enter key
        if (ch == 8) { // Backspace
            if (i > 0) {
                i--;
                inputBuffer[i] = '\0';
                outtextxy(420,300, "                           "); // Clear previous text
                outtextxy(420,300, inputBuffer);
            }
        } else if (i < maxLength - 1) {
            inputBuffer[i] = ch;
            inputBuffer[i + 1] = '\0';
            outtextxy(420,300, inputBuffer);
            i++;
        }
    }
}
void clearScreen() {
    cleardevice();
    drawInterface();
    displayTasks();
}
// Sort tasks alphabetically
// Sort tasks alphabetically
void sortTasks() {
    for (int i = 0; i < taskCount - 1; i++) {
        for (int j = i + 1; j < taskCount; j++) {
            if (strcmp(tasks[i].dueDate, tasks[j].dueDate) > 0) {
                // Swap tasks
                Task temp = tasks[i];
                tasks[i] = tasks[j];
                tasks[j] = temp;
            }
            else if(strcmp(tasks[i].dueDate, tasks[j].dueDate)== 0){
                if (strcmp(tasks[i].name, tasks[j].name) > 0) {
                // Swap tasks
                Task temp = tasks[i];
                tasks[i] = tasks[j];
                tasks[j] = temp;
            }}
        }
    }
    clearScreen();   // Clear the screen and redraw the interface after sorting
    displayTasks();  // Ensure tasks are displayed in the new order
}


// Clear the screen and redraw the interface


// Display graphical statistics of tasks
// Display graphical statistics of tasks
void displayStatistics() {
    int completedCount = 0;
    int pendingCount = 0;

    // Calculate completed and pending tasks
    for (int i = 0; i < taskCount; i++) {
        if (tasks[i].completed) {
            completedCount++;
        } else {
            pendingCount++;
        }
    }

    int totalTasks = completedCount + pendingCount;

    if (totalTasks == 0) {
        printf("No tasks to display statistics for.\n");
        return;
    }

    // Calculate percentages and angles
    float completedPercentage = (float)completedCount / totalTasks * 100;
    float pendingPercentage = (float)pendingCount / totalTasks * 100;
    int completedAngle = (int)(360.0 * completedCount / totalTasks);

    // Initialize graphics
    int gd = DETECT, gm;
    initgraph(&gd, &gm, NULL);

    // Draw the pie chart
    cleardevice();
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 3);
    outtextxy(200, 20, "Statistics");

    setcolor(GREEN);
    setfillstyle(SOLID_FILL, GREEN);
    pieslice(300, 200, 0, completedAngle, 100);

    setcolor(RED);
    setfillstyle(SOLID_FILL, RED);
    pieslice(300, 200, completedAngle, 360, 100);

    // Display percentages
    char completedText[50];
    char pendingText[50];
    sprintf(completedText, "Completed: %.2f%%", completedPercentage);
    sprintf(pendingText, "Pending: %.2f%%", pendingPercentage);

    setcolor(WHITE);
    outtextxy(50, 350, completedText);
    outtextxy(50, 400, pendingText);
    
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2); // Font: Default, Direction: Horizontal, Size: 1 (smallest size)

    outtextxy(30, 450, "Click anywhere to continue...");
    while (!ismouseclick(WM_LBUTTONDOWN)) {
        delay(100);
    }

    
}
void loading(){
    setcolor(WHITE);
    rectangle(150 ,200,450,230);
for(int i =0 ; i<300 ; i++){
line(150+i,200,150+i,230);
outtextxy(260,250,"Loading...");
delay(10);
}
cleardevice();
}

int main() {
    int gd = DETECT, gm;
    initgraph(&gd, &gm, "C:\\TURBOC3\\BGI");
    
    welcomeAnimation();
    loading();
    
    
    loadTasks(); // Load tasks when the application starts
    drawInterface();
    displayTasks();

    while (1) {
        if (ismouseclick(WM_LBUTTONDOWN)) {
            int x, y;
            getmouseclick(WM_LBUTTONDOWN, x, y);

            if (x >= 30 && x <= 30 + 150 && y >= 390 && y <= 390 + 50) { // Add Task
                char newTask[MAX_TASK_LEN] = {0};
                char dueDate[MAX_DUE_DATE_LEN] = {0};
                clearScreen();
                outtextxy(150,280, "Enter new task: ");
                getInput(newTask, MAX_TASK_LEN);
                outtextxy(20, 300, "Enter due date (YYYY-MM-DD): ");
                getInput1(dueDate, MAX_DUE_DATE_LEN);
                addTask(newTask, dueDate);
            } else if (x >= 30 + 150 + 10 && x <= 30 + 2 * 150 + 10 && y >= 390 && y <= 390 + 50) { // Task Completed
                char taskNum[3] = {0};
                clearScreen();
                outtextxy(10, 300, "Enter task no : ");
                getInput1(taskNum, 3);
                int index = atoi(taskNum) - 1;
                markTaskCompleted(index);
            } else if (x >= 30 + 2 * 150 + 2 * 10 && x <= 30 + 3 * 150 + 2 * 10 && y >= 390 && y <= 390 + 50) { // Remove Task
                char taskNum[3] = {0};
                clearScreen();
                outtextxy(30,300, "Enter task number: ");
                getInput1(taskNum, 3);
                int index = atoi(taskNum) - 1;
                removeTask(index);
            } else if (x >= 480 && x <= 580 && y >= 430 && y <=450) { // Edit Task
                char taskNum[3] = {0};
                char newTask[MAX_TASK_LEN] = {0};
                char newDueDate[MAX_DUE_DATE_LEN] = {0};
                clearScreen();
                outtextxy(10, 280,"Enter task no. to edit: ");
                
                getInput(taskNum, 3);
                outtextxy(10,280,"                               ");
                int index = atoi(taskNum) - 1;
                if (index >= 0 && index < taskCount) {
                    outtextxy(80, 280, "Enter new task: ");
                    getInput(newTask, MAX_TASK_LEN);
                    outtextxy(20, 300, "Enter new due date: ");
                    getInput1(newDueDate, MAX_DUE_DATE_LEN);
                    editTask(index, newTask, newDueDate);
                }
                else {
                    outtextxy(150, 300, "Invalid task number!");
                }
            }
            else if (x >= 100 && x <= 250 && y >= 420 && y <= 460) { // View Statistics
    displayStatistics();
}
else if (x >= 320 && x <= 450 && y >= 420 && y <= 460) { // Sort Task button
    sortTasks();  // Sort the tasks and update the screen
}

      
    else if (x >= 500 && x<= 590 &&
        y >=360 && y <= 400) {
        outtextxy(200, 200, "Exiting application...");
        delay(2000); // Wait 2 seconds
        exit(0);     // Exit the application
    }
          clearScreen();
        }
        saveTasks(); // Save tasks on each iteration (for simplicity)
    }

    closegraph();
    return 0;
}