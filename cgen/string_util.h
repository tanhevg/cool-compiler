//
// Created by Evgeny Tanhilevich on 22/04/2016.
//

#ifndef COOL_COMPILER_STRING_UTIL_H
#define COOL_COMPILER_STRING_UTIL_H
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <string>

using std::string;
using std::isspace;
using std::find_if_not;
using std::ptr_fun;

// trim from start
static inline string &ltrim(string &s) {
    s.erase(s.begin(), find_if_not(s.begin(), s.end(), ptr_fun<int, int>(isspace)));
    return s;
}

// trim from end
static inline string &rtrim(string &s) {
    s.erase(find_if_not(s.rbegin(), s.rend(), ptr_fun<int, int>(isspace)).base(), s.end());
    return s;
}

// trim from both ends
static inline string &trim(string &s) {
    return ltrim(rtrim(s));
}

#endif //COOL_COMPILER_STRING_UTIL_H
