#include <iostream>
#include "include/multilayer_radix_pq.h"

typedef struct data
{
    int x = 0;
} DATA;

int main() {
    data x;
    multilayer_radix_pq::multilayer_radix_pq<uint64_t, DATA, 4, size_t (pow(2,20))> mlrpq;
    mlrpq.push(167, x);
    mlrpq.push(267, x);
    mlrpq.push(386, x);
    mlrpq.push(856, x);
    mlrpq.push(784, x);
    mlrpq.pop();
    mlrpq.pop();
    mlrpq.pop();
    mlrpq.push(pow(2,27), x);
    mlrpq.push(pow(2,27)+1, x);
    mlrpq.push(pow(2,27)+2, x);
    mlrpq.push(pow(2,27)+3, x);
    mlrpq.push(pow(2,30)+4, x);
    mlrpq.push(pow(2,63), x); // max
    mlrpq.pop();
    mlrpq.pop();
    mlrpq.pop();
    mlrpq.pop();
    mlrpq.pop();
    mlrpq.pop();
    mlrpq.pop();
    mlrpq.pop();
    std::cout << "Queue empty: " << mlrpq.empty() << std::endl;


    return 0;
}
