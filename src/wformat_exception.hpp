#pragma once
#include <string>

class WFormatException {
private:
    std::string message;
public:
    WFormatException(const std::string& message) : message(message) {};
    const std::string& get_message() { return this->message; };
};