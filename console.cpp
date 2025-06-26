#include "console.h"
#include <ctime>

using namespace std;

void Console::DrawScreen(const string& name) {
    Clear();
    cout << "[Screen for: " << name << "]" << endl;
    cout << "Process Name: " << name << endl;
    cout << "Instruction Line: 532 / 1240" << endl;
    cout << "Created At: " << screens[name] << endl;
    cout << "CPU Cycles: " << scheduler.getCpuCycles() << endl;
    cout << "-------------------------------" << endl;
}

void Console::ProcessSmi() {
    cout << "process-smi command recognized. Displaying stats..." << endl;
    cout << "[CPU Usage: 34%] [Memory Usage: 128MB] [Disk I/O: 2.4MB/s]" << endl;
    cout << "[CPU Cycles: " << scheduler.getCpuCycles() << "]" << endl;
}

void Console::ScreenSession(const string& name) {
    DrawScreen(name);
    string input;
    while (true) {
        cout << "Enter command: ";
        getline(cin, input);
        if (input == "process-smi") ProcessSmi();
        else if (input == "clear") DrawScreen(name);
        else if (input == "exit") { Clear(); return; }
        else cout << "Unknown command. Try again." << endl;
    }
}

bool Console::ScreenExists(const string& name) {
    return screens.find(name) != screens.end();
}

void Console::Initialize() {
    if (initialized) {
        cout << "[INFO] System is already initialized.\n";
        return;
    }

    if (!scheduler.loadConfig()) {
        cout << "[ERROR] Failed to load config.txt. Initialization aborted.\n";
        return;
    }

    scheduler.startScheduler();
    initialized = true;
    cout << "[SYSTEM] Initialization successful.\n";
}

bool Console::IsInitialized() {
    return initialized;
}

void Console::CreateScreen(const string& name) {
    if (!initialized) {
        cout << "[ERROR] Please initialize the system first using 'initialize'.\n";
        return;
    }

    if (scheduler.findProcess(name)) {
        cout << "Process '" << name << "' already exists. Use 'screen -r " << name << "' to resume." << endl;
        return;
    }

    // Add the process to the scheduler
    scheduler.addProcess(name);

    screens[name] = GetCurrentTimestamp();
    ScreenSession(name);
}

void Console::ListScreens() {
    if (!initialized) {
        cout << "[ERROR] Please initialize the system first using 'initialize'.\n";
        return;
    }

    cout << "Existing Screens:" << endl;
    if (screens.empty()) cout << "  [none yet]" << endl;
    else for (auto& [name, ts] : screens) cout << "  " << name << " (Created: " << ts << ")" << endl;
}

void Console::ResumeScreen(const string& name) {
    if (!initialized) {
        cout << "[ERROR] Please initialize the system first using 'initialize'.\n";
        return;
    }

    if (!ScreenExists(name)) {
        cout << "Screen '" << name << "' does not exist. Use 'screen -s <name>' to create it." << endl;
    }
    else {
        ScreenSession(name);
    }
}

void Console::SchedulerStart() {
    if (!initialized) {
        cout << "[ERROR] Please initialize the system first using 'initialize'.\n";
        return;
    }

    scheduler.startScheduler();
}

void Console::SchedulerStop() {
    if (!initialized) {
        cout << "[ERROR] Please initialize the system first using 'initialize'.\n";
        return;
    }

    scheduler.stopScheduler();
}

void Console::ReportUtil() {
    if (!initialized) {
        cout << "[ERROR] Please initialize the system first using 'initialize'.\n";
        return;
    }

    cout << "[REPORT] Current CPU Cycles: " << scheduler.getCpuCycles() << endl;
    cout << "[REPORT] Scheduler Algorithm: " <<
        (scheduler.config.scheduler == "rr" ? "Round Robin" : scheduler.config.scheduler) << endl;
    cout << "[REPORT] Number of CPUs: " << scheduler.config.numCpu << endl;
    cout << "[REPORT] Quantum Cycles: " << scheduler.config.quantumCycles << endl;
}