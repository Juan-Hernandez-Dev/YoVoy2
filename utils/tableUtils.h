#ifndef TABLE_UTILS_H
#define TABLE_UTILS_H

#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <conio.h>
#include "colors.h"

struct TableRow {
    int id;
    std::string col1;
    std::string col2;
    std::string col3;
};

class InteractivePaginatedTable {
private:
    std::vector<TableRow> rows;
    std::string title;
    std::string col1Header;
    std::string col2Header;
    std::string col3Header;
    int pageSize;
    int currentPage;
    int selectedIndex;
    int totalPages;

    void display() {
        system("cls");
        std::cout << BOLD << title << RESET << "\n";

        if (rows.empty()) {
            std::cout << "No data available.\n";
            return;
        }

        int startIdx = currentPage * pageSize;
        int endIdx = std::min(startIdx + pageSize, (int)rows.size());

        // Header
        std::cout << std::setw(6) << "ID" << " | "
                  << std::setw(20) << col1Header << " | "
                  << std::setw(15) << col2Header;
        if (!col3Header.empty()) {
            std::cout << " | " << std::setw(15) << col3Header;
        }
        std::cout << "\n";
        std::cout << std::string(65, '-') << "\n";

        // Rows
        for (int i = startIdx; i < endIdx; i++) {
            if (i == selectedIndex) {
                std::cout << CYAN << "> ";
            } else {
                std::cout << "  ";
            }

            std::cout << std::setw(4) << rows[i].id << " | "
                      << std::setw(20) << rows[i].col1 << " | "
                      << std::setw(15) << rows[i].col2;
            if (!rows[i].col3.empty()) {
                std::cout << " | " << std::setw(15) << rows[i].col3;
            }
            std::cout << RESET << "\n";
        }

        // Footer
        std::cout << "\n";
        std::cout << "Page " << (currentPage + 1) << " of " << totalPages
                  << " | Total: " << rows.size() << " items\n\n";
        std::cout << "[W/S or Up/Down] Navigate | [A/D or Left/Right] Change Page\n";
        std::cout << "[Enter] Select | [0-9] Enter ID | [ESC] Cancel\n";
    }

    void updateTotalPages() {
        totalPages = (rows.size() + pageSize - 1) / pageSize;
        if (totalPages == 0) totalPages = 1;
    }

public:
    InteractivePaginatedTable(const std::string& t, const std::string& c1,
                             const std::string& c2, const std::string& c3 = "", int ps = 5)
        : title(t), col1Header(c1), col2Header(c2), col3Header(c3),
          pageSize(ps), currentPage(0), selectedIndex(0) {
        updateTotalPages();
    }

    void addRow(int id, const std::string& c1, const std::string& c2, const std::string& c3 = "") {
        rows.push_back({id, c1, c2, c3});
        updateTotalPages();
    }

    void clearRows() {
        rows.clear();
        currentPage = 0;
        selectedIndex = 0;
        updateTotalPages();
    }

    int run() {
        if (rows.empty()) {
            std::cout << "No data available.\n";
            return -1;
        }

        selectedIndex = 0;
        std::string numberInput = "";

        while (true) {
            display();

            if (!numberInput.empty()) {
                std::cout << "\nEntering ID: " << numberInput << "_";
            }

            int ch = _getch();

            // Handle number input
            if (ch >= '0' && ch <= '9') {
                numberInput += (char)ch;
                continue;
            }

            // Backspace
            if (ch == 8 && !numberInput.empty()) {
                numberInput.pop_back();
                continue;
            }

            // Enter with number input
            if (ch == 13 && !numberInput.empty()) {
                int enteredId = std::stoi(numberInput);
                // Validate ID exists
                for (const auto& row : rows) {
                    if (row.id == enteredId) {
                        return enteredId;
                    }
                }
                numberInput = "";
                continue;
            }

            // Arrow keys or WASD
            if (ch == 224 || ch == 0) {
                ch = _getch();
                if (ch == 72 || ch == 'H') { // Up arrow
                    if (selectedIndex > 0) {
                        selectedIndex--;
                        if (selectedIndex < currentPage * pageSize) {
                            currentPage--;
                        }
                    }
                } else if (ch == 80 || ch == 'P') { // Down arrow
                    if (selectedIndex < (int)rows.size() - 1) {
                        selectedIndex++;
                        if (selectedIndex >= (currentPage + 1) * pageSize) {
                            currentPage++;
                        }
                    }
                } else if (ch == 75 || ch == 'K') { // Left arrow
                    if (currentPage > 0) {
                        currentPage--;
                        selectedIndex = currentPage * pageSize;
                    }
                } else if (ch == 77 || ch == 'M') { // Right arrow
                    if (currentPage < totalPages - 1) {
                        currentPage++;
                        selectedIndex = currentPage * pageSize;
                    }
                }
            } else if (ch == 'w' || ch == 'W') {
                if (selectedIndex > 0) {
                    selectedIndex--;
                    if (selectedIndex < currentPage * pageSize) {
                        currentPage--;
                    }
                }
            } else if (ch == 's' || ch == 'S') {
                if (selectedIndex < (int)rows.size() - 1) {
                    selectedIndex++;
                    if (selectedIndex >= (currentPage + 1) * pageSize) {
                        currentPage++;
                    }
                }
            } else if (ch == 'a' || ch == 'A') {
                if (currentPage > 0) {
                    currentPage--;
                    selectedIndex = currentPage * pageSize;
                }
            } else if (ch == 'd' || ch == 'D') {
                if (currentPage < totalPages - 1) {
                    currentPage++;
                    selectedIndex = currentPage * pageSize;
                }
            } else if (ch == 13) { // Enter
                return rows[selectedIndex].id;
            } else if (ch == 27) { // ESC
                return -1;
            }
        }

        return -1;
    }
};

#endif
