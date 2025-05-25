#include <string>
#include <windows.h>
#include <iostream>
using namespace std;

void Welcome() {
    cout << " ____  ____  _____  _____  ____  ____  __  __" << endl;
    cout << "| ...|/ ...|| ... || ... || ...|/ ...||  ||  |" << endl;
    cout << "| |   | |__ | | | || |.| || |.. | |__ |  \\/  |" << endl;
    cout << "| |   \\._  || | | ||  __/ | ...| \\._ | \\.  ./" << endl;
    cout << "| |__. __| || |.| || |    | |... __| |  |  |" << endl;
    cout << "|____||____||_____||_|    |____||____|  |__|" << endl;

    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(handle, 2); //Green text

    cout << "Welcome to CSOPESY Emulator!" << endl;
    cout << "This is for CSOPESY Groupwork #2" << endl;

    SetConsoleTextAttribute(handle, 14); //Yellow text
    cout << "Type 'exit' to quit, 'clear' to clear the screen" << endl;
    SetConsoleTextAttribute(handle, 15); //White text
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

void AddProcess(string& process_name) {
    /* clear everything since you are adding and inside a process */
    system("cls"); // changed it instead of using Clear() since this is a new console layout
    
    /**
     * Change this line to follow the specs in #3:
       - Process name
       - Current line of instruction / Total line of instruction.
       - Timestamp of when the screen is created in (MM/DD/YYYY, HH:MM:SS AM/PM) format
     * Optional now: We could also include here to check if the process exists
     * and if it does not, we can return an error message. 
     * */
    cout << "screen -s command recognized. Doing something." << endl;
    
    bool add_run = true;
    while(add_run) {
        string command;
        cout << "Enter command: ";
        getline(cin, command);

        if (command == "process-smi") {
            ProcessSmi();
        } else if (command == "clear") {
            system("cls"); // changed it instead of using Clear() since this is a new console layout
        } else if (command == "exit") {
            add_run = false;
            Clear();
            return;
        } else {
            cout << "Unknown command. Please try again." << endl;
        }
    }
}

void UpdateProcess(string& process_name) {
    /* clear everything since you are accessing/retrieving a process */
    system("cls"); // changed it instead of using Clear() since this is a new console layout
    
    /**
     * Change this line to follow the specs in #3:
       - Process name
       - Current line of instruction / Total line of instruction.
       - Timestamp of when the screen is created in (MM/DD/YYYY, HH:MM:SS AM/PM) format
     * Optional now: We could also include here to check if the process exists
     * and if it does not, we can return an error message. 
     * */
    cout << "screen -r command recognized. Doing something." << endl;
    
    bool update_run = true;
    while(update_run) {
        string command;
        cout << "Enter command: ";
        getline(cin, command);

        if (command == "process-smi") {
            ProcessSmi();
        } else if (command == "clear") {
            system("cls"); // changed it instead of using Clear() since this is a new console layout
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
        } else if (command == "screen -ls") {
            ListProcesses();
        } else if (command.rfind("screen -s ", 0) == 0) {
            string process_name = command.substr(10); 
            if (!process_name.empty()) {
                AddProcess(process_name);
            } else {
                cout << "Error: No process name provided." << endl;
            }
        } else if (command.rfind("screen -r ", 0) == 0) {
            string process_name = command.substr(10);
            if (!process_name.empty()) {
                UpdateProcess(process_name);
            } else {
                cout << "Error: No process name provided." << endl;
            }
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