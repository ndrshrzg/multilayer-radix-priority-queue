#include <iostream>
#include "include/multilayer_radix_pq.h"

typedef struct data
{
    int x = 0;
} DATA;

int main() {
    data x;
    multilayer_radix_pq::multilayer_radix_pq<uint64_t, DATA, 3> mlrpq(4096);
    mlrpq.push(7, x);
    std::cout << "Queue empty: " << mlrpq.empty() << std::endl;
    mlrpq.push(8, x);
    mlrpq.push(pow(2,7), x);
    mlrpq.push(128, x);
    mlrpq.push(pow(2, 11), x);
    mlrpq.pop();
    mlrpq.pop();
    mlrpq.pop();
    mlrpq.pop();
    mlrpq.push(pow(2, 13), x);
    mlrpq.pop();
    mlrpq.pop();
    std::cout << "Queue empty: " << mlrpq.empty() << std::endl;
    return 0;
}
