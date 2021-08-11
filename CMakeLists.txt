cmake_minimum_required(VERSION 3.19)
project("wformat")
set(CMAKE_CXX_STANDARD 20)

#Architecture detection
if (CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "Detected x86 architecture which is unsupported one. Exiting...")
endif()

#Mode and system detection
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("Detected DEBUG mode.")
    set(DEBUG_BUILD true)
else()
    message("Detected RELEASE mode.")
    set(DEBUG_BUILD false)
endif()

#Additional find_package search paths
set(CMAKE_PREFIX_PATH "${DEPENDENCIES_PATHS}" "${CMAKE_PREFIX_PATH}")

#Windows runtime linkage mode setup
if (MSVC)
    if (${DEBUG_BUILD})
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebugDLL")
    else()
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
    endif()
endif()

#CPPCheck setup
if (DEFINED CPPCHECK_PATH)
    message("CPPCheck detected in: ${CPPCHECK_PATH}")
    set(CMAKE_CXX_CPPCHECK "${CPPCHECK_PATH}")
endif()

#SPDLOG setup
find_package(spdlog CONFIG REQUIRED)
link_libraries(spdlog::spdlog)

#Warnings setup
set (WARNINGS_FLAGS)
list(APPEND WARNINGS_FLAGS )
if (MSVC) 
    list(APPEND WARNINGS_FLAGS /W4)
else()
    list(APPEND WARNINGS_FLAGS -Wall -Weverything -Wpedantic -Wno-c++98-compat)
endif()
add_compile_options(${WARNINGS_FLAGS})

if (MSVC)
    add_compile_options(/EHsc) #Enable exceptions
endif()  

add_executable(${PROJECT_NAME}
    "src/main.cpp"
    "src/cpp_constants.hpp" "src/cpp_constants.cpp"
    "src/token_stream.hpp" "src/token_stream.cpp"
    "src/token.hpp" "src/token.cpp" 
    "src/clang_helpers.hpp" 
    "src/formatters/partial_formatter.hpp" 
    "src/formatters/include_formatter.hpp" "src/formatters/include_formatter.cpp" 
    "src/utils/variant_utils.hpp"
 "src/wformat_exception.hpp" "src/utils/string_utils.hpp" )

if (NOT DEFINED LLVM_PATH)
    message(FATAL_ERROR "LLVM_PATH not provided!")
endif()

target_include_directories(${PROJECT_NAME} PRIVATE "${LLVM_PATH}/include/")
target_link_directories(${PROJECT_NAME} PRIVATE "${LLVM_PATH}/lib/")
target_link_libraries(${PROJECT_NAME} PRIVATE "libclang")

target_include_directories(${PROJECT_NAME} PRIVATE "src/")