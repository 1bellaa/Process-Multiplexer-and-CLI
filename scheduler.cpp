#include <string>
#include <windows.h>
#include <iostream>
#include <map>
#include <ctime>
#include <thread>
#include <mutex>
#include <queue>
#include <deque>
#include <fstream>
#include <atomic>
#include <chrono>
#include <sstream>
#include <iomanip>

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
    tm ltm;
    localtime_s(&ltm, &now);
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

vector<Process> allProcesses;

// FCFS and RR Queues
queue<Process> readyQueue;
deque<Process> rrQueue;
mutex queueMutex;

int NUM_CORES = 4;
atomic<bool> schedulerRunning(false);
atomic<bool> workersRunning(true);
atomic<bool> useRoundRobin(false);
int timeQuantum = 10;

vector<thread> cpuCores;
thread schedulerThread;

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

            if (input == "process-smi") ProcessSmi();
            else if (input == "clear") DrawScreen(name);
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

                SetConsoleTextAttribute(handle, 15); // White
                cout << name << "  (" << ts << ")  ";
                SetConsoleTextAttribute(handle, 11); // Cyan
                cout << "Core: " << coreID << "    ";
                SetConsoleTextAttribute(handle, 6); // Dark Yellow
                cout << printsDone << " / " << totalPrints << endl;
            }
        }

        SetConsoleTextAttribute(handle, 10); // Green
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
        lock_guard<mutex> lock(queueMutex);
        allProcesses.push_back(proc);
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
void Initialize() {
    ifstream config("config.txt");
    if (!config.is_open()) {
        cerr << "Failed to open config.txt" << endl;
        return;
    }

    string key;
    while (config >> key) {
        if (key == "num-cpu") {
            config >> NUM_CORES;
        }
        else if (key == "scheduler") {
            string schedType;
            config >> schedType;
            useRoundRobin = (schedType == "rr");
        }
        else if (key == "quantum-cycles") {
            config >> timeQuantum;
        }
    }

    config.close();
    cout << "Configuration loaded. Scheduler: " << (useRoundRobin ? "Round Robin" : "FCFS") << ", Cores: " << NUM_CORES << ", Quantum: " << timeQuantum << endl;
}

void ReportUtil() { cout << "report-util command recognized.\n"; }
void Screen() { cout << "screen command recognized.\n"; }

void WorkerThread(int coreID) {
    while (workersRunning) {
        Process currentProcess;
        bool hasWork = false;

        {
            lock_guard<mutex> lock(queueMutex);
            if (useRoundRobin && !rrQueue.empty()) {
                currentProcess = rrQueue.front();
                rrQueue.pop_front();
                hasWork = true;
            }
            else if (!useRoundRobin && !readyQueue.empty()) {
                currentProcess = readyQueue.front();
                readyQueue.pop();
                hasWork = true;
            }
        }

        if (!hasWork) {
            this_thread::sleep_for(chrono::milliseconds(100));
            continue;
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
        if (!progressMap.count(currentProcess.name)) {
            progressMap[currentProcess.name] = { 0, currentProcess.remaining_prints };
        }

        int printsToDo = useRoundRobin ? min(timeQuantum, currentProcess.remaining_prints) : currentProcess.remaining_prints;

        for (int i = 0; i < printsToDo; ++i) {
            time_t now = time(0);
            tm ltm;
            localtime_s(&ltm, &now);
            char buffer[50];
            strftime(buffer, sizeof(buffer), "(%m/%d/%Y %I:%M:%S%p)", &ltm);

            file << buffer << " Core:" << coreID << " \"Hello world from " << currentProcess.name << "!\"" << endl;

            {
                lock_guard<mutex> lock(queueMutex);
                progressMap[currentProcess.name].first++;
            }

            this_thread::sleep_for(chrono::milliseconds(50));
        }

        currentProcess.remaining_prints -= printsToDo;

        file.close();

        {
            lock_guard<mutex> lock(queueMutex);
            if (currentProcess.remaining_prints > 0 && useRoundRobin) {
                rrQueue.push_back(currentProcess);
            }
            else if (currentProcess.remaining_prints <= 0) {
                finishedScreens.push_back(currentProcess.name);
                processCoreMap.erase(currentProcess.name);
            }
        }
    }
}

void SchedulerStart() {
    if (schedulerRunning) {
        cout << "Scheduler is already running.\n";
        return;
    }

    {
        lock_guard<mutex> lock(queueMutex);
        if (useRoundRobin) {
            for (const auto& proc : allProcesses) {
                rrQueue.push_back(proc);
            }
        }
        else {
            for (const auto& proc : allProcesses) {
                readyQueue.push(proc);
            }
        }
    }

    cout << (useRoundRobin ? "Starting Round Robin Scheduler...\n" : "Starting FCFS Scheduler...\n");
    schedulerRunning = true;

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

    for (auto& t : cpuCores) {
        if (t.joinable()) t.join();
    }
    cpuCores.clear();

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
        else if (command.rfind("screen -s ", 0) == 0) console.AddScreen(command.substr(10));
        else if (command.rfind("screen -r ", 0) == 0) console.ResumeScreen(command.substr(10));
        else if (command == "scheduler-start") {
            if (!schedulerRunning) {
                SchedulerStart();
            }
            else {
                cout << "Scheduler is already running.\n";
            }
        }
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
