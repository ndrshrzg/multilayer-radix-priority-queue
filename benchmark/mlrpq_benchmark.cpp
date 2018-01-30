//
// Created by ndrshrzg on 1/30/18.
//

#include "mlrpq_benchmark.h"
#include "../include/multilayer_radix_pq.h"


int main(){
    typedef uint64_t KEY_TYPE; // later given to wrapper via template parameter

    KEY_TYPE max_key = size_t(1) << 12;
    KEY_TYPE max_segment_size = size_t(1) << 14;
    KEY_TYPE number_of_segments = 200;


    benchmark::NumberGeneratorWrapper<KEY_TYPE, int> wrapper1(10, max_key, max_segment_size, number_of_segments, false);

    auto count = wrapper1.getTotalCount();

    std::cout << "Total count was: " << count << std::endl;

    return 0;
}