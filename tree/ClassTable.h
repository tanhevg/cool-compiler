//
// Created by Evgeny Tanhilevich on 04/01/2016.
//

#ifndef COOL_COMPILER_CLASSTABLE_H
#define COOL_COMPILER_CLASSTABLE_H

#include <map>
#include <vector>
#include <algorithm>
#include <functional>
#include "visitor.h"

using std::map;
using std::vector;
using std::sort;
using std::function;

class class__class;
class attr_class;

template<class T>
class IndexedRecord {
private:
    T *ref;
    int index;
public:
    IndexedRecord(T *_ref): ref(_ref), index(-1) {}
    T* get_ref() {
        return ref;
    }
    int get_index() {
        return index;
    }
    void set_index(int _index) {
        index = _index;
    }
};

//todo don't sort; rely on natural ordering

typedef IndexedRecord<attr_class> AttrRecord;

class MethodRecord: public IndexedRecord<method_class> {
private:
    Symbol declaring_class;
public:
    MethodRecord(method_class *_ref, Symbol _declaring_class) :
            IndexedRecord(_ref), declaring_class(_declaring_class) { }
    Symbol get_declaring_class() {
        return declaring_class;
    }
};

class ClassTableRecord {
private:
    Class_ cls;
    int prototype_size;
    map<Symbol, IndexedRecord<attr_class> *> attribute_index;//todo use rvalue reference?
    map<Symbol, MethodRecord *> method_index;
    int class_tag;
    bool indexed;
    template <class T> void visit_ordered(map<Symbol, T *> &p_map, function<void(T*)> lambda) {
        vector<T *> records;
        for (auto item : p_map) {
            records.push_back(item.second);
        }
        sort(records.begin(), records.end(), [](auto a, auto b) {
            return a->get_index() < b->get_index();
        });
        for (auto record : records) {
            lambda(record);
        }

    }


public:
    ClassTableRecord(Class_ _cls, int _class_tag):
            attribute_index(), method_index(), cls(_cls), class_tag(_class_tag), indexed(false) {}
    int get_attribute_count() {
        return (int) attribute_index.size();
    }
    int get_method_count() {
        return (int)method_index.size();
    }
    int get_prototype_size() {
        return (int) (attribute_index.size() + 3); // space for class tag, object size and dispatch pointer
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
    void add_attr(attr_class *attr);
    void index_features(int start_attr_idx, int start_method_idx);
    void add_method(method_class *method);
    virtual ~ClassTableRecord();

    void copy_features(ClassTableRecord *parent);
    int get_method_offset(Symbol method_name);

    void visit_attributes_ordered(function<void(AttrRecord *)> lambda) {
        visit_ordered(attribute_index, lambda);
    }
    void visit_methods_ordered(function<void(MethodRecord *)> lambda) {
        visit_ordered(method_index, lambda);
    }
};

class ClassTable : public TreeVisitor {
    map<Symbol, ClassTableRecord*> class_by_name;
    vector<ClassTableRecord*> class_by_tag;
    ClassTableRecord* current_class_record;
    void index_attributes_and_methods_rec(Symbol class_name);
    int class_tag;
public:
    virtual ~ClassTable();
    ClassTable() : class_by_name(), current_class_record(nullptr), class_tag(1) {}
    Class_ get_class(Symbol s);
    Symbol get_parent(Symbol class_name);
    bool is_subtype(Symbol sub, Symbol super);
    int get_prototype_size(Symbol class_name);

    Symbol join(const vector<Symbol> &type1);

    void before(class__class *node);
    void after(attr_class *node);
    void after(method_class *node);


    void index();

    int get_class_tag(Symbol class_name);

    int get_method_offset(Symbol class_name, Symbol method_name);

    void visit_ordered_attrs_of_class(Symbol class_name, function<void(AttrRecord *)> lambda) {
        class_by_name[class_name]->visit_attributes_ordered(lambda);
    }

    void visit_ordered_methods_of_class(Symbol class_name, function<void(MethodRecord *)> lambda) {
        class_by_name[class_name]->visit_methods_ordered(lambda);
    }

    void visit_classes_ordered_by_tag(function<void(Class_)> lambda) {
//        vector<ClassTableRecord *> records;
//        for (auto r: class_by_name) {
//            records.push_back(r.second);
//        }
//        sort(records.begin(), records.end(), [](auto a, auto b) {
//            return a->get_class_tag() < b->get_class_tag();
//        });
        for (auto r: class_by_tag) {
            lambda(r->get_class());
        }
    }
};


#endif //COOL_COMPILER_CLASSTABLE_H
