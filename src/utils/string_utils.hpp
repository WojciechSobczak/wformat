#pragma once
#include <string>

std::string trim(const std::string& string) {
    size_t first_space = string.find_first_not_of(' ');
    if (first_space == std::string::npos) {
        return "";
    }
    size_t last_space = string.find_last_not_of(' ');
    return string.substr(first_space, (last_space - first_space + 1));
}