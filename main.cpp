#include <iostream>
#include "include/multilayer_radix_pq.h"

typedef struct data
{
    int x = 0;
} DATA;

int main() {
    data x;
    multilayer_radix_pq::multilayer_radix_pq<uint64_t, DATA, 3> mlrpq(4096);
    mlrpq.push(128, x);
    mlrpq.push(129, x);

    return 0;
}
