#include <string>
#include <windows.h>
#include <iostream>
#include <map>
#include <ctime>

using namespace std;

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
    void Initialize() {
        if (initialized) {
            cout << "Already initialized." << endl;
            return;
        }
        
        scheduler.loadConfig();
        scheduler.startScheduler();
        initialized = true;
        cout << "System initialized successfully." << endl;
    }
    
    bool IsInitialized() { return initialized; }

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