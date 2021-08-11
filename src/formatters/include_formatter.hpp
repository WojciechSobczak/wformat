#pragma once

#include "partial_formatter.hpp"

class IncludeFormatter : public PartialFormatter {
private:
    enum class ParseState {
        START,
        HASH,
        INCLUDE,
        PRAGMA,
        OPEN_PUNCTUATION
    };

    struct Include {
        std::string name;
        Range in_file_place;

        Include() = default;
        Include(std::string name, Range in_file_place) : name(name), in_file_place(in_file_place) {};
    };

    struct ParsingResult {
        std::optional<Range> pragma_once_range;
        std::vector<Include> standard_includes;
        std::vector<Include> angled_includes;
        std::vector<Include> other_includes;
    };

    ParseState on_start_state(const Token* token, std::vector<const Token*>& tokens_involved);
    ParseState on_hash_state(ParsingResult& parsing_results, const Token* token, std::vector<const Token*>& tokens_involved);
    ParseState on_pragma_state(ParsingResult& parsing_results, const Token* token, std::vector<const Token*>& tokens_involved);
    ParseState on_include_state(ParsingResult& parsing_results, const Token* token, std::vector<const Token*>& tokens_involved);
    ParseState on_open_punctuation_state(
        ParsingResult& parsing_results, 
        const Token* token, 
        std::vector<const Token*>& tokens_involved,
        std::vector<const Token*>& include_name_tokens
    );

    std::string clean_include_string(const std::string& include);
    Range get_tokens_range(const std::vector<const Token*> tokens);

    ParsingResult parse_token_stream(TokenStream& token_stream);

public:
    virtual std::vector<Replacement> format(TokenStream& token_stream) override;
    
};