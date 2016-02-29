//
// Created by Evgeny Tanhilevich on 04/01/2016.
//

#ifndef COOL_COMPILER_CLASSTABLE_H
#define COOL_COMPILER_CLASSTABLE_H

#include <map>
#include <vector>
#include "visitor.h"

using std::map;
using std::vector;

class class__class;
class attr_class;

class AttributeRecord {
private:
    attr_class *attr;
    int index;
    friend class ClassTable;
public:
    AttributeRecord(attr_class * _attr): attr(_attr), index(-1) {}
    attr_class* get_attr() {
        return attr;
    }
    int get_index() {
        return index;
    }
    void set_index(int _index) {
        index = _index;
    }
};

class ClassTableRecord {
private:
    Class_ cls;
    int prototype_size;
    map<Symbol, AttributeRecord*> attribute_by_name;//todo use rvalue reference?
    int class_tag;
    bool indexed;
public:
    ClassTableRecord(Class_ _cls, int _class_tag):
            attribute_by_name(), cls(_cls), class_tag(_class_tag), indexed(false) {}
    int get_attribute_count() {
        return (int) attribute_by_name.size();
    }
    int get_prototype_size() {
        return (int) (attribute_by_name.size() + 3); // space for class tag, object size and dispatch pointer
    }
    Class_ get_class() {
        return cls;
    }
    int get_class_tag() {
        return class_tag;
    }
    bool is_indexed() {
        return indexed;
    }
    AttributeRecord* get_attribute_record(Symbol s);
    void add_attr(attr_class *attr);
    void visit_attributes_ordered(TreeVisitor *visitor);
    void index_attributes(int start_idx);
    virtual ~ClassTableRecord();

    void copy_attributes(ClassTableRecord *parent);
};

class ClassTable : public TreeVisitor {
    map<Symbol, ClassTableRecord*> class_by_name;
    ClassTableRecord* currentClassRecord;
    void index_attributes_rec(Symbol class_name);
    int class_tag;
public:
    virtual ~ClassTable();
    ClassTable() : class_by_name(), currentClassRecord(nullptr), class_tag(0) {}
    Class_ get_class(Symbol s);
    Symbol get_parent(Symbol class_name);
    bool is_subtype(Symbol sub, Symbol super);
    int get_attr_index(Symbol class_name, Symbol attr_name);
    int get_prototype_size(Symbol class_name);

    Symbol join(const vector<Symbol> &type1);

    void before(class__class *node);
    void after(attr_class *node);

    void visit_ordered_attrs_of_class(Symbol class_name, TreeVisitor *other);

    void index_attributes();

    int get_class_tag(Symbol class_name);

};


#endif //COOL_COMPILER_CLASSTABLE_H
