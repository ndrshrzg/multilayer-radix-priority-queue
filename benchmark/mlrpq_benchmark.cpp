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

    KEY_TYPE max_key = size_t(1) << 14;
    KEY_TYPE max_segment_size = size_t(1) << 14;
    KEY_TYPE number_of_segments = 250;

    // initialize with template params key_type, value_type and constructor params: number of steps,
    // the maximum of all keys, the maximum length of one individual segment, the number of segments,
    benchmark::allInAllOut<KEY_TYPE, data<2>> allInAllOut1(2, max_key, max_segment_size, number_of_segments);

    benchmark::allInAllOutInterrupted<KEY_TYPE, data<2>> allInAllOutInterrupted1(2, KEY_TYPE(0), max_key, number_of_segments, max_segment_size);



    return 0;
}