//
// Created by Evgeny Tanhilevich on 22/04/2016.
//

#ifndef COOL_COMPILER_STRING_UTIL_H
#define COOL_COMPILER_STRING_UTIL_H
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

// trim from start
static inline std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if_not(s.begin(), s.end(), std::ptr_fun<int, int>(std::isspace)));
    return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
    s.erase(std::find_if_not(s.rbegin(), s.rend(), std::ptr_fun<int, int>(std::isspace)).base(), s.end());
    return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
    return ltrim(rtrim(s));
}

#endif //COOL_COMPILER_STRING_UTIL_H
