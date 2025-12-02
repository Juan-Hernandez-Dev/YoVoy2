#ifndef INPUT_UTILS_H
#define INPUT_UTILS_H

#include <string>
#include <iostream>
#include <limits>

// Trim whitespace from string
inline std::string trimString(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    size_t end = str.find_last_not_of(" \t\n\r");

    if (start == std::string::npos || end == std::string::npos) {
        return "";
    }

    return str.substr(start, end - start + 1);
}

// Get non-empty string input with validation
inline std::string getValidString(const std::string& prompt) {
    std::string input;

    while (true) {
        std::cout << prompt;
        std::getline(std::cin, input);

        input = trimString(input);

        if (!input.empty()) {
            return input;
        }

        std::cout << "Error: Input cannot be empty! Try again.\n\n";
    }
}

// Get valid integer input
inline int getValidInt(const std::string& prompt) {
    int value;

    while (true) {
        std::cout << prompt;

        if (std::cin >> value) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }

        std::cout << "Error: Invalid number! Try again.\n\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

// Get valid double input
inline double getValidDouble(const std::string& prompt) {
    double value;

    while (true) {
        std::cout << prompt;

        if (std::cin >> value) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (value > 0) {
                return value;
            }

            std::cout << "Error: Value must be positive! Try again.\n\n";
        } else {
            std::cout << "Error: Invalid number! Try again.\n\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

#endif
