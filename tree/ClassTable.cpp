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

AttributeRecord *ClassTableRecord::get_attribute_record(Symbol s) {
    auto it = attribute_by_name.find(s);
    if (it == attribute_by_name.end()) {
        return nullptr;
    } else {
        return it->second;
    }
}

int ClassTable::get_attr_index(Symbol class_name, Symbol attr_name) {
    do {
        AttributeRecord *attribute_record = class_by_name[class_name]->get_attribute_record(attr_name);
        if (attribute_record != nullptr) {
            return attribute_record->get_index();
        }
        class_name = get_parent(class_name);
    } while (class_name != No_class);
    return -1;
}

void ClassTable::before(class__class *node) {
    class_by_name[node->get_name()] = currentClassRecord = new ClassTableRecord(node, class_tag++);
}

void ClassTable::after(attr_class *node) {
    currentClassRecord->add_attr(node);
}

ClassTableRecord::~ClassTableRecord() {
    for (auto item : attribute_by_name) {
        delete item.second;
    }
}

ClassTable::~ClassTable() {
    for (auto item : class_by_name) {
        delete item.second;
    }
}

void ClassTableRecord::add_attr(attr_class *attr) {
    AttributeRecord *record = new AttributeRecord(attr);
    attribute_by_name[attr->get_name()] = record;
}

int ClassTable::get_prototype_size(Symbol class_name) {
    return class_by_name[class_name]->get_prototype_size();
}

void ClassTable::visit_ordered_attrs_of_class(Symbol class_name, TreeVisitor *other) {
    class_by_name[class_name]->visit_attributes_ordered(other);

}

void ClassTableRecord::visit_attributes_ordered(TreeVisitor *visitor) {
    vector<AttributeRecord *> attr_records;
    for (auto item : attribute_by_name) {
        attr_records.push_back(item.second);
    }
    sort(attr_records.begin(), attr_records.end(), [](AttributeRecord *a, AttributeRecord *b) {
        return a->get_index() < b->get_index();
    });
    for (AttributeRecord *record : attr_records) {
        record->get_attr()->traverse_tree(visitor);
    }
}

void ClassTableRecord::index_attributes(int start_idx) {
    for (auto item : attribute_by_name) {
        item.second->set_index(start_idx++);
    }
    indexed = true;
}

void ClassTable::index_attributes() {
    for (auto item : class_by_name) {
        index_attributes_rec(item.first);
    }
}

void ClassTableRecord::copy_attributes(ClassTableRecord *parent_record) {
    for (auto attr : parent_record->attribute_by_name) {
        attribute_by_name[attr.first] = attr.second;
    }
}

void ClassTable::index_attributes_rec(Symbol class_name) {
    ClassTableRecord *&class_table_record = class_by_name[class_name];
    if (class_table_record->is_indexed()) {
        return;
    }
    Symbol parent_name = class_table_record->get_class()->get_parent();
    if (parent_name != No_class) {
        index_attributes_rec(parent_name);
        ClassTableRecord *&parent_record = class_by_name[parent_name];
        int start_index = parent_record->get_attribute_count() + 3;// space for class tag, object size and dispatch pointer
        class_table_record->index_attributes(start_index);
        class_table_record->copy_attributes(parent_record);
    } else {
        class_table_record->index_attributes(0);
    }


}

int ClassTable::get_class_tag(Symbol class_name) {
    return class_by_name[class_name]->get_class_tag();
}