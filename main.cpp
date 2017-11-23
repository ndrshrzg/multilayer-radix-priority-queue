#include <iostream>
#include "include/multilayer_radix_pq.h"

typedef struct data
{
    int x = 0;
} DATA;

int main() {
    data x;
    multilayer_radix_pq::multilayer_radix_pq<uint64_t, DATA, 3, 4096> mlrpq;
    mlrpq.push(128,x);
    mlrpq.pop();
    mlrpq.push(131,x);
    std::cout << "Queue empty: " << mlrpq.empty() << std::endl;
    return 0;
}
