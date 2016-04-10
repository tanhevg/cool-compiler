//
// Created by Evgeny Tanhilevich on 21/02/2016.
//

#ifndef COOL_COMPILER_PROTOTYPECODER_H
#define COOL_COMPILER_PROTOTYPECODER_H

#include <iostream>
#include <visitor.h>
#include "ClassTable.h"

using std::ostream;

class PrototypeCoder: public TreeVisitor {
private:
    ostream& str;
    ClassTable* classTable;
    void code_attr_rec(Symbol class_name, bool encode);
public:
    PrototypeCoder(ostream& _str, ClassTable* _classTable):
            str(_str), classTable(_classTable){}
    void before(class__class *);

};

class DispTableCoder: public TreeVisitor {
private:
    ostream& str;
    ClassTable *class_table;
public:
    DispTableCoder(ostream& _str, ClassTable* _classTable):
            str(_str), class_table(_classTable){}
    void before(class__class *);

};


#endif //COOL_COMPILER_PROTOTYPECODER_H
