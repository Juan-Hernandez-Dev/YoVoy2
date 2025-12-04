#ifndef INPUT_UTILS_H
#define INPUT_UTILS_H

#include <string>
#include <iostream>
#include <limits>
#include <conio.h>
#include <optional>
#include <functional>

// ===== INPUT WRAPPER SYSTEM =====
// Clean container-based approach for ESC detection

// Trim whitespace from string
inline std::string trimString(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    size_t end = str.find_last_not_of(" \t\n\r");

    if (start == std::string::npos || end == std::string::npos) {
        return "";
    }

    return str.substr(start, end - start + 1);
}

// Core input reader with ESC detection
inline std::optional<std::string> readInputWithEsc(const std::string& prompt,
                                                     std::function<bool(char)> charFilter = nullptr) {
    std::cout << prompt;
    std::string input = "";

    while (true) {
        if (_kbhit()) {
            int ch = _getch();
            if (ch == 27) { // ESC
                std::cout << "\n";
                return std::nullopt; // Cancellation
            } else if (ch == 13) { // Enter
                std::cout << "\n";
                return input;
            } else if (ch == 8) { // Backspace
                if (!input.empty()) {
                    input.pop_back();
                    std::cout << "\b \b";
                }
            } else if (ch >= 32 && ch <= 126) { // Printable characters
                if (!charFilter || charFilter((char)ch)) {
                    input += (char)ch;
                    std::cout << (char)ch;
                }
            }
        }
    }
}

// Wrapped input functions using std::optional

// Get string input with ESC support
inline std::optional<std::string> getStringInput(const std::string& prompt) {
    while (true) {
        auto result = readInputWithEsc(prompt);
        if (!result.has_value()) {
            return std::nullopt; // ESC pressed
        }

        std::string trimmed = trimString(result.value());
        if (!trimmed.empty()) {
            return trimmed;
        }

        std::cout << "Error: Input cannot be empty! Try again.\n\n";
    }
}

// Get integer input with ESC support
inline std::optional<int> getIntInput(const std::string& prompt) {
    auto numFilter = [](char ch) { return (ch >= '0' && ch <= '9') || ch == '-'; };

    while (true) {
        auto result = readInputWithEsc(prompt, numFilter);
        if (!result.has_value()) {
            return std::nullopt; // ESC pressed
        }

        if (!result.value().empty()) {
            try {
                return std::stoi(result.value());
            } catch (...) {
                std::cout << "Error: Invalid number! Try again.\n\n";
            }
        } else {
            std::cout << "Error: Input cannot be empty! Try again.\n\n";
        }
    }
}

// Get double input with ESC support
inline std::optional<double> getDoubleInput(const std::string& prompt, bool positiveOnly = true) {
    auto numFilter = [](char ch) { return (ch >= '0' && ch <= '9') || ch == '.' || ch == '-'; };

    while (true) {
        auto result = readInputWithEsc(prompt, numFilter);
        if (!result.has_value()) {
            return std::nullopt; // ESC pressed
        }

        if (!result.value().empty()) {
            try {
                double value = std::stod(result.value());
                if (positiveOnly && value <= 0) {
                    std::cout << "Error: Value must be positive! Try again.\n\n";
                    continue;
                }
                return value;
            } catch (...) {
                std::cout << "Error: Invalid number! Try again.\n\n";
            }
        } else {
            std::cout << "Error: Input cannot be empty! Try again.\n\n";
        }
    }
}

// Execute operation with input, handle cancellation automatically
template<typename T, typename Func>
inline bool withInput(std::optional<T> input, Func onSuccess) {
    if (!input.has_value()) {
        std::cout << "Operation cancelled.\n";
        return false;
    }
    onSuccess(input.value());
    return true;
}

// Chain multiple inputs - stops at first cancellation
template<typename Func>
inline bool inputChain(Func operation) {
    return operation();
}

// ===== LEGACY FUNCTIONS (for backward compatibility) =====

// Get non-empty string input with validation (returns empty string if ESC pressed)
inline std::string getValidString(const std::string& prompt) {
    auto result = getStringInput(prompt);
    return result.value_or("");
}

// Get valid integer input (returns -999999 if ESC pressed)
inline int getValidInt(const std::string& prompt) {
    auto result = getIntInput(prompt);
    return result.value_or(-999999);
}

// Get valid double input (returns -999999.0 if ESC pressed)
inline double getValidDouble(const std::string& prompt) {
    auto result = getDoubleInput(prompt);
    return result.value_or(-999999.0);
}

// Check if value indicates cancellation
inline bool isCancelled(const std::string& value) {
    return value.empty();
}

inline bool isCancelled(int value) {
    return value == -999999;
}

inline bool isCancelled(double value) {
    return value == -999999.0;
}

#endif
