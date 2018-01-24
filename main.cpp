//
// Created by Andreas Herzog on 12/07/17.
// andreas.herzog@stud.uni-frankfurt.de
//

#include "include/multilayer_radix_pq.h"
#include <iostream>

typedef struct data
{
    int x = 0;
} DATA;

int main() {

    multilayer_radix_pq::multilayer_radix_pq<uint32_t, DATA, 3> mlrpq;
    DATA val;
    mlrpq.push(0, val);
    mlrpq.push(128, val);
    std::cout << mlrpq.top().first << std::endl;
    mlrpq.pop();
    std::cout << mlrpq.top().first << std::endl;
    mlrpq.push(132, val);
    mlrpq.pop();
    std::cout << mlrpq.top().first << std::endl;
    mlrpq.pop();

    std::cout << mlrpq.empty() << std::endl;

}
