//
// Created by ndrshrzg on 1/30/18.
//

#include "mlrpq_benchmark.h"
#include "../include/multilayer_radix_pq.h"

template <size_t size>
struct B{
    char data[size] = {0b00000000};
};
template <size_t size>
struct KiB{
    char data[1024][size] = {0b00000000};
};
template <size_t size>
struct MiB{
    char data[1024][1024][size] = {0b00000000};
};




int main(){
    typedef uint64_t KEY_TYPE; // later given to wrapper via template parameter

    KEY_TYPE max_key = size_t(1) << 31;
    KEY_TYPE max_segment_size = size_t(1) << 1;
    KEY_TYPE number_of_segments = 250;

    // initialize with template params key_type, value_type and constructor params: number of steps,
    // the maximum of all keys, the maximum length of one individual segment, the number of segments,
    //benchmark::allInAllOut<KEY_TYPE, KiB<10>> allInAllOut1(10, max_key, max_segment_size, number_of_segments);

    //benchmark::allInAllOutInterrupted<KEY_TYPE, data<2>> allInAllOutInterrupted1(10, KEY_TYPE(0), max_key, number_of_segments, max_segment_size);

    benchmark::mlrpqOnlyAllInAllOut<KEY_TYPE, B<1>, 3> mlrpqOnlyAllInAllOut1(10, max_key, max_segment_size, number_of_segments);



    return 0;
}