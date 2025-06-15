#include <string>
#include <windows.h>
#include <iostream>
#include <map>
#include <ctime>
#include <thread>
#include <mutex>
#include <queue>
#include <fstream>
#include <atomic>
#include <chrono>
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
    cout << "This is for CSOPESY Groupwork #3" << endl;

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

/* ========== SCHEDULER & PROCESS CONTROL ========== */

struct Process {
    string name;
    string created_at;
    int remaining_prints;
};

// Global queue for FCFS
queue<Process> readyQueue;
mutex queueMutex;

// Scheduler and worker thread management
const int NUM_CORES = 4;
atomic<bool> schedulerRunning(false);
atomic<bool> workersRunning(true);
vector<thread> cpuCores;
thread schedulerThread;

// Tracking process execution status
vector<string> finishedScreens;
map<string, pair<int, int>> progressMap;
map<string, int> processCoreMap;

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
        HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);

        cout << "----------------------------------------------" << endl;
        cout << "Running processes:" << endl;

        for (const auto& [name, ts] : screens) {
            if (find(finishedScreens.begin(), finishedScreens.end(), name) == finishedScreens.end()) {
                int printsDone = progressMap.count(name) ? progressMap[name].first : 0;
                int totalPrints = progressMap.count(name) ? progressMap[name].second : 100;
                int coreID = processCoreMap.count(name) ? processCoreMap[name] : -1;

                SetConsoleTextAttribute(handle, 15);
                cout << name << "  (" << ts << ")  ";
                SetConsoleTextAttribute(handle, 11);
                cout << "Core: " << coreID << "   ";
                SetConsoleTextAttribute(handle, 6);
                cout << printsDone << " / " << totalPrints << endl;
            }
        }

        cout << "\nFinished processes:" << endl;

        for (const string& name : finishedScreens) {
            string ts = screens[name];

            SetConsoleTextAttribute(handle, 15);
            cout << name << "  (" << ts << ")  ";
            SetConsoleTextAttribute(handle, 10);
            cout << "Finished   ";
            SetConsoleTextAttribute(handle, 6);
            cout << "100 / 100" << endl;
        }

        SetConsoleTextAttribute(handle, 15);
        cout << "----------------------------------------------" << endl;
    }

    void AddScreen(const string& name) {
        if (ScreenExists(name)) {
            cout << "Screen '" << name << "' already exists." << endl;
            return;
        }

        string timestamp = GetCurrentTimestamp();
        screens[name] = timestamp;

        Process proc = { name, timestamp, 100 };

        {
            lock_guard<mutex> lock(queueMutex);
            readyQueue.push(proc);
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
void ReportUtil() { cout << "report-util command recognized.\n"; }
void Screen() { cout << "screen command recognized.\n"; }

void WorkerThread(int coreID) {
    while (workersRunning) {
        Process currentProcess;

        {
            lock_guard<mutex> lock(queueMutex);
            if (!readyQueue.empty()) {
                currentProcess = readyQueue.front();
                readyQueue.pop();
            }
            else {
                this_thread::sleep_for(chrono::milliseconds(100));
                continue;
            }
        }

        string filename = currentProcess.name + ".txt";
        ofstream file(filename, ios::app);
        if (!file.is_open()) {
            cerr << "Failed to open file for " << currentProcess.name << endl;
            continue;
        }

        file << "Process name: " << currentProcess.name << endl;
        file << "Logs:" << endl;

        processCoreMap[currentProcess.name] = coreID;
        progressMap[currentProcess.name] = { 0, currentProcess.remaining_prints };

        for (int i = 0; i < currentProcess.remaining_prints; ++i) {
            time_t now = time(0);
            tm ltm;
            localtime_s(&ltm, &now);

            char buffer[50];
            strftime(buffer, sizeof(buffer), "(%m/%d/%Y %I:%M:%S%p)", &ltm);

            file << buffer << " Core:" << coreID << " \"Hello world from " << currentProcess.name << "!\"" << endl;

            // Track progress
            {
                lock_guard<mutex> lock(queueMutex);
                progressMap[currentProcess.name].first++;
            }

            this_thread::sleep_for(chrono::milliseconds(50));
        }

        file.close();

        {
            lock_guard<mutex> lock(queueMutex);
            finishedScreens.push_back(currentProcess.name);
            processCoreMap.erase(currentProcess.name);
        }
    }
}

void SchedulerStart() {
    if (schedulerRunning) {
        cout << "Scheduler is already running.\n";
        return;
    }

    cout << "Starting FCFS Scheduler...\n";
    schedulerRunning = true;

    // Start CPU Core Threads
    for (int i = 0; i < NUM_CORES; ++i) {
        cpuCores.emplace_back(WorkerThread, i + 1);
    }
}

void SchedulerStop() {
    if (!schedulerRunning) {
        cout << "Scheduler is not running.\n";
        return;
    }

    cout << "Stopping scheduler and all CPU cores...\n";
    schedulerRunning = false;
    workersRunning = false;

    // Join all CPU threads
    for (auto& t : cpuCores) {
        if (t.joinable()) t.join();
    }
    cpuCores.clear();

    // Join scheduler thread
    if (schedulerThread.joinable()) schedulerThread.join();

    cout << "All processes finished.\n";
}

/* ========== MAIN ========== */
int main() {
    Welcome();
    Console console;
    for (int i = 1; i <= 10; ++i) {
        stringstream ss;
        ss << "screen_" << setfill('0') << setw(2) << i;
        console.AddScreen(ss.str());
    }
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
        else if (command == "exit") {
            SchedulerStop();
            running = false;
        }
        else cout << "Unknown command. Please try again." << endl;
    }

    return 0;
}