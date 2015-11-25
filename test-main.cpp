//
// Created by Evgeny Tanhilevich on 22/11/15.
//

#include <iostream>

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


int main() {
    A a;
    B b;
    Test1 test;
    a.test(test);
    b.test(test);
    A& a1 = b;
    a1.test(test);
    return 0;
}