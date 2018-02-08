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

std::ostream& operator<< (std::ostream& o, const std::pair<uint64_t, B<1>>& d) {
    o << d.first;
    return o;
};


int main(){
    typedef uint64_t KEY_TYPE;

    KEY_TYPE min_key = 0;
    KEY_TYPE max_key = size_t(1) << 12;
    KEY_TYPE max_segment_size = size_t(1) << 18;
    KEY_TYPE number_of_segments = 50;

    benchmark::allInAllOut<KEY_TYPE, B<1>> allInAllOut1(3, max_key, max_segment_size, number_of_segments);

    benchmark::allInAllOutInterrupted<KEY_TYPE, B<1>> allInAllOutInterrupted1(3, KEY_TYPE(0), max_key, number_of_segments, max_segment_size);

    benchmark::mlrpqOnlyAllInAllOut<KEY_TYPE, B<1>, 3>
            mlrpqOnlyAllInAllOut1(3, max_key, number_of_segments, max_segment_size);

    benchmark::mlrpqOnlyAllInAllOutInterrupted<KEY_TYPE, B<1>, 3>
            mlrpqOnlyAllInAllOutInterrupted(3, min_key, max_key, number_of_segments, max_segment_size);

    return 0;
}