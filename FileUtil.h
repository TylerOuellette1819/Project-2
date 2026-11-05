// Purpose: Shared text/file helpers for trimming, splitting, and validation
// Author: Shared work (all team members)
#ifndef FILEUTIL_H
#define FILEUTIL_H

#include <string>
#include <vector>

namespace FileUtil {

std::string trim(const std::string& str);
std::string toUpper(const std::string& str);
std::string toLower(const std::string& str);
bool equalsIgnoreCase(const std::string& a, const std::string& b);

bool isBlankLine(const std::string& line);
bool isCommentLine(const std::string& line);
bool isIgnorableLine(const std::string& line);

std::vector<std::string> splitFields(const std::string& line, char delimiter);

std::string baseName(const std::string& path);
std::string joinPath(const std::string& dir, const std::string& file);

bool parseNumber(const std::string& str, int& out);
void printWarning(const std::string& filename, int lineNumber, const std::string& reason);

} // namespace FileUtil

#endif // FILEUTIL_H
