cmake_minimum_required(VERSION 3.10)
project(SemanticReasoner)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configuration for External Dependencies
# Ensure the folder structure matches: external/bitnet/3rdparty/llama.cpp
add_subdirectory(external/bitnet)

# Includes
include_directories(src)
include_directories(external/bitnet/include)
include_directories(external/bitnet/3rdparty/llama.cpp/include)

# 1. Main Application Executable
add_executable(SemanticReasoner 
    src/main.cpp 
    src/DiagnosticAgent.cpp
)

# Link against bitnet/llama targets. 
# Note: Ensure 'bitnet' and 'llama' are the correct target names exported by the submodule.
# Sometimes it is just 'llama'.
target_link_libraries(SemanticReasoner PRIVATE bitnet llama)

# 2. Test Suite Executable (ADDED)
add_executable(TestSemanticReasoner
    tests/TestDiagnosticAgent.cpp
    src/DiagnosticAgent.cpp
)

target_link_libraries(TestSemanticReasoner PRIVATE bitnet llama)
