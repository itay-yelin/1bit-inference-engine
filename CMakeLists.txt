cmake_minimum_required(VERSION 3.10)
project(SemanticReasoner)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# User must clone bitnet manually into external/bitnet
# git clone --recursive https://github.com/microsoft/BitNet.git external/bitnet
add_subdirectory(external/bitnet)

include_directories(external/bitnet/include)
include_directories(src)

add_executable(SemanticReasoner 
    src/main.cpp 
    src/DiagnosticAgent.cpp
)

# Link against the bitnet library (name might vary based on bitnet's own cmake, assumed 'bitnet' or 'ggml')
# Adjusting to likely targets based on typical ggml/bitnet structures.
# Often these expose a library named 'bitnet' or 'ggml'. 
target_link_libraries(SemanticReasoner PRIVATE bitnet)
