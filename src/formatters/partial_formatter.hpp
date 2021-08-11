#pragma once
#include "../token_stream.hpp"
#include <string>
#include "wformat_exception.hpp"

class Range {
public:
    size_t start_offset{};
    size_t end_offset{};

    Range() = default;
    Range(size_t start_offset, size_t end_offset) : start_offset(start_offset), end_offset(end_offset) {};
};

class Replacement {
public:
    Range range;
    std::string text;

    Replacement(Range range, const std::string& text) : range(range), text(text) {};
};

class PartialFormatter {
public:
    virtual ~PartialFormatter() {};
    virtual std::vector<Replacement> format(TokenStream& /*token_stream*/) {
        throw WFormatException("format() NOT IMPLEMENTED");
    }
};

