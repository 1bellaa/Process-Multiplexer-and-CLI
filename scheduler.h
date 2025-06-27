/**
 * @file scheduler.h
 * @brief This file contains the CPUScheduler class definition
 */

#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <cstdint>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <unordered_map>
#include <queue>
#include <deque>
#include <ctime>

using namespace std;

/* ========== PROCESS CONTEXT INTERFACE ========== */
class IProcessContext {
public:
    virtual void log(const std::string& message) = 0;
    virtual void setSymbol(const std::string& name, uint16_t value) = 0;
    virtual uint16_t getSymbol(const std::string& name) const = 0;
    virtual std::string getName() const = 0;
    virtual int& getQuantumLeft() = 0;
    virtual int& getSleepCounter() = 0;
    virtual int& getForLoopDepth() = 0;
    virtual ~IProcessContext() = default;
};

/* ========== PROCESS INSTRUCTION TYPES ========== */
enum InstructionType {
    PRINT, DECLARE, ADD, SUBTRACT, SLEEP, FOR_LOOP
};

/* ========== INSTRUCTION STRUCT ========== */
class Instruction {
    InstructionType type;
public:
    Instruction(InstructionType t) : type(t) {}

    InstructionType getType() const {
        return type;
    }

    void execute(IProcessContext& context) {
        switch (type) {
        case PRINT: {
            context.log("Hello world from " + context.getName() + "!");
            break;
        }
        case DECLARE: {
            string name;
            int length = 5;
			// Generate a random variable name
            static const char charset[] = "abcdefghijklmnopqrstuvwxyz";
            for (size_t i = 0; i < length; ++i) {
                name += charset[rand() % (sizeof(charset) - 1)];
            }
            uint16_t value = static_cast<uint16_t>(rand() % 65536);
            context.setSymbol(name, value);
            break;
        }
        case ADD: {
            string varNames[3] = { "", "", "" };
            uint16_t values[3];
            for (int i = 0; i < 3; ++i) {
                bool useExisting = (rand() % 2 == 0);
                if (useExisting) {
                    string name = "var" + to_string(rand() % 10);
                    varNames[i] = name;
                    values[i] = context.getSymbol(name);
                }
                else {
                    int length = 5;
                    static const char charset[] = "abcdefghijklmnopqrstuvwxyz";
                    for (int j = 0; j < length; ++j) {
                        varNames[i] += charset[rand() % (sizeof(charset) - 1)];
                    }
                    values[i] = static_cast<uint16_t>(rand() % 65536);
                    context.setSymbol(varNames[i], values[i]);
                }
            }
            values[0] = values[1] + values[2];
            context.setSymbol(varNames[0], values[0]);
            break;
        }
        case SUBTRACT: {
            string varNames[3] = { "", "", "" };
            uint16_t values[3];
            for (int i = 0; i < 3; ++i) {
                bool useExisting = (rand() % 2 == 0);
                if (useExisting) {
                    string name = "var" + to_string(rand() % 10);
                    varNames[i] = name;
                    values[i] = context.getSymbol(name);
                }
                else {
                    int length = 5;
                    static const char charset[] = "abcdefghijklmnopqrstuvwxyz";
                    for (int j = 0; j < length; ++j) {
                        varNames[i] += charset[rand() % (sizeof(charset) - 1)];
                    }
                    values[i] = static_cast<uint16_t>(rand() % 65536);
                    context.setSymbol(varNames[i], values[i]);
                }
            }
            values[0] = values[1] - values[2];
            context.setSymbol(varNames[0], values[0]);
            break;
        }
        case SLEEP: {
            int sleepDuration = rand() % 255 + 1;
            while (sleepDuration > 0) {
                sleepDuration--;
                context.getQuantumLeft()--;
                if (context.getQuantumLeft() <= 0) {
                    context.getSleepCounter() = sleepDuration;
                    return;
                }
            }
            break;
        }
        case FOR_LOOP: {
            InstructionType newInstType;
            int numInstructions = rand() % 10 + 1;
            vector<Instruction> loopInstructions;
            context.getForLoopDepth()++;
            for (int i = 0; i < numInstructions; ++i) {
                if (context.getForLoopDepth() == 3) {
                    newInstType = static_cast<InstructionType>(rand() % 5);
                }
                else {
                    newInstType = static_cast<InstructionType>(rand() % 6);
                }
                loopInstructions.push_back(Instruction(newInstType));
            }
            int repeatCount = rand() % 10 + 1;
            for (int i = 0; i < repeatCount; ++i) {
                for (auto& inst : loopInstructions) {
                    inst.execute(context);
                }
            }
            context.getForLoopDepth()--;
            break;
        }
        default: {
            cout << "[ERROR] Unknown instruction type." << endl;
            break;
        }
        }
    }
};

/* ========== PROCESS CLASS ========== */
class Process : public IProcessContext {
public:
    string name;
    int pid;
    vector<Instruction> instructions;
    unordered_map<string, uint16_t> symbolTable;
    vector<string> outputLog;
    int currentInstruction = 0;
    int sleepCounter = 0;
    bool isFinished = false;
    string createdAt;
    int coreId = -1;
    int quantumLeft = 0;
    int forLoopDepth = 0;

    Process() {}

    Process(string n, int id) : name(n), pid(id) {
        createdAt = getCurrentTimestamp();
    }

    void addInstruction(const Instruction& inst) {
        instructions.push_back(inst);
    }

    bool executeNextInstruction(int delays) {
        if (sleepCounter > 0) {
            sleepCounter--;
            return false;
        }
        if (currentInstruction >= instructions.size()) {
            isFinished = true;
            return true;
        }
        Instruction& inst = instructions[currentInstruction];
        inst.execute(*this); // Pass as IProcessContext
        int i = 0;
        while (i < delays) {
            i++;
        }
        currentInstruction++;
        return false;
    }

    // IProcessContext interface implementation
    void log(const std::string& message) override {
        outputLog.push_back(message);
    }
    void setSymbol(const std::string& name, uint16_t value) override {
        symbolTable[name] = value;
    }
    uint16_t getSymbol(const std::string& name) const override {
        auto it = symbolTable.find(name);
        return it != symbolTable.end() ? it->second : 0;
    }
    std::string getName() const override {
        return name;
    }
    int& getQuantumLeft() override {
        return quantumLeft;
    }
    int& getSleepCounter() override {
        return sleepCounter;
    }
    int& getForLoopDepth() override {
        return forLoopDepth;
    }

private:
    string getCurrentTimestamp() {
        time_t now = time(0);
        tm ltm;
        localtime_s(&ltm, &now);
        char buffer[50];
        strftime(buffer, sizeof(buffer), "%m/%d/%Y, %I:%M:%S %p", &ltm);
        return string(buffer);
    }
};

/* ========== CPU SCHEDULER ========== */
class CPUScheduler {
public:
    struct Config {
        int numCpu = 4;
        string scheduler = "rr"; // "rr" for Round Robin
        int quantumCycles = 5;
        int batchProcessFreq = 1;
        int minIns = 1000;
        int maxIns = 1000;
        int delaysPerExec = 0;
    } config;

    CPUScheduler() : isRunning(false), cpuCycles(0) {}

    ~CPUScheduler() {
        stopScheduler();
    }

    bool loadConfig() {
        ifstream file("../config.txt");
        if (!file.is_open()) {
            cout << "[CONFIG] Config file not found. Using default values." << endl;
            return true; // Still consider it a success with defaults
        }
        string line;
        while (getline(file, line)) {
            istringstream iss(line);
            string param, value;
            if (iss >> param >> value) {
                if (param == "num-cpu") config.numCpu = stoi(value);
                else if (param == "scheduler") config.scheduler = value;
                else if (param == "quantum-cycles") config.quantumCycles = stoi(value);
                else if (param == "batch-process-freq") config.batchProcessFreq = stoi(value);
                else if (param == "min-ins") config.minIns = stoi(value);
                else if (param == "max-ins") config.maxIns = stoi(value);
                else if (param == "delays-per-exec") config.delaysPerExec = stoi(value);
            }
        }
        file.close();
        cout << "[CONFIG] Configuration loaded successfully." << endl;
        return true;
    }

    void startScheduler() {
        if (isRunning) {
            cout << "[SCHEDULER] Scheduler is already running." << endl;
            return;
        }

        isRunning = true;
        cpuCycles = 0;

        // Start worker threads
        string schedulerType = (config.scheduler == "rr") ? "Round Robin" : "FCFS";
		// For each CPU core, create a thread that will handle process execution
		// Each thread will run a loop that checks for processes in the queue
        for (int i = 0; i < config.numCpu; ++i) {
            cpuCores.emplace_back([this, schedulerType, i]() {
                while (isRunning) {
                    Process* currentProcess = nullptr;
                    bool hasWork = false;
                    {
                        lock_guard<mutex> lock(queueMutex);
                        if (schedulerType == "Round Robin" && !rrQueue.empty()) {
                            currentProcess = rrQueue.front();
                            rrQueue.pop_front();
                            hasWork = true;
                        }
                        else if (schedulerType == "FCFS" && !readyQueue.empty()) {
                            currentProcess = readyQueue.front();
                            readyQueue.pop();
                            hasWork = true;
                        }
                    }

					// If no process is available, sleep for a short time
                    if (!hasWork) {
                        this_thread::sleep_for(chrono::milliseconds(100));
                        continue;
                    }

                    if (schedulerType == "Round Robin") {
						// TODO: Implement Round Robin scheduling logic
						// I think we should put the round robin logic somewhere else, like in a separate function
						// STEPS:
						// Get first process from the queue
						// Execute the process for the time slice
						// Check if the process is finished or if time slice is over
						// If finished, remove it from the queue
						// If not finished, push it back to the end of the queue
						// You can use Yuri's code as a reference for this logic, in the FCFS branch
                    }
                    else {
						// TODO: Implement FCFS scheduling logic
						// FCFS scheduling logic
						// Execute the process until it finishes (quantum ignored)
                    }
                }
                });
        }

        // Start the scheduler thread (for status output and process generation)
        schedulerThread = thread([this, schedulerType]() {
            while (isRunning) {
                cpuCycles++;

                // Generate a process every tick or every batchProcessFreq ticks
                if (config.batchProcessFreq <= 1 || cpuCycles % config.batchProcessFreq == 0) {
                    // Generate a unique process name
                    string pname = "P" + to_string(nextPid);
                    Process newProc(pname, nextPid++);
                    // Add random instructions
                    int numInstructions = config.minIns + rand() % (config.maxIns - config.minIns + 1);
                    for (int i = 0; i < numInstructions; ++i) {
                        InstructionType t = static_cast<InstructionType>(rand() % 6);
                        newProc.addInstruction(Instruction(t));
                    }

                    {
                        lock_guard<mutex> lock(schedulerMutex);
                        processes.push_back(std::move(newProc));
                        Process* pPtr = &processes.back();
                        lock_guard<mutex> qlock(queueMutex);
                        if (schedulerType == "Round Robin") {
                            rrQueue.push_back(pPtr);
                        }
                        else {
                            readyQueue.push(pPtr);
                        }
                    }
                    cout << "[SCHEDULER] New process generated: " << pname << endl;
                }

				// Just for showing CPU cycles and active processes: REMOVE before passing
                if (cpuCycles % 1000 == 0) {
                    lock_guard<mutex> lock(schedulerMutex);
                    cout << "[SCHEDULER] CPU Cycles: " << cpuCycles
                        << ", Active Processes: " << processes.size() << endl;
                }
                this_thread::sleep_for(chrono::milliseconds(10));
            }
            });
    }

    void stopScheduler() {
        if (!isRunning) {
            cout << "[SCHEDULER] Scheduler is not running." << endl;
            return;
        }

        isRunning = false;

        if (schedulerThread.joinable()) {
            schedulerThread.join();
        }

        for (auto& core : cpuCores) {
            if (core.joinable()) {
                core.join();
            }
        }
        cpuCores.clear();

        cout << "[SCHEDULER] Scheduler stopped." << endl;
    }

    bool findProcess(const string& name) {
        lock_guard<mutex> lock(schedulerMutex);
        for (const auto& proc : processes) {
            if (proc.name == name) {
                return true;
            }
        }
        return false;
    }

    // Add a process to the scheduler
    void addProcess(const string& name) {
        lock_guard<mutex> lock(schedulerMutex);
        int pid = nextPid++;
        processes.push_back(Process(name, pid));
        cout << "[SCHEDULER] Process '" << name << "' added with PID " << pid << endl;
    }

    // Get current CPU cycle count
    uint64_t getCpuCycles() const {
        return cpuCycles;
    }

private:
    vector<Process> processes;
    deque<Process*> rrQueue;
    queue<Process*> readyQueue;
    mutex queueMutex;
    vector<thread> cpuCores;
    thread schedulerThread;
    atomic<bool> isRunning;
    atomic<uint64_t> cpuCycles;
    mutex schedulerMutex;
    int nextPid = 1000;

    // Optionally, move these into CPUScheduler if you use them
    vector<string> finishedScreens;
    map<string, pair<int, int>> progressMap;
    map<string, int> processCoreMap;
};