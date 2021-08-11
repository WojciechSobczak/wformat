#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <clang-c/Index.h>
#include <filesystem>

#include "cpp_constants.hpp"
#include "token_stream.hpp"
#include "clang_helpers.hpp"
#include "formatters/partial_formatter.hpp"
#include "formatters/include_formatter.hpp"

#include <spdlog/spdlog.h>


std::string apply_replacements(const std::string& file_string, const std::vector<Replacement>& replacements) {
    std::string output_string = file_string;
    for (const Replacement& replacement : replacements) {
        //output_string.replace(replacement.range.start_offset, replacement.range.end_offset, replacement.text)
    }
    return output_string;
}


int main(int /*args*/, char** /*argv*/, char** /*env*/) {
    std::filesystem::path path("D:/Programowanie/wformat/format_test.hpp");
    std::filesystem::path temp_path("D:/Programowanie/wformat/format_test.hpp.wformat_temp.hpp");
    if (std::filesystem::exists(temp_path)) {
        std::filesystem::remove(temp_path);
    }
    std::filesystem::copy(path, temp_path);



    spdlog::set_level(spdlog::level::trace);
    std::vector<std::unique_ptr<PartialFormatter>> formatters;
    formatters.push_back(std::make_unique<IncludeFormatter>());
    
    for (auto& formatter : formatters) {
        TokenStream token_stream(temp_path);
        auto replacements = formatter->format(token_stream);

        std::ifstream formatted_file(temp_path);
        std::string read_file_string((std::istreambuf_iterator<char>(formatted_file)), std::istreambuf_iterator<char>());

        std::ofstream output_stream(temp_path);
        output_stream << apply_replacements(read_file_string, replacements);
    }

    return 1;
}
