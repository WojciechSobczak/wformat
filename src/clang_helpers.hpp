#pragma once
#include <memory>
#include <filesystem>
#include <clang-c/Index.h>

struct ClangTranslationUnit {
private:
    CXIndex index;
    CXTranslationUnit translation_unit;
public:

    ClangTranslationUnit(std::filesystem::path file_path) {
        this->index = clang_createIndex(0, 0);
        this->translation_unit = clang_parseTranslationUnit(
            index,
            file_path.string().c_str(),
            nullptr,
            0,
            nullptr,
            0,
            clang_defaultEditingTranslationUnitOptions()
        );

        if (this->translation_unit == nullptr) {
            throw std::runtime_error("File given for translation unit cannot be opened");
        }
    }

    ~ClangTranslationUnit(){
        clang_disposeIndex(index);
        clang_disposeTranslationUnit(translation_unit);
    }

    CXTranslationUnit get() {
        return this->translation_unit;
    }
};







