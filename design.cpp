#include <iostream>
using namespace std;

void Clear() {
    system("cls");
}

void Welcome() {
    cout << "Welcome to CSOPESY Emulator!" << endl  
         << "This is for CSOPESY Seatwork #1" << endl;
}

void ListProcesses() {
    cout << "screen -ls command recognized. Doing something." << endl;
}

void ProcessSmi() {
    cout << "process-smi command recognized. Doing something." << endl;
}

void AddProcess() {
    cout << "screen -s command recognized. Doing something." << endl;
    
    bool add_run = true;
    while(add_run) {
        string command;
        cout << "Enter command: ";
        getline(cin, command);

        if (command == "process-smi") {
            ProcessSmi();
        } else if (command == "clear") {
            Clear();
        } else if (command == "exit") {
            add_run = false;
            Clear();
            return;
        } else {
            cout << "Unknown command. Please try again." << endl;
        }
    }
}

void UpdateProcess() {
    cout << "screen -r command recognized. Doing something." << endl;
    
    bool update_run = true;
    while(update_run) {
        string command;
        cout << "Enter command: ";
        getline(cin, command);

        if (command == "process-smi") {
            ProcessSmi();
        } else if (command == "clear") {
            Clear();
        } else if (command == "exit") {
            update_run = false;
            Clear();
            return;
        } else {
            cout << "Unknown command. Please try again." << endl;
        }
    }
}

void Initialize() {
    cout << "initialize command recognized. Doing something." << endl;
}

void SchedulerStart() {
    cout << "scheduler-start command recognized. Doing something." << endl;
}

void SchedulerStop() {
    cout << "scheduler-stop command recognized. Doing something." << endl;
}

void ReportUtil() {
    cout << "report-util command recognized. Doing something." << endl;
}

void Screen() {
    /* Screen commands */
    cout << "screen command recognized." << endl;
    
    bool screen_run = true;
    while(screen_run) {
        string command;
        cout << "Enter command: ";
        getline(cin, command);

        if (command == "screen -ls") {
            ListProcesses();
        } else if (command == "screen -s") {
            AddProcess();
        } else if (command == "screen -r") {
            UpdateProcess();
        } else if (command == "process-smi") {
            ProcessSmi();
        } else if (command == "clear") {
            Clear();
        } else if (command == "exit") {
            screen_run = false;
            Clear();
            Welcome();
            return;
        } else {
            cout << "Unknown command. Please try again." << endl;
        }
    }
}

int main() {
    /* Welcome message for CSOPESY Emulator main menu console */ 
    Welcome();
    
    /* Loop for running the program */
    bool running = true;
    while(running) {
        string command;
        cout << "Enter command: ";
        cin >> command;
        cin.ignore(); // Clear the input buffer to avoid issues with getline (forgot the problem i encountered kaya ganito s'ya naka-implement)

        if (command == "initialize") {
            Initialize();
        } else if (command == "screen") {
            Screen();
        } else if (command == "scheduler-start") {
            SchedulerStart();
        } else if (command == "scheduler-stop") {
            SchedulerStop();
        } else if (command == "report-util") {
            ReportUtil();
        } else if (command == "clear") {
            Clear();
        } else if (command == "exit") {
            running = false;
        } else {
            cout << "Unknown command. Please try again." << endl;
        }
    }

    return 0;
}
