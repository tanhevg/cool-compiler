//
// Created by Evgeny Tanhilevich on 21/02/2016.
//

#include <extern_symbols.h>
#include "PrototypeCoder.h"
#include "cgen_helpers.h"
#include "emit.h"

void PrototypeCoder::code_attr_rec(Symbol class_name, bool encode) {
    Symbol parent_name = classTable->get_parent(class_name);
    if (parent_name != No_class) {
        code_attr_rec(parent_name, true);
    }
    if (encode) {
    }
}

void PrototypeCoder::before(class__class *cls) {
    str << WORD << "-1" << endl;
    Symbol class_name = cls->get_name();
    emit_protobj_ref(class_name, str);
    str << LABEL
    << WORD << classTable->get_class_tag(class_name) << endl
    << WORD << classTable->get_prototype_size(class_name) << endl
    << WORD;
    emit_disptable_ref(class_name, str);
    str << endl;
    classTable->visit_ordered_attrs_of_class(class_name, &attr_coder);
}

void PrototypeAttrCoder::before(attr_class *attr) {
    str << WORD;
    if (attr->get_type() == Bool) {
        falsebool.code_ref(str);
    } else if (attr->get_type() == Str) {
        empty_string->code_ref(str);
    } else if (attr->get_type() == Int) {
        zero_int->code_ref(str);
    } else {
        str << 0;
    }
    str << "\t# " << attr->get_name() << ": " << attr->get_type() << endl;
}
