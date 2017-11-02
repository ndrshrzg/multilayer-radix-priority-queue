#include <iostream>
#include "include/multilayer_radix_pq.h"

typedef struct data
{
    int x;
} DATA;

int main() {
    data x;
    //multilayer_radix_pq<uint64_t, std::string, 8> mlrq;
    multilayer_radix_pq::multilayer_radix_pq<uint64_t, DATA> mlrq(8, 4096);
    mlrq.push(pow(2,17)+pow(2,10)+pow(2,3), x);
    mlrq.push(11, x);
    mlrq.push(11, x);
    // Calls resulting in Blocks > 16 not allowed for now
    //mlrq.push(pow(2,pow(2,16)), x);
    std::cout << "Is empty: " << mlrq.empty() << std::endl;
    return 0;
}
