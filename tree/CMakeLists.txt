include_directories("${PROJECT_SOURCE_DIR}/cooldist")
add_compile_options(-Wno-write-strings)
add_library(tree tree.cc cool-tree.cc dumptype.cc handle_flags.cc)