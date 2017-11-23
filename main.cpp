#include <iostream>
#include "include/multilayer_radix_pq.h"

typedef struct data
{
    int x = 0;
} DATA;

int main() {
    data x;
    multilayer_radix_pq::multilayer_radix_pq<uint64_t, DATA, 3, size_t (pow(2,20))> mlrpq;
    mlrpq.push(256,x);
    mlrpq.pop();
    mlrpq.push(331,x);
    mlrpq.push(pow(2,21),x);
    mlrpq.push(pow(2,20)-128,x);
    std::cout << "Queue empty: " << mlrpq.empty() << std::endl;
    mlrpq.pop();
    mlrpq.pop();
    mlrpq.push(pow(2,22),x);
    mlrpq.push(pow(2,22)+2,x);
    mlrpq.push(pow(2,36),x);
    mlrpq.pop();
    mlrpq.pop();
    mlrpq.pop();
    mlrpq.pop();
    mlrpq.pop();
    mlrpq.pop();
    mlrpq.pop();
    mlrpq.push(pow(2,42), x);
    mlrpq.pop();

    return 0;
}
