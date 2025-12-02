#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <string>
#include <algorithm>

inline std::string sanitizeFilename(const std::string& input) {
    std::string filename = input;

    // Remove leading/trailing whitespace
    size_t start = filename.find_first_not_of(" \t\n\r");
    size_t end = filename.find_last_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    filename = filename.substr(start, end - start + 1);

    // Remove any directory traversal patterns
    size_t pos;
    while ((pos = filename.find("..")) != std::string::npos) {
        filename.erase(pos, 2);
    }

    // Remove path separators (block folder paths)
    while ((pos = filename.find("/")) != std::string::npos) {
        filename.erase(pos, 1);
    }
    while ((pos = filename.find("\\")) != std::string::npos) {
        filename.erase(pos, 1);
    }

    // Remove any existing extension
    pos = filename.find_last_of('.');
    if (pos != std::string::npos) {
        filename = filename.substr(0, pos);
    }

    // Remove any remaining dangerous characters
    filename.erase(std::remove_if(filename.begin(), filename.end(),
        [](char c) { return c == ':' || c == '*' || c == '?' || c == '"' ||
                     c == '<' || c == '>' || c == '|'; }), filename.end());

    return filename;
}

inline std::string getDataPath(const std::string& filename) {
    return "data/" + sanitizeFilename(filename) + ".csv";
}

inline std::string getSeedPath(const std::string& filename) {
    return "seeds/" + sanitizeFilename(filename) + ".csv";
}

#endif
