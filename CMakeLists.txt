cmake_minimum_required(VERSION 3.10)
project(SemanticReasoner)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# STANDALONE MOCK MODE
# (Uncomment these if you have the real library)
add_subdirectory(external/bitnet)
include_directories(external/bitnet/include)
include_directories(external/bitnet/3rdparty/llama.cpp/include)

include_directories(src)

add_executable(SemanticReasoner 
    src/main.cpp 
    src/DiagnosticAgent.cpp
)

# No linking needed for mock mode
target_link_libraries(SemanticReasoner PRIVATE bitnet llama)

