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

using namespace std;

/* ========== PROCESS INSTRUCTION TYPES ========== */
enum InstructionType {
    PRINT, DECLARE, ADD, SUBTRACT, SLEEP, FOR_LOOP, END_FOR
};

/* ========== INSTRUCTION STRUCT ========== */
struct Instruction {
    InstructionType type;
};

/* ========== PROCESS CLASS ========== */
class Process {
public:
    string name;
    int pid;
    vector<Instruction> instructions;
    map<string, uint16_t> variables;
    vector<string> outputLog;
    int currentInstruction = 0;
    int sleepCounter = 0;
    bool isFinished = false;
    string createdAt;
    int coreId = -1;
    int quantumLeft = 0;

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

        switch (inst.type) {
            // add other commands here later
            case PRINT: {
                string msg = "Hello world from " + name + "!";
                outputLog.push_back(msg);
                break;
            }
            case DECLARE: {
                // Example: Declare a variable with a default value
                break;
			}
            case ADD: {
                // Add two variables
                break;
            }
            case SUBTRACT: {
                // Subtract two variables
                break;
			}
            case SLEEP: {
                // Simulate a sleep operation
                break;
            }
            case FOR_LOOP: {
                // Handle for loop logic (not implemented in this example)
                break;
            }
            case END_FOR: {
                // Handle end of for loop (not implemented in this example)
                break;
            }
            default:
                cout << "[ERROR] Unknown instruction type." << endl;
				return true; // Mark as finished on error
        }

        currentInstruction++;
        return false;
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

        // Start the scheduler thread
        schedulerThread = thread([this]() {
            cout << "[SCHEDULER] Scheduler started. Using "
                << (config.scheduler == "rr" ? "Round Robin" : config.scheduler)
                << " algorithm with " << config.numCpu << " CPUs." << endl;

            while (isRunning) {
                // Perform scheduling
                schedule();

                // Increment CPU cycle counter
                cpuCycles++;

                // Output status every 1000 cycles (can be adjusted)
                if (cpuCycles % 1000 == 0) {
                    lock_guard<mutex> lock(schedulerMutex);
                    cout << "[SCHEDULER] CPU Cycles: " << cpuCycles
                        << ", Active Processes: " << processes.size() << endl;
                }

                // Sleep to prevent maxing out CPU
                this_thread::sleep_for(chrono::milliseconds(10));
            }

            cout << "[SCHEDULER] Scheduler stopped after " << cpuCycles << " cycles." << endl;
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
    atomic<bool> isRunning;
    atomic<uint64_t> cpuCycles;
    thread schedulerThread;
    mutex schedulerMutex;
    int nextPid = 1000;

    void schedule() {
        // The actual scheduling algorithm implementation
        lock_guard<mutex> lock(schedulerMutex);

        if (processes.empty()) {
            return;  // Nothing to schedule
        }

        // Simple implementation of Round Robin for now
        if (config.scheduler == "rr") {
            // Round Robin implementation
            for (auto& proc : processes) {
                if (!proc.isFinished) {
                    // Assign processor if not assigned
                    if (proc.coreId == -1) {
                        // Find available core (simplified)
                        for (int i = 0; i < config.numCpu; i++) {
                            // Check if core i is available (simplified)
                            bool coreAvailable = true;
                            for (const auto& p : processes) {
                                if (p.coreId == i) {
                                    coreAvailable = false;
                                    break;
                                }
                            }

                            if (coreAvailable) {
                                proc.coreId = i;
                                proc.quantumLeft = config.quantumCycles;
                                break;
                            }
                        }
                    }

                    // If process has a core assigned
                    if (proc.coreId != -1) {
                        // Execute one instruction
                        bool finished = proc.executeNextInstruction(config.delaysPerExec);

                        // Decrement quantum
                        proc.quantumLeft--;

                        // If quantum expired or process finished, release core
                        if (proc.quantumLeft <= 0 || finished) {
                            proc.coreId = -1;
                        }
                    }
                }
            }
        }
        // Additional scheduling algorithms can be added here
    }
};