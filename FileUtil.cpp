// Purpose: Implementation of shared text/file helpers
// Author: Shared work (all team members)
#include "FileUtil.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <sstream>

namespace FileUtil {

namespace {
const std::string kWhitespace = " \t\n\r\f\v";
}

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(kWhitespace);
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(kWhitespace);
    return str.substr(first, (last - first + 1));
}

std::string toUpper(const std::string& str) {
    std::string result = str;
    for (char& ch : result) {
        ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
    }
    return result;
}

std::string toLower(const std::string& str) {
    std::string result = str;
    for (char& ch : result) {
        ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    }
    return result;
}

bool equalsIgnoreCase(const std::string& a, const std::string& b) {
    std::string la = toLower(trim(a));
    std::string lb = toLower(trim(b));
    return la == lb;
}

bool isBlankLine(const std::string& line) {
    return trim(line).empty();
}

bool isCommentLine(const std::string& line) {
    std::string t = trim(line);
    if (t.empty()) {
        return false;
    }
    return t[0] == '#';
}

bool isIgnorableLine(const std::string& line) {
    return isBlankLine(line) || isCommentLine(line);
}

std::vector<std::string> splitFields(const std::string& line, char delimiter) {
    std::vector<std::string> fields;
    std::stringstream ss(line);
    std::string field;
    while (std::getline(ss, field, delimiter)) {
        fields.push_back(trim(field));
    }
    return fields;
}

std::string baseName(const std::string& path) {
    size_t pos = path.find_last_of("/\\");
    if (pos == std::string::npos) {
        return path;
    }
    return path.substr(pos + 1);
}

std::string joinPath(const std::string& dir, const std::string& file) {
    if (dir.empty()) {
        return file;
    }
    if (dir.back() == '/' || dir.back() == '\\') {
        return dir + file;
    }
    return dir + "/" + file;
}

bool parseNumber(const std::string& str, int& out) {
    std::string t = trim(str);
    if (t.empty()) {
        return false;
    }
    size_t i = 0;
    if (t[0] == '-' || t[0] == '+') {
        if (t.length() == 1) {
            return false;
        }
        i = 1;
    }
    for (; i < t.length(); ++i) {
        if (!std::isdigit(static_cast<unsigned char>(t[i]))) {
            return false;
        }
    }
    try {
        out = std::stoi(t);
    } catch (...) {
        return false;
    }
    return true;
}

void printWarning(const std::string& filename, int lineNumber, const std::string& reason) {
    std::cout << baseName(filename) << ":" << lineNumber << ": " << reason << "\n";
}

} // namespace FileUtil
