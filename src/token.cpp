#include "token.hpp"
#include <sstream>


Token::Token(const TokenKind token_kind, const std::string& token_text, TokenLocation token_location) 
    : kind(token_kind), text(token_text), location(token_location)
{
}

std::string Token::join(std::vector<const Token*> tokens) noexcept {
    std::stringstream string_stream;
    for (const Token* token : tokens) {
        string_stream << token->text;
    }
    return string_stream.str();
}

void Token::debug_print(std::ostream& out_stream) const noexcept {
    out_stream << "{\n";
    out_stream << "    text: \"" << this->text << "\",\n";
    out_stream << "    kind: " << debug_token_kind_to_string(this->kind) << ",\n";
    out_stream << "    location: {\n";
    out_stream << "        start: " << this->location.start_offset << ",\n";
    out_stream << "        end: " << this->location.end_offset << ",\n";
    out_stream << "        line: " << this->location.line << ",\n";
    out_stream << "        column: " << this->location.column << ",\n";
    out_stream << "    }\n";
    out_stream << "}\n";
}


//Removes not all control paths return a value warning on MSVC, due to lack of default
//Its made on purpose to spawn no all enum values are handled to handle them properly
#pragma warning(push)
#pragma warning(disable: 4715)
std::string Token::debug_token_kind_to_string(TokenKind token_kind) noexcept {
    switch (token_kind) {
        case Token::TokenKind::COMMENT: return "COMMENT";
        case Token::TokenKind::IDENTIFIER: return "IDENTIFIER";
        case Token::TokenKind::KEYWORD: return "KEYWORD";
        case Token::TokenKind::LITERAL: return "LITERAL";
        case Token::TokenKind::PUCTUATION: return "PUCTUATION";
    }
}
#pragma warning(pop)