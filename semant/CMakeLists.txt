include_directories("${PROJECT_SOURCE_DIR}/tree")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
add_compile_options(-Wno-write-strings)
add_library(semant semant.cc semant_error.cpp type_env.cpp type_checker.cpp type_checker.h)