#include "../src/log.h"

class A {
public :
    void test(int, int) {
        log();
    }
};

int main() {
    A a;
    a.test(0, 1);

    return 0;
}
