#include <string>
#include <windows.h>
#include <iostream>
#include <map>
#include <ctime>
using namespace std;

/* ========== HELPER FUNCTIONS ========== */
void Welcome() {
    cout << " ____  ____  _____  _____  ____  ____  __  __" << endl;
    cout << "| ...|/ ...|| ... || ... || ...|/ ...||  ||  |" << endl;
    cout << "| |   | |__ | | | || |.| || |.. | |__ |  \\/  |" << endl;
    cout << "| |   \\._  || | | ||  __/ | ...| \\._ | \\.  ./" << endl;
    cout << "| |__. __| || |.| || |    | |... __| |  |  |" << endl;
    cout << "|____||____||_____||_|    |____||____|  |__|" << endl;

    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(handle, 2); // Green
    cout << "Welcome to CSOPESY Emulator!" << endl;
    cout << "This is for CSOPESY Groupwork #2" << endl;

    SetConsoleTextAttribute(handle, 14); // Yellow
    cout << "Type 'exit' to quit, 'clear' to clear the screen" << endl;

    SetConsoleTextAttribute(handle, 15); // White
}

string GetCurrentTimestamp() {
    time_t now = time(0);
    tm ltm; // use a local object instead of pointer
    localtime_s(&ltm, &now); // safer version of localtime

    char buffer[50];
    strftime(buffer, sizeof(buffer), "%m/%d/%Y, %I:%M:%S %p", &ltm);
    return string(buffer);
}

void Clear() {
    system("cls");
    Welcome();
}

/* ========== CONSOLE CLASS ========== */
class Console {
private:
    map<string, string> screens;

    void DrawScreen(const string& name) {
        Clear();
        cout << "[Screen for: " << name << "]" << endl;
        cout << "Process Name: " << name << endl;
        cout << "Instruction Line: 532 / 1240" << endl;
        cout << "Created At: " << screens[name] << endl;
        cout << "-------------------------------" << endl;
    }

    void ProcessSmi() {
        cout << "process-smi command recognized. Displaying dummy stats..." << endl;
        cout << "[CPU Usage: 34%] [Memory Usage: 128MB] [Disk I/O: 2.4MB/s]" << endl;
    }

    void ScreenSession(const string& name) {
        DrawScreen(name);
        string input;

        while (true) {
            cout << "Enter command: ";
            getline(cin, input);

            if (input == "process-smi") {
                ProcessSmi();
            }
            else if (input == "clear") {
                DrawScreen(name);
            }
            else if (input == "exit") {
                Clear();
                return;
            }
            else {
                cout << "Unknown command. Try again." << endl;
            }
        }
    }

    bool ScreenExists(const string& name) {
        return screens.find(name) != screens.end();
    }

public:
    void ListScreens() {
        cout << "Existing Screens:" << endl;
        if (screens.empty()) {
            cout << "  [none yet]" << endl;
        }
        else {
            for (auto& [name, ts] : screens) {
                cout << "  " << name << " (Created: " << ts << ")" << endl;
            }
        }
    }

    void AddScreen(const string& name) {
        if (ScreenExists(name)) {
            cout << "Screen '" << name << "' already exists." << endl;
        }
        else {
            screens[name] = GetCurrentTimestamp();
            ScreenSession(name);
        }
    }

    void ResumeScreen(const string& name) {
        if (!ScreenExists(name)) {
            cout << "Screen '" << name << "' does not exist. Use screen -s to create it." << endl;
        }
        else {
            ScreenSession(name);
        }
    }
};

/* ========== OTHER EMULATOR FUNCTIONS ========== */
void Initialize() { cout << "initialize command recognized.\n"; }
void SchedulerStart() { cout << "scheduler-start command recognized.\n"; }
void SchedulerStop() { cout << "scheduler-stop command recognized.\n"; }
void ReportUtil() { cout << "report-util command recognized.\n"; }
void Screen() { cout << "screen command recognized.\n"; }

/* ========== MAIN ========== */
int main() {
    Welcome();
    Console console;
    bool running = true;

    while (running) {
        string command;
        cout << "Enter command: ";
        getline(cin, command);

        if (command == "initialize") Initialize();
        else if (command == "screen") Screen();
        else if (command == "screen -ls") console.ListScreens();
        else if (command.rfind("screen -s ", 0) == 0)
            console.AddScreen(command.substr(10));
        else if (command.rfind("screen -r ", 0) == 0)
            console.ResumeScreen(command.substr(10));
        else if (command == "scheduler-start") SchedulerStart();
        else if (command == "scheduler-stop") SchedulerStop();
        else if (command == "report-util") ReportUtil();
        else if (command == "clear") Clear();
        else if (command == "exit") running = false;
        else cout << "Unknown command. Please try again." << endl;
    }

    return 0;
}
