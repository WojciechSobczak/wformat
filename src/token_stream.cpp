#include "token_stream.hpp"


TokenStream::TokenStream(std::filesystem::path file_path) {
    ClangTranslationUnit translation_unit(file_path);
    CXCursor cursor = clang_getTranslationUnitCursor(translation_unit.get());
    CXSourceRange source_range = clang_getCursorExtent(cursor);

    CXToken* clang_tokens = nullptr;
    uint32_t clang_tokens_count = 0;
    clang_tokenize(translation_unit.get(), source_range, &clang_tokens, &clang_tokens_count);
    this->tokens.reserve(clang_tokens_count);

    for (size_t i = 0; i < clang_tokens_count; ++i) {
        CXToken current_clang_token = clang_tokens[i];
        CXSourceLocation clang_source_location = clang_getTokenLocation(translation_unit.get(), current_clang_token);
        CXString clang_token_spelling = clang_getTokenSpelling(translation_unit.get(), current_clang_token);
        CXTokenKind clang_token_kind = clang_getTokenKind(current_clang_token);

        const Token::TokenKind token_kind = [clang_token_kind]() {
            //Removes enum class preferable warning on MSVC
            #pragma warning(push)
            #pragma warning(disable: 26812)
            switch (clang_token_kind) {
                case CXToken_Comment: return Token::TokenKind::COMMENT;
                case CXToken_Identifier: return Token::TokenKind::IDENTIFIER;
                case CXToken_Keyword: return Token::TokenKind::KEYWORD;
                case CXToken_Literal: return Token::TokenKind::LITERAL;
                case CXToken_Punctuation: return Token::TokenKind::PUCTUATION;
            }
            #pragma warning(pop)
            throw std::runtime_error("Cannot parse token kind.");
        }();

        std::string token_string = clang_getCString(clang_token_spelling);

        uint32_t token_line = 0;
        uint32_t token_column = 0;
        uint32_t token_start_index = 0;
        clang_getExpansionLocation(clang_source_location, nullptr, &token_line, &token_column, &token_start_index);

        Token::TokenLocation token_location{
            .start_offset = token_start_index,
            .end_offset = token_start_index + token_string.size(),
            .line = token_line,
            .column = token_column,
        };

        this->tokens.push_back(Token(token_kind, token_string, token_location));
    }

    clang_disposeTokens(translation_unit.get(), clang_tokens, clang_tokens_count);
}


std::optional<const Token*> TokenStream::get(size_t index) {
    if (index >= this->tokens.size()) {
        return {};
    }
    return &this->tokens[index];
}

std::optional<const Token*> TokenStream::peek(size_t how_many_forward) {
    return this->get(this->current_index + how_many_forward);
}
std::optional<const Token*> TokenStream::read() {
    auto next_token = this->peek();
    if (next_token.has_value()) {
        this->current_index++;
    }
    return next_token;
}

void TokenStream::reset() {
    this->current_index = 0;
}
