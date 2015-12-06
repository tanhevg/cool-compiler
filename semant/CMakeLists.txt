include_directories("${PROJECT_SOURCE_DIR}/tree")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
add_compile_options(-Wno-write-strings)
add_library(semant visitor.cpp semant.cc inheritance.cpp semant_error.cpp symtab.h)