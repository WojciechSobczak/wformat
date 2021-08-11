#pragma once
#include <string>
#include <filesystem>
#include <optional>

#include "token.hpp"
#include "clang_helpers.hpp"


class TokenStream {
private:
    std::vector<Token> tokens{};
    size_t current_index{};
public:
    TokenStream(std::filesystem::path file_path);

    std::optional<const Token*> get(size_t index);
    std::optional<const Token*> peek(size_t how_many_forward = 0);
    std::optional<const Token*> read();

    void reset();

    template<typename Function>
    void for_each(Function&& function) {
        for (auto token = this->read(); token.has_value(); token = this->read()) {
            const Token* token_ptr = token.value();
            function(token_ptr);
        }
    }



};

