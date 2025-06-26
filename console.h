/**
 * @file console.h
 * @brief This file contains the Console class definition
 */

#pragma once
#include <string>
#include <iostream>
#include <map>
#include "scheduler.h"

using namespace std;

string GetCurrentTimestamp();
void Clear();
void Welcome();

class Console {
    bool initialized = false;
    CPUScheduler scheduler;
    map<string, string> screens;

    void DrawScreen(const string& name);
    void ProcessSmi();
    void ScreenSession(const string& name);
    bool ScreenExists(const string& name);

public:
    void Initialize();
    bool IsInitialized();
    void CreateScreen(const string& name);
    void ListScreens();
    void ResumeScreen(const string& name);
    void SchedulerStart();
    void SchedulerStop();
    void ReportUtil();
};