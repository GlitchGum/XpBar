#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <fstream>

#define NOMINMAX
#include <windows.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

// --- Data Structures ---
struct Step {
    std::string description;
    bool is_complete = false;
};

struct Goal {
    std::string title;
    std::vector<Step> steps;
};

// --- Global Variables ---
std::vector<Goal> goals;
int total_steps = 0;
int completed_steps = 0;
int completed_goals = 0;
const std::string SAVE_FILE_NAME = "goals.txt";

// --- Function Declarations ---
void displayMenu();
void addGoal();
void viewGoals();
void completeStep();
void deleteGoal();
void drawExpBar();
void playCompletionSound();
void saveGoals();
void loadGoals();
void recalculateProgress();

// --- Main Application ---
int main() {
    loadGoals(); // Load existing goals at startup

    int choice = 0;
    while (true) {
        system("cls");
        std::cout << "=== Goal Tracker & To-Do List ===\n\n";

        std::cout << "Achievements: " << completed_goals << " Goals Completed | " << completed_steps << " Steps Completed\n";
        std::cout << "--------------------------------------------------------\n\n";

        drawExpBar();

        if (total_steps > 0 && completed_steps == total_steps) {
            std::cout << "\nCongratulations! All goals completed!\n";
            std::cout << " __   __  _______  __   __  _______  __   __    _    \n";
            std::cout << "|  | |  ||       ||  | |  ||       ||  | |  |  | |   \n";
            std::cout << "|  |_|  ||   _   ||  |_|  ||    ___||  |_|  |  | |   \n";
            std::cout << "|       ||  | |  ||       ||   |___ |       |  |_|   \n";
            std::cout << "|_     _||  |_|  ||       ||    ___||_     _|  (_)   \n";
            std::cout << "  |   |  |       | |     | |   |___   |   |          \n";
            std::cout << "  |___|  |_______|  |___|  |_______|  |___|          \n\n";

            playCompletionSound();
            std::cout << "Press Enter to reset and start new goals...";
            std::cin.get();

            goals.clear();
            recalculateProgress();
            saveGoals();
            continue;
        }

        displayMenu();
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            choice = 0;
        }
        else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

        switch (choice) {
        case 1: addGoal(); break;
        case 2: viewGoals(); break;
        case 3: completeStep(); break;
        case 4: deleteGoal(); break;
        case 5:
            std::cout << "Goodbye!\n";
            return 0;
        default:
            std::cout << "Invalid choice. Please try again.\n";
            std::cout << "Press Enter to continue...";
            std::cin.get();
        }
    }
    return 0;
}

// --- Function Definitions ---

void displayMenu() {
    std::cout << "\n--- Menu ---\n";
    std::cout << "1. Add a new goal\n";
    std::cout << "2. View goals and steps\n";
    std::cout << "3. Complete a step\n";
    std::cout << "4. Delete a goal\n";
    std::cout << "5. Exit\n";
    std::cout << "------------\n";
}

void addGoal() {
    // Stop any currently playing completion music
    PlaySound(NULL, 0, 0);

    Goal new_goal;
    std::cout << "\nEnter the title of your new goal: ";
    std::getline(std::cin, new_goal.title);

    int num_steps;
    std::cout << "How many steps does this goal have? ";
    std::cin >> num_steps;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    for (int i = 0; i < num_steps; ++i) {
        Step new_step;
        std::cout << "Enter description for step " << i + 1 << ": ";
        std::getline(std::cin, new_step.description);
        new_goal.steps.push_back(new_step);
    }

    goals.push_back(new_goal);
    recalculateProgress();
    saveGoals();
    std::cout << "\nGoal '" << new_goal.title << "' added and saved!\n";
    std::cout << "Press Enter to continue...";
    std::cin.get();
}

void viewGoals() {
    std::cout << "\n--- Your Goals ---\n";
    if (goals.empty()) {
        std::cout << "You have no goals yet. Add one from the menu!\n";
    }
    else {
        for (size_t i = 0; i < goals.size(); ++i) {
            std::cout << "\n" << i + 1 << ". " << goals[i].title << "\n";
            for (size_t j = 0; j < goals[i].steps.size(); ++j) {
                std::cout << "   - Step " << j + 1 << ": " << goals[i].steps[j].description;
                std::cout << (goals[i].steps[j].is_complete ? " [Completed]" : " [Pending]") << "\n";
            }
        }
    }
    std::cout << "\nPress Enter to continue...";
    std::cin.get();
}

void completeStep() {
    viewGoals();
    if (goals.empty()) return;

    int goal_num, step_num;
    std::cout << "\nEnter the goal number to update: ";
    std::cin >> goal_num;
    std::cout << "Enter the step number to complete: ";
    std::cin >> step_num;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    goal_num--;
    step_num--;

    if (goal_num >= 0 && goal_num < goals.size() && step_num >= 0 && step_num < goals[goal_num].steps.size()) {
        if (!goals[goal_num].steps[step_num].is_complete) {
            goals[goal_num].steps[step_num].is_complete = true;
            recalculateProgress();
            saveGoals();
            std::cout << "\nStep marked as complete! Keep it up!\n";
        }
        else {
            std::cout << "\nThat step is already complete!\n";
        }
    }
    else {
        std::cout << "\nInvalid goal or step number.\n";
    }
    std::cout << "Press Enter to continue...";
    std::cin.get();
}

void deleteGoal() {
    // Stop any currently playing completion music
    PlaySound(NULL, 0, 0);

    viewGoals();
    if (goals.empty()) return;

    int goal_num;
    std::cout << "\nEnter the goal number to DELETE: ";
    std::cin >> goal_num;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    goal_num--;

    if (goal_num >= 0 && goal_num < goals.size()) {
        goals.erase(goals.begin() + goal_num);
        recalculateProgress();
        saveGoals();
        std::cout << "\nGoal deleted successfully.\n";
    }
    else {
        std::cout << "\nInvalid goal number.\n";
    }
    std::cout << "Press Enter to continue...";
    std::cin.get();
}

void drawExpBar() {
    int bar_width = 50;
    float progress = (total_steps == 0) ? 0 : static_cast<float>(completed_steps) / total_steps;
    int pos = static_cast<int>(bar_width * progress);

    std::cout << "EXP: [";
    for (int i = 0; i < bar_width; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << static_cast<int>(progress * 100.0) << " %\n";
}

void playCompletionSound() {
    PlaySound(TEXT("victory.wav"), NULL, SND_FILENAME | SND_ASYNC);
}

void saveGoals() {
    std::ofstream outFile(SAVE_FILE_NAME);
    if (!outFile) {
        std::cerr << "Error: Could not open file for saving.\n";
        return;
    }

    for (const auto& goal : goals) {
        outFile << goal.title << '\n';
        outFile << goal.steps.size() << '\n';
        for (const auto& step : goal.steps) {
            outFile << step.description << '\n';
            outFile << step.is_complete << '\n';
        }
    }
}

void loadGoals() {
    std::ifstream inFile(SAVE_FILE_NAME);
    if (!inFile) return;

    std::string line;
    while (std::getline(inFile, line)) {
        Goal new_goal;
        new_goal.title = line;

        int num_steps;
        if (!(inFile >> num_steps)) break;
        inFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        for (int i = 0; i < num_steps; ++i) {
            Step new_step;
            if (!std::getline(inFile, new_step.description)) break;

            int complete_status;
            if (!(inFile >> complete_status)) break;
            inFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            new_step.is_complete = (complete_status == 1);

            new_goal.steps.push_back(new_step);
        }
        goals.push_back(new_goal);
    }
    recalculateProgress();
}

void recalculateProgress() {
    total_steps = 0;
    completed_steps = 0;
    completed_goals = 0;

    for (const auto& goal : goals) {
        int steps_in_this_goal = goal.steps.size();
        int completed_steps_in_this_goal = 0;
        total_steps += steps_in_this_goal;

        for (const auto& step : goal.steps) {
            if (step.is_complete) {
                completed_steps_in_this_goal++;
            }
        }
        completed_steps += completed_steps_in_this_goal;

        if (steps_in_this_goal > 0 && completed_steps_in_this_goal == steps_in_this_goal) {
            completed_goals++;
        }
    }
}