//
// Created by Evgeny Tanhilevich on 07/12/2015.
//

#ifndef COOL_COMPILER_EXTERN_SYMBOLS_H
#define COOL_COMPILER_EXTERN_SYMBOLS_H

#include "stringtab.h"

extern Symbol No_type;
extern Symbol No_class;
extern Symbol Bool;
extern Symbol Int;
extern Symbol Str;
extern Symbol Object;
extern Symbol self;
extern Symbol SELF_TYPE;
extern Symbol prim_slot;

extern IntEntryP zero_int;
extern StringEntryP empty_string;
extern BoolConst falsebool;
extern BoolConst truebool;

#endif //COOL_COMPILER_EXTERN_SYMBOLS_H
