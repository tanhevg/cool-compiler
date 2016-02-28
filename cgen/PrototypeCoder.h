//
// Created by Evgeny Tanhilevich on 21/02/2016.
//

#ifndef COOL_COMPILER_PROTOTYPECODER_H
#define COOL_COMPILER_PROTOTYPECODER_H

#include <iostream>
#include <visitor.h>
#include "ClassTable.h"

using std::ostream;

class PrototypeAttrCoder: public TreeVisitor {
    ostream& str;
public:
    PrototypeAttrCoder(ostream& _str): str(_str) {}
    void before(attr_class *);

};

class PrototypeCoder: public TreeVisitor {
private:
    ostream& str;
    ClassTable* classTable;
    PrototypeAttrCoder attr_coder;
    void code_attr_rec(Symbol class_name, bool encode);
public:
    PrototypeCoder(ostream& _str, ClassTable* _classTable):
            str(_str), classTable(_classTable), attr_coder(_str) {}
    void before(class__class *);

};


#endif //COOL_COMPILER_PROTOTYPECODER_H
