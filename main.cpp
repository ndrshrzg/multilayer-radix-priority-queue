#include <iostream>
#include "include/multilayer_radix_pq.h"

typedef struct data
{
    int x = 0;
} DATA;

int main() {
    data x;
    multilayer_radix_pq::multilayer_radix_pq<uint64_t, DATA, 3> mlrpq(4096);
    mlrpq.push(8, x);
    mlrpq.push(pow(2,7), x);
    mlrpq.push(9087, x);
    mlrpq.push(pow(2,11) + pow(2,9), x);
    std::cout << "Is empty: " << mlrpq.empty() << std::endl;
    std::cout << "First non empty Bucket: " << mlrpq.top().first << ", " << mlrpq.top().second << std::endl;
    return 0;
}
