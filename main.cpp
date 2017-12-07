#include <iostream>
#include <queue>
#include "include/multilayer_radix_pq.h"


typedef struct data
{
    int x = 0;
} DATA;

int main() {

    multilayer_radix_pq::multilayer_radix_pq<uint64_t, DATA, 3> mlrpq;
    DATA val;

    mlrpq.empty();
    mlrpq.push(129, val);
    mlrpq.push(6854985, val);
    mlrpq.empty();
    mlrpq.top();
    mlrpq.pop();
    mlrpq.top();
    mlrpq.pop();
    mlrpq.empty();

}