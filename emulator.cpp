/**
 * @file emulator.cpp
 * @brief This file contains the main entry point for the CSOPESY Emulator.
 * It initializes the console, handles user commands, and manages the emulator's functionality.
 */

#include <iostream>
#include <string>
#include <ctime>
#include <windows.h>
#include "console.h"
#include "marquee.h"


#pragma once

void StartMarqueeConsole();
using namespace std;

extern void StartMarqueeConsole();

string GetCurrentTimestamp() {
    time_t now = time(0);
    tm ltm;
    localtime_s(&ltm, &now);
    char buffer[50];
    strftime(buffer, sizeof(buffer), "%m/%d/%Y, %I:%M:%S %p", &ltm);
    return string(buffer);
}

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
    cout << "This is for CSOPESY Mockup MO" << endl;
    SetConsoleTextAttribute(handle, 14); // Yellow
    cout << "Type 'exit' to quit, 'clear' to clear the screen" << endl;
    SetConsoleTextAttribute(handle, 15); // White
}

void Clear() {
    system("cls");
    Welcome();
}

int main() {
    Welcome();
    Console console;
    bool running = true;
    while (running) {
        string command;
        cout << "Enter command: ";
        getline(cin, command);
        if (command == "initialize") console.Initialize();
        else if (!console.IsInitialized() && command != "exit") {
            cout << "Please initialize the system first using 'initialize' command." << endl;
            // add a part where the config file will be loaded?? or smth here emememe
        }
        else if (command == "screen -ls") console.ListScreens();
        else if (command.rfind("screen -s ", 0) == 0) console.CreateScreen(command.substr(10));
        else if (command.rfind("screen -r ", 0) == 0) console.ResumeScreen(command.substr(10));
        else if (command == "scheduler-start") console.SchedulerStart();
        else if (command == "scheduler-stop") console.SchedulerStop();
        else if (command == "report-util") console.ReportUtil();
        else if (command == "marquee") {
            StartMarqueeConsole();
            Welcome(); // Show welcome screen again after exiting marquee
        }
        else if (command == "clear") Clear();
        else if (command == "exit") running = false;
        else cout << "Unknown command. Please try again." << endl;
    }
    return 0;
}