#include <iostream>
#include "include/multilayer_radix_pq.h"

int main() {

    multilayer_radix_pq<uint64_t, std::string, 8> mlrq;
    mlrq.push(1, "test");
    mlrq.push(53, "test");
    mlrq.push(871, "test");
    std::cout << "Is empty: " << mlrq.empty() << std::endl;
    return 0;
}