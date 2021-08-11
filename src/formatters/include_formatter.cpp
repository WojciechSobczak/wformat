#include <variant>
#include <vector>
#include <optional>
#include <sstream>

#include <spdlog/spdlog.h>

#include "include_formatter.hpp"
#include "utils/string_utils.hpp"
#include "cpp_constants.hpp"
#include "wformat_exception.hpp"


std::vector<Replacement> IncludeFormatter::format(TokenStream& token_stream) {
    ParsingResult parsing_result = this->parse_token_stream(token_stream);

    std::vector<Replacement> replacements;

    std::stringstream string_stream;
    if (parsing_result.pragma_once_range.has_value()) {
        string_stream << "#pragma once \n";
        replacements.push_back(Replacement(parsing_result.pragma_once_range.value(), ""));
    }

    for (const Include& include : parsing_result.standard_includes) {
        string_stream << "#include <" << include.name << ">\n";
        replacements.push_back(Replacement(include.in_file_place, ""));
    }

    if (parsing_result.standard_includes.empty() == false) {
        string_stream << "\n";
    }

    for (const Include& include : parsing_result.angled_includes) {
        string_stream << "#include <" << include.name << ">\n";
        replacements.push_back(Replacement(include.in_file_place, ""));
    }

    for (const Include& include : parsing_result.other_includes) {
        string_stream << "#include \"" << include.name << "\"\n";
        replacements.push_back(Replacement(include.in_file_place, ""));
    }

    std::string replacement_text = string_stream.str();
    if (parsing_result.pragma_once_range.has_value()) {
        replacements[0].text = std::move(replacement_text);
    }
    else {
        //Gówno
    }

    return replacements;
}


IncludeFormatter::ParsingResult IncludeFormatter::parse_token_stream(TokenStream& token_stream) {
    ParseState current_state = ParseState::START;

    ParsingResult parsing_result;
    std::vector<const Token*> tokens_involved;
    std::vector<const Token*> include_name_tokens;

    token_stream.for_each([&, this](const Token* token) {
        if (token->text == " ") {
            throw WFormatException("EMPTY TOKEN EXISTS");
        }
        switch (current_state) {
            case ParseState::START: { 
                current_state = this->on_start_state(token, tokens_involved); 
                break;
            }
            case ParseState::HASH: { 
                current_state = this->on_hash_state(parsing_result, token, tokens_involved); 
                break;
            }
            case ParseState::PRAGMA: {
                current_state = this->on_pragma_state(parsing_result, token, tokens_involved);
                break;
            }
            case ParseState::INCLUDE: {
                current_state = this->on_include_state(parsing_result, token, tokens_involved);
                break;
            }
            case ParseState::OPEN_PUNCTUATION: {
                current_state = this->on_open_punctuation_state(parsing_result, token, tokens_involved, include_name_tokens);
                break;
            }
            default:
                break;
        }
    });

    std::sort(parsing_result.standard_includes.begin(), parsing_result.standard_includes.end(), [](const auto& lhs, const auto& rhs) {
        return lhs.name < rhs.name;
    });

    return parsing_result;
}

IncludeFormatter::ParseState IncludeFormatter::on_start_state(const Token* token, std::vector<const Token*>& tokens_involved) {
    spdlog::trace("IncludeFormatter::on_start_state() | Token: {}", token->text);
    if (token->kind == Token::TokenKind::PUCTUATION && token->text == "#") {
        tokens_involved.push_back(token);
        return ParseState::HASH;
    }
    return ParseState::START;
}

IncludeFormatter::ParseState IncludeFormatter::on_hash_state(ParsingResult& parsing_result, const Token* token, std::vector<const Token*>& tokens_involved) {
    spdlog::trace("IncludeFormatter::on_hash_state() | Token: {}", token->text);
    if (token->kind == Token::TokenKind::IDENTIFIER) {
        if (token->text == "pragma") {
            tokens_involved.push_back(token);
            return ParseState::PRAGMA;
        }
        if (token->text == "include") {
            tokens_involved.push_back(token);
            return ParseState::INCLUDE;
        }
        else {
            //We dont want to parse it
            return ParseState::START;
        }
    }
    return ParseState::HASH;
}

IncludeFormatter::ParseState IncludeFormatter::on_pragma_state(ParsingResult& parsing_result, const Token* token, std::vector<const Token*>& tokens_involved) {
    spdlog::trace("IncludeFormatter::on_include_state() | Token: {}", token->text);
    if (token->kind == Token::TokenKind::IDENTIFIER && token->text == "once") {
        tokens_involved.push_back(token);
        parsing_result.pragma_once_range = get_tokens_range(tokens_involved);
        tokens_involved.clear();
    }
    return ParseState::START;
}

IncludeFormatter::ParseState IncludeFormatter::on_include_state(ParsingResult& parsing_result, const Token* token, std::vector<const Token*>& tokens_involved) {
    spdlog::trace("IncludeFormatter::on_include_state() | Token: {}", token->text);
    if (token->kind == Token::TokenKind::PUCTUATION) {
        if (token->text == "<") {
            tokens_involved.push_back(token);
            return ParseState::OPEN_PUNCTUATION;
        }
        throw WFormatException("Bad token after include open punctuation: " + token->text);
    }

    if (token->kind == Token::TokenKind::LITERAL) {
        tokens_involved.push_back(token);
        std::string include_name = clean_include_string(token->text);
        Range tokens_range = get_tokens_range(tokens_involved);
        parsing_result.other_includes.push_back(Include(include_name, tokens_range));
        tokens_involved.clear();
        return ParseState::START;
    }

    throw WFormatException("Bad token after include macro: " + token->text);
}



IncludeFormatter::ParseState IncludeFormatter::on_open_punctuation_state(
    ParsingResult& parsing_result, 
    const Token* token, 
    std::vector<const Token*>& tokens_involved, 
    std::vector<const Token*>& include_name_tokens
) 
{
    spdlog::trace("IncludeFormatter::on_open_puctuation_state() | Token: {}", token->text);
    if (token->kind == Token::TokenKind::PUCTUATION && token->text == ">") {
        std::string include_path = Token::join(include_name_tokens);
        include_path = clean_include_string(include_path);
        tokens_involved.push_back(token);
        Range tokens_range = get_tokens_range(tokens_involved);

        if (CPPConstants::STANDARD_INCLUDES.contains(include_path)) {
            parsing_result.standard_includes.push_back(Include(include_path, tokens_range));
        } else {
            parsing_result.angled_includes.push_back(Include(include_path, tokens_range));
        }

        tokens_involved.clear();
        include_name_tokens.clear();
        return ParseState::START;
    }

    tokens_involved.push_back(token);
    include_name_tokens.push_back(token);

    return ParseState::OPEN_PUNCTUATION;
}

std::string IncludeFormatter::clean_include_string(const std::string& include_bare_string) {
    std::string include_name = include_bare_string;

    for (char& character : include_name) {
        if (character == '"') {
            character = ' ';
        }
    }

    if (include_name.starts_with(" ") || include_name.ends_with(" ")) {
        include_name = trim(include_name);
    }

    return include_name;
}

Range IncludeFormatter::get_tokens_range(const std::vector<const Token*> tokens) {
    size_t start_offset = tokens[0]->location.start_offset;
    size_t end_offset = tokens[tokens.size() - 1]->location.end_offset;

    return Range(start_offset, end_offset);
}














