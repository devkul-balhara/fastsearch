cmake_minimum_required(VERSION 3.16)
project(FastSearch VERSION 2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

add_library(fastsearch
  src/BM25Ranker.cpp
  src/BenchmarkRunner.cpp
  src/ForwardIndex.cpp
  src/MetricsCollector.cpp
  src/PositionalIndex.cpp
  src/QueryParser.cpp
  src/SearchEngine.cpp
  src/SpellCorrector.cpp
  src/TextProcessor.cpp
  src/Trie.cpp
)
target_include_directories(fastsearch PUBLIC include)

add_executable(fastsearch_cli src/main.cpp)
target_link_libraries(fastsearch_cli PRIVATE fastsearch)

add_executable(fastsearch_api api/server.cpp api/ApiServer.cpp)
target_include_directories(fastsearch_api PRIVATE api)
target_link_libraries(fastsearch_api PRIVATE fastsearch)

add_executable(fastsearch_benchmark benchmarks/benchmark.cpp)
target_link_libraries(fastsearch_benchmark PRIVATE fastsearch)

enable_testing()
add_executable(fastsearch_tests tests/test_fastsearch.cpp)
target_link_libraries(fastsearch_tests PRIVATE fastsearch)
add_test(NAME fastsearch_tests COMMAND fastsearch_tests)
