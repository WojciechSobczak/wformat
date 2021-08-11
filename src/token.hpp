#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <clang-c/Index.h>

class Token {
public:
    enum class TokenKind {
        COMMENT,
        IDENTIFIER,
        KEYWORD,
        LITERAL,
        PUCTUATION
    };

    struct TokenLocation {
        size_t start_offset{};
        size_t end_offset{};
        size_t line{};
        size_t column{};
    };

public:
    std::string text{};
    TokenLocation location{};
    TokenKind kind{};

    Token() {};
    Token(const TokenKind token_kind, const std::string& text, TokenLocation token_location);

    static std::string join(std::vector<const Token*> tokens) noexcept;

    void debug_print(std::ostream& out_stream = std::cout) const noexcept;
    static std::string debug_token_kind_to_string(TokenKind token_kind) noexcept;
};