//
// Created by Evgeny Tanhilevich on 04/01/2016.
//

#include <algorithm>
#include "ClassTable.h"
#include "extern_symbols.h"


Class_ ClassTable::get_class(Symbol s) {
    auto find = class_by_name.find(s);
    if (find == class_by_name.end()) {
        return nullptr;
    }
    return find->second->get_class();
}

Symbol ClassTable::get_parent(Symbol class_name) {
    return get_class(class_name)->get_parent();
}

Symbol ClassTable::join(const vector<Symbol> &types) {
    vector<vector<Symbol>> ancestors;
    for (Symbol s : types) {
        vector<Symbol> v;
        Symbol ss = s;
        while (ss != No_class) {
            v.insert(v.begin(), ss);
            ss = get_class(ss)->get_parent();
        }
        // although vector push_back accepts parameters by reference, it creates copies of values for storing them
        // use std::move to create an rvalue reference, such that move is performed instead of a copy
        ancestors.push_back(move(v));
    }
    int i = 0;
    bool br = false;
    Symbol ret = nullptr, test;
    vector<Symbol> &v0 = ancestors[0];
    do {
        test = v0[i];
        for (vector<Symbol> &v : ancestors) {
            if (i >= v.size() || v[i] != test) {
                br = true;
                break;
            }
        }
        if (!br) {
            ++i;
            ret = test;
        }
    } while (!br && i < v0.size());
    return ret;
}

bool ClassTable::is_subtype(Symbol sub, Symbol super) {
    if (sub == No_type) {
        return true;
    }
    while (sub != No_class) {
        if (sub == super) {
            return true;
        }
        sub = get_class(sub)->get_parent();
    }
    return false;
}

void ClassTable::before(class__class *node) {
    int _class_tag = 0;
    if (node->get_name() != Object) {
        _class_tag = class_tag++;
    }
    class_by_name[node->get_name()] = current_class_record = new ClassTableRecord(node, _class_tag);
    stringtable.add_string(node->get_name()->get_string());
}

void ClassTable::after(attr_class *node) {
    current_class_record->add_attr(node);
}

ClassTableRecord::~ClassTableRecord() {
    for (auto item : attribute_index) {
        delete item.second;
    }
}

ClassTable::~ClassTable() {
    for (auto item : class_by_name) {
        delete item.second;
    }
}

void ClassTableRecord::add_attr(attr_class *attr) {
    attribute_index[attr->get_name()] = new IndexedRecord<attr_class>(attr);
}

int ClassTable::get_prototype_size(Symbol class_name) {
    return class_by_name[class_name]->get_prototype_size();
}

void ClassTableRecord::index_features(int start_attr_idx, int start_method_idx) {
    for (auto item : attribute_index) {
        if (item.second->get_index() == -1) {
            item.second->set_index(start_attr_idx++);
        }
    }
    for (auto item : method_index) {
        MethodRecord *method_record = item.second;
        if (method_record->get_index() == -1) {
            method_record->set_index(start_method_idx++);
        }
    }
    indexed = true;
}

void ClassTable::index() {
    class_by_tag = vector<ClassTableRecord*>(class_by_name.size());
    for (auto item : class_by_name) {
        auto class_record = item.second;
        index_attributes_and_methods_rec(item.first);
        class_by_tag[class_record->get_class_tag()] = class_record;
    }
}

void ClassTableRecord::copy_features(ClassTableRecord *parent_record) {
    for (auto attr : parent_record->attribute_index) {
        attribute_index[attr.first] = attr.second;
    }
    for (auto method_idx : parent_record->method_index) {
        if (method_index.find(method_idx.first) == method_index.end()) {
            method_index[method_idx.first] = method_idx.second;
        } else {
            method_index[method_idx.first]->set_index(method_idx.second->get_index());
        }
    }
}

void ClassTable::index_attributes_and_methods_rec(Symbol class_name) {
    ClassTableRecord *&class_table_record = class_by_name[class_name];
    if (class_table_record->is_indexed()) {
        return;
    }
    Symbol parent_name = class_table_record->get_class()->get_parent();
    if (parent_name != No_class) {
        index_attributes_and_methods_rec(parent_name);
        ClassTableRecord *&parent_record = class_by_name[parent_name];
        int start_index = parent_record->get_attribute_count() + 3;// space for class tag, object size and dispatch pointer
        class_table_record->copy_features(parent_record);
        class_table_record->index_features(start_index, parent_record->get_method_count());
    } else {
        class_table_record->index_features(0, 0);
    }
}

int ClassTable::get_class_tag(Symbol class_name) {
    return class_by_name[class_name]->get_class_tag();
}

int ClassTable::get_method_offset(Symbol class_name, Symbol method_name) {
    return class_by_name[class_name]->get_method_offset(method_name);
}

void ClassTableRecord::add_method(method_class *method) {
    method_index[method->get_name()] = new MethodRecord(method, cls->get_name());
}


void ClassTable::after(method_class *node) {
    current_class_record->add_method(node);
}

int ClassTableRecord::get_method_offset(Symbol method_name) {
    return method_index[method_name]->get_index();
}
