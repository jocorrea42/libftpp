#include <iostream>
#include "pool.hpp"

class Foo {
public:
    Foo(int v) : value(v) { std::cout << "Foo constructed " << value << std::endl; }
    ~Foo() { std::cout << "Foo destroyed " << value << std::endl; }
    void show() const { std::cout << "Foo: " << value << std::endl; }
private:
    int value;
};

int main() {
    Pool<Foo> pool;
    pool.resize(2);

    {
        auto o1 = pool.acquire(10);
        o1->show();
        auto o2 = pool.acquire(20);
        o2->show();
        // o1, o2 liberarán sus slots al salir de este scope
    }

    {
        auto o3 = pool.acquire(30);
        o3->show();
    }

    return 0;
}
