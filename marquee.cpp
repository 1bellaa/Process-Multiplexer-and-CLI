#include "marquee.h"
#include <string>
#include <windows.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <conio.h>

using namespace std;

/* Marquee Constants */
atomic<bool> RUNNING_MARQUEE(false);
atomic<bool> EXIT_MARQUEE(false);
vector<string> RECENT_COMMANDS;
mutex COMMANDS_MUTEX;
mutex INPUT_MUTEX;
string CURRENT_INPUT = "";

/* Constants for polling and screen refresh */
const int screen_refresh_delay = 20; // higher value, the slower; test 10 vs. 80
const int keyboard_poll_delay = 50;  // 5, 10, 15; test 5 vs. 50

/* Functions for Marquee */
int marquee_x = 1, marquee_y = 5;
int dx = 1, dy = 1;
const int CONSOLE_WIDTH = 120;
const int CONSOLE_HEIGHT = 50;
const string MARQUEE_TEXT = "Hello world in marquee!";

void SetCursorPosition(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void HideCursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void ShowCursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    cursorInfo.bVisible = true;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void MarqueeConsole() {
    system("cls");

    // Header
    SetCursorPosition(0, 0);
    cout << "****************************************" << endl;
    cout << "* Displaying a marquee console! *" << endl;
    cout << "****************************************" << endl;

    // Marquee position
    marquee_x += dx;
    marquee_y += dy;

    // Bounce like dvd 
    if (marquee_x <= 0 || marquee_x + MARQUEE_TEXT.length() >= CONSOLE_WIDTH - 1) {
        dx = -dx;
        marquee_x = max(1, min(marquee_x, CONSOLE_WIDTH - (int)MARQUEE_TEXT.length() - 1));
    }
    if (marquee_y <= 3 || marquee_y >= 15) {
        dy = -dy;
        marquee_y = max(4, min(marquee_y, 14));
    }

    // Draw marquee text at current position
    SetCursorPosition(marquee_x, marquee_y);
    cout << MARQUEE_TEXT;

    // Draw input prompt
    SetCursorPosition(0, 17);
    cout << "Enter a command for marquee console: ";

    {
        lock_guard<mutex> lock(INPUT_MUTEX);
        cout << CURRENT_INPUT;
    }

    // Draw command history
    {
        lock_guard<mutex> lock(COMMANDS_MUTEX);

        // Display recent commands (show last 3) 
        int start_index = max(0, (int)RECENT_COMMANDS.size() - 3);
        for (int i = start_index; i < RECENT_COMMANDS.size(); i++) {
            SetCursorPosition(0, 18 + (i - start_index));
            cout << "Command processed in marquee console: " << RECENT_COMMANDS[i];
        }
    }

    cout.flush(); // Display output immediately
}

void MarqueeWorkerThread() {
    while (RUNNING_MARQUEE && !EXIT_MARQUEE) {
        if (!EXIT_MARQUEE) {
            MarqueeConsole();
        }
        this_thread::sleep_for(chrono::milliseconds(screen_refresh_delay)); // Speed for flickering effect
    }
}

void ProcessMarqueeCommand(const string& command) {
    if (command == "clear") {
        {
            lock_guard<mutex> lock(COMMANDS_MUTEX);
            RECENT_COMMANDS.clear();
        }
    }
    else if (command == "exit") {
        EXIT_MARQUEE = true;
        RUNNING_MARQUEE = false;
    }
    else if (!command.empty()) {
        {
            lock_guard<mutex> lock(COMMANDS_MUTEX);
            RECENT_COMMANDS.push_back(command);
            // Last 3 commands
            if (RECENT_COMMANDS.size() > 3) {
                RECENT_COMMANDS.erase(RECENT_COMMANDS.begin());
            }
        }
    }
}

void MarqueeInputThread() {
    while (RUNNING_MARQUEE && !EXIT_MARQUEE) {
        if (_kbhit()) {
            char ch = _getch();

            if (ch == 13) {
                string command_to_process;
                {
                    lock_guard<mutex> lock(INPUT_MUTEX);
                    command_to_process = CURRENT_INPUT;
                    CURRENT_INPUT = "";
                }

                if (!command_to_process.empty()) {
                    ProcessMarqueeCommand(command_to_process);
                }
            }
            else if (ch == 8) {
                {
                    lock_guard<mutex> lock(INPUT_MUTEX);
                    if (!CURRENT_INPUT.empty()) {
                        CURRENT_INPUT.pop_back();
                    }
                }
            }
            else if (ch >= 32 && ch <= 126) { // Printable characters
                {
                    lock_guard<mutex> lock(INPUT_MUTEX);
                    CURRENT_INPUT += ch;
                }
            }
        }
        this_thread::sleep_for(chrono::milliseconds(keyboard_poll_delay)); // Faster input response
    }
}

void StartMarqueeConsole() {
    system("cls");
    HideCursor();

    RUNNING_MARQUEE = true;
    EXIT_MARQUEE = false;
    marquee_x = 1;
    marquee_y = 5;
    dx = 1;
    dy = 1;

    {
        lock_guard<mutex> lock(COMMANDS_MUTEX);
        RECENT_COMMANDS.clear();
    }

    {
        lock_guard<mutex> lock(INPUT_MUTEX);
        CURRENT_INPUT = "";
    }

    thread animation_thread(MarqueeWorkerThread);
    thread input_thread(MarqueeInputThread);

    while (RUNNING_MARQUEE && !EXIT_MARQUEE) {
        this_thread::sleep_for(chrono::milliseconds(100));
    }

    RUNNING_MARQUEE = false;
    if (animation_thread.joinable()) {
        animation_thread.join();
    }
    if (input_thread.joinable()) {
        input_thread.join();
    }

    ShowCursor();
    system("cls");
}