set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
add_compile_options(-Wno-write-strings)
add_library(tree ClassTable.cpp tree.cc cool-tree.cc dumptype.cc handle_flags.cc)