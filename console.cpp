#include "console.h"
#include <ctime>
#include <fstream>

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
    activeScreens.insert(name);  // Mark as active
    DrawScreen(name);
    string input;

    while (true) {
        cout << "Enter command: ";
        getline(cin, input);

        if (input == "process-smi") ProcessSmi();
        else if (input == "clear") DrawScreen(name);
        else if (input == "exit") {
            activeScreens.erase(name); // Mark as inactive
            Clear();
            return;
        }
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

    /*scheduler.startScheduler();*/
    // scheduler only starts when scheduler-start is called
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
    if (screens.empty()) {
        cout << "  [none yet]" << endl;
    }
    else {
        for (auto& [name, ts] : screens) {
            string status = activeScreens.count(name) ? "Active" : "Inactive";
            cout << "  " << name << " (Created: " << ts << ", Status: " << status << ")" << endl;
        }
    }
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

    ofstream file("csopesy-log.txt");
    if (!file.is_open()) {
        cout << "[ERROR] Could not open csopesy-log.txt for writing.\n";
        return;
    }

    file << "[REPORT] CPU Cycles: " << scheduler.getCpuCycles() << "\n";
    file << "[REPORT] Scheduler: " << (scheduler.config.scheduler == "rr" ? "Round Robin" : "FCFS") << "\n";
    file << "[REPORT] Number of CPUs: " << scheduler.config.numCpu << "\n";
    file << "[REPORT] Quantum Cycles: " << scheduler.config.quantumCycles << "\n";
    file << "--------------------------------------\n";

    const vector<Process>& all = scheduler.getAllProcesses();
    for (const Process& p : all) {
        file << "PID: " << p.pid << "\n";
        file << "Name: " << p.name << "\n";
        file << "Created At: " << p.createdAt << "\n";
        file << "Finished: " << (p.isFinished ? "Yes" : "No") << "\n";
        file << "Instruction Progress: "
            << p.currentInstruction << " / " << p.instructions.size() << "\n";
        file << "--------------------------------------\n";
    }

    file.close();
    cout << "[REPORT] csopesy-log.txt created.\n";
}
