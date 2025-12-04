#ifndef KEY_UTILS_H
#define KEY_UTILS_H

#include <conio.h>
#include <thread>
#include <chrono>

// Check if ESC key is pressed (non-blocking)
inline bool isEscPressed() {
    if (_kbhit()) {
        int ch = _getch();
        if (ch == 27) { // ESC key
            return true;
        }
    }
    return false;
}

// Sleep with ESC detection - returns true if ESC was pressed
inline bool sleepWithEscCheck(int milliseconds) {
    const int checkInterval = 50; // Check every 50ms
    int elapsed = 0;

    while (elapsed < milliseconds) {
        if (isEscPressed()) {
            return true; // ESC was pressed
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(checkInterval));
        elapsed += checkInterval;
    }

    return false; // ESC was not pressed
}

#endif
