#include <string>
#include <windows.h>
#include <conio.h>
#include <iostream>
using namespace std;

void Welcome() {
    cout << " ____  ____  _____  _____  ____  ____  __  __" << endl;
    cout << "| ...|/ ...|| ... || ... || ...|/ ...||  ||  |" << endl;
    cout << "| |   | |__ | | | || |.| || |.. | |__ |  \\/  |" << endl;
    cout << "| |   \\._  || | | ||  __/ | ...| \\._ | \\.  ./" << endl;
    cout << "| |__. __| || |.| || |    | |... __| |  |  |" << endl;
    cout << "|____||____||_____||_|    |____||____|  |__|" << endl;

    HANDLE hc = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hc, 2);

    cout << "Welcome to CSOPESY Emulator!" << endl;
    cout << "This is for CSOPESY Groupwork #1" << endl;

    SetConsoleTextAttribute(hc, 14);
    cout << "Type 'exit' to quit, 'clear' to clear the screen" << endl;
    SetConsoleTextAttribute(hc, 15);
}

void Clear() {
    system("cls");
    Welcome();
}

void ListProcesses() {
    cout << "screen -ls command recognized. Doing something." << endl;
}

void ProcessSmi() {
    cout << "process-smi command recognized. Doing something." << endl;
}

void AddProcess() {
    Clear(); // clear everything since you are inside a process
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
    Clear(); // clear everything since you are accessing/retrieving a process
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
    cout << "screen command recognized. Doing something." << endl;
}

int main() {
    /* Welcome message for CSOPESY Emulator main menu console */ 
    Welcome();
    
    /* Loop for running the program */
    bool running = true;
    while(running) {
        string command;
        cout << "Enter command: ";
        getline(cin, command);

        if (command == "initialize") {
            Initialize();
        } else if (command =="screen") {
            Screen();
        /* For next groupwork
        } else if (command == "screen -ls") {
            ListProcesses();
        } else if (command == "screen -s") {
            AddProcess();
        } else if (command == "screen -r") {
            UpdateProcess();*/
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