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

using namespace std;

class Process; // Forward declaration

/* ========== PROCESS INSTRUCTION TYPES ========== */
enum InstructionType {
    PRINT, DECLARE, ADD, SUBTRACT, SLEEP, FOR_LOOP
};

/* ========== INSTRUCTION STRUCT ========== */
class Instruction {
    InstructionType type;
public:
	Instruction(InstructionType t){
		this->type = t;
    }

    InstructionType getType() const {
        return type;
	}

    void execute(Process& process) {
         switch (type) {  
             case PRINT: {  
                 process.outputLog.push_back("Hello world from " + process.name + "!");  
                 break;  
             }  
             case DECLARE: {  
                 string name;  
                 int length = 5;  
                 static const char charset[] = "abcdefghijklmnopqrstuvwxyz";  
                 for (size_t i = 0; i < length; ++i) {  
                     name += charset[rand() % (sizeof(charset) - 1)];  
                 }  
                 uint16_t value = static_cast<uint16_t>(rand() % 65536);  
                 process.symbolTable[name] = value;  
                 break;  
             }  
             case ADD: {  
                 // For value1, value2, value3: randomly use existing variable or create new one
                 auto& table = process.symbolTable;
                 string varNames[3];
                 uint16_t values[3];

                 for (int i = 0; i < 3; ++i) {
                     bool useExisting = (!table.empty() && (rand() % 2 == 0));
                     if (useExisting) {
                         // Pick a random existing variable
                         size_t idx = rand() % table.size();
                         auto it = table.begin();
                         std::advance(it, idx);
                         varNames[i] = it->first;
                         values[i] = it->second;
                     } else {
                         // Create a new variable
                         int length = 5;
                         static const char charset[] = "abcdefghijklmnopqrstuvwxyz";
                         for (int j = 0; j < length; ++j) {
                             varNames[i] += charset[rand() % (sizeof(charset) - 1)];
                         }
                         values[i] = static_cast<uint16_t>(rand() % 65536);
                         table[varNames[i]] = values[i];
                     }
                 }

                 // Perform the addition: value1 = value2 + value3
                 values[0] = values[1] + values[2];
                 table[varNames[0]] = values[0];
                 break;
             }  
             case SUBTRACT: {
                 // For value1, value2, value3: randomly use existing variable or create new one
                 auto& table = process.symbolTable;
                 string varNames[3];
                 uint16_t values[3];

                 for (int i = 0; i < 3; ++i) {
                     bool useExisting = (!table.empty() && (rand() % 2 == 0));
                     if (useExisting) {
                         // Pick a random existing variable
                         size_t idx = rand() % table.size();
                         auto it = table.begin();
                         std::advance(it, idx);
                         varNames[i] = it->first;
                         values[i] = it->second;
                     }
                     else {
                         // Create a new variable
                         int length = 5;
                         static const char charset[] = "abcdefghijklmnopqrstuvwxyz";
                         for (int j = 0; j < length; ++j) {
                             varNames[i] += charset[rand() % (sizeof(charset) - 1)];
                         }
                         values[i] = static_cast<uint16_t>(rand() % 65536);
                         table[varNames[i]] = values[i];
                     }
                 }

                 // Perform the addition: value1 = value2 + value3
                 values[0] = values[1] - values[2];
                 table[varNames[0]] = values[0];
                 break;
             }
             case SLEEP: {
			     // Sleep for a random duration between 1 and 255 seconds
			     int sleepDuration = rand() % 255 + 1; // Random duration between 1 and 255 seconds
                 while (sleepDuration > 0) {
                     sleepDuration--;
				     process.quantumLeft--; // Decrement quantum left
				     // for round robin, if sleeping exceeds quantum, yield the CPU
                     if (process.quantumLeft <= 0) {
                         process.sleepCounter = sleepDuration; // Set sleep counter
                         return; // Yield CPU
                     }
                 }
                 break;
             }  
             case FOR_LOOP: {
                 InstructionType newInstType;

                 // Generate random array of instructions
			     int numInstructions = rand() % 10 + 1; // Random number of instructions between 1 and 10
                 vector<Instruction> loopInstructions;


				 process.forLoopDepth++; // Increment loop depth for nested loops
                 for (int i = 0; i < numInstructions; ++i) {
                     if (process.forLoopDepth == 3) { // Prevent more than 3 nested loops
						 newInstType = static_cast<InstructionType>(rand() % 5); // Randomly choose from PRINT, DECLARE, ADD, SUBTRACT, SLEEP
                     }
                     else { // if nested loops not reaching 3, then carry on with potentially adding for loops
                         newInstType = static_cast<InstructionType>(rand() % 6); // Randomly choose from PRINT, DECLARE, ADD, SUBTRACT, SLEEP, FOR_LOOP
                     }
                     loopInstructions.push_back(Instruction(newInstType));
                 }
			     // random number of repeat iterations for the loop
			     int repeatCount = rand() % 10 + 1; // Random number of iterations between 1 and 10

			     // Execute the loop instructions
                 for (int i = 0; i < repeatCount; ++i) {
                     for (auto& inst : loopInstructions) {
                         inst.execute(process);
                     }
				 }
				 process.forLoopDepth--; // Decrement loop depth after execution
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
class Process {
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
	int forLoopDepth = 0; // For tracking nested loops

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
        inst.execute(*this);

        int i = 0;
        while (i < delays) {
            i++;
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
        } else if (config.scheduler == "fcfs") {
            for (auto& proc : processes) {
				// First-Come, First-Served implementation
            }
        } else {
            cout << "[SCHEDULER] Unknown scheduling algorithm: " << config.scheduler << endl;
		}
        // Additional scheduling algorithms can be added here
    }
};