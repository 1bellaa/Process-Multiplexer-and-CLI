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

    void schedule() {}
    void loadConfig() {
        ifstream file("config.txt");
        if (!file.is_open()) {
            cout << "Config file not found. Using default values." << endl;
            return;
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
    }
    void startScheduler() { cout << "Scheduler started.\n"; }
    void stopScheduler() { cout << "Scheduler stopped.\n"; }
    bool findProcess(const string&) { return false; } // Dummy for now
};
