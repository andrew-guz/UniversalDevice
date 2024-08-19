#include "String.hpp"

std::vector<std::string> SplitString(const std::string& str, const char delimeter, const bool keepEmpty) {
    std::vector<std::string> result;

    std::string currentString;

    const auto storeCurrent = [&result, keepEmpty](const std::string& curr) -> void {
        if (!curr.empty() || keepEmpty)
            result.push_back(curr);
    };

    for (const char& character : str) {
        if (character == delimeter) {
            storeCurrent(currentString);
            currentString.clear();
            continue;
        }
        currentString.push_back(character);
    }

    storeCurrent(currentString);

    return result;
}
