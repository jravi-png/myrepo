#include <iostream>

class Base {
public:
    virtual int func() { return 0; }
};

class Derived : public Base {
public:
    int func() override { return 42; }
};

int main() {
    Base* base = new Derived();
    int result = ((Derived*)base)->func(); // Redundant type cast
    std::cout << "Result: " << result << std::endl;
    delete base;
    return 0;
}