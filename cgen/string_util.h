//
// Created by Evgeny Tanhilevich on 22/04/2016.
//

#ifndef COOL_COMPILER_STRING_UTIL_H
#define COOL_COMPILER_STRING_UTIL_H
#include <string>

using std::string;

static inline const string& erase_newline(string &s) {
    for (auto it = s.begin(); it != s.end(); it++) {
        if (*it == '\n' || *it == '\r') {
            *it = ' ';
        }
    }
    return s;
}


#endif //COOL_COMPILER_STRING_UTIL_H
