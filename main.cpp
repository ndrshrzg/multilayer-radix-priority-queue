#include <iostream>
#include "include/multilayer_radix_pq.h"

int main() {
    multilayer_radix_pq<int, std::string> mlrq(4096, 8);
    mlrq.testInternal();
    //TODO throws out of range exception
    //mlrq.push(1, "test");

    return 0;
}