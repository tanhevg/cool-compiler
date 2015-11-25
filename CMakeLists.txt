cmake_minimum_required(VERSION 3.3)
project(cool-compiler)

add_compile_options(-Wno-deprecated -Wno-write-strings)

include_directories("${PROJECT_SOURCE_DIR}/cooldist")
include_directories("${PROJECT_SOURCE_DIR}/tree")
add_subdirectory(cooldist)
add_subdirectory(tree)
add_subdirectory(semant)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(LEXER_SOURCE_FILES lexer-main.cc cool-lex.cc)
set(PARSER_SOURCE_FILES parser-main.cc cool-parse.cc cool-lex.cc)

add_custom_command(OUTPUT cool-lex.cc DEPENDS cool.flex
        COMMAND flex -ocool-lex.cc ${PROJECT_SOURCE_DIR}/cool.flex)
add_custom_command(OUTPUT cool-parse.cc DEPENDS cool.y
        COMMAND bison -d -v -y -b cool --debug -p cool_yy -o cool-parse.cc ${PROJECT_SOURCE_DIR}/cool.y)

add_executable(cool-lexer ${LEXER_SOURCE_FILES})
target_link_libraries(cool-lexer cooldist l)

add_executable(cool-parser ${PARSER_SOURCE_FILES})
target_link_libraries(cool-parser cooldist tree semant l)

add_executable(test-app test-main.cpp)

