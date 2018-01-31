//
// Created by ndrshrzg on 1/30/18.
//

#include "mlrpq_benchmark.h"
#include "../include/multilayer_radix_pq.h"

template <uint64_t size_>
struct data
{
    float large[size_][size_] = {std::numeric_limits<float>::max()};
};



int main(){
    typedef uint64_t KEY_TYPE; // later given to wrapper via template parameter

    KEY_TYPE max_key = size_t(1) << 31;
    KEY_TYPE max_segment_size = size_t(1) << 12;
    KEY_TYPE number_of_segments = 100;

    // initialize with template params key_type, value_type and constructor params: number of steps,
    // the maximum of all keys, the maximum length of one individual segment, the number of segments,
    // and if intermittend pops are to be executed
    benchmark::NumberGeneratorStepper<KEY_TYPE, data<2>> wrapper1(50, max_key, max_segment_size, number_of_segments, false);

    auto count = wrapper1.getTotalCount();

    std::cout << "Total count was: " << count << std::endl;

    return 0;
}