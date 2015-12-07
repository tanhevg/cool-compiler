//
// Created by Evgeny Tanhilevich on 22/11/15.
//

#include <iostream>
#include <map>
#include <utility>
#include <string>
#include <vector>


using std::endl;
using std::cout;

class A;
class B;

class Test {
public:
    virtual void test(A*) { cout << "Test A" << endl; }
    virtual void test(B*) { cout << "Test B" << endl; }
};

class Test1: public Test {
public:
    virtual void test(A*) override { cout << "Test1 A" << endl; }
    virtual void test(B*) override { cout << "Test1 B" << endl; }
};


class A {
public:
    virtual void test(Test& test) { cout << "A "; test.test(this); }
};

class B: public A {
public:
    virtual void test(Test& test) override { cout << "B "; test.test(this); }
};

class C {
protected:
    char *c;
public:
    C(char _c) {
        c = new char[2];
        c[0] = _c;
        c[1] = '\0';
    }
    ~C() {
        delete c;
    }
};

typedef C* CC;

using namespace std;

class RefTest {
private:
    int val;
public:
    RefTest(int _val): val(_val) { cout << "Default ctor " << val << endl; }
    RefTest(const RefTest& _ref_test): val(_ref_test.val) {cout << "Copy ctor" << endl;}
    RefTest(RefTest&& _ref_test): val(_ref_test.val) {_ref_test.val = 0; cout << "Move ctor" << endl;}
    int get_val() { return val;}
};

int main() {
//    A a;
//    B b;
//    Test1 test;
//    a.test(test);
//    b.test(test);
//    A& a1 = b;
//    a1.test(test);
//    return 0;

//    map<pair<string, string>, pair<CC, CC>> m;
//    m[pair<string, string>("a", "b")] = pair<CC, CC>(new C('A'), new C('B'));
//    cout << m[pair<string, string>("a", "b")].first << endl;
//    cout << (m[pair<string, string>("a", "c")].first == NULL) << endl;
    RefTest rt1(42);
    vector<unique_ptr<RefTest>> v;
    v.push_back(make_unique<RefTest>(rt1));
    cout << rt1.get_val() << endl;
}