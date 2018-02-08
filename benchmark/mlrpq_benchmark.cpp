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

std::ostream& operator<< (std::ostream& o, const std::pair<uint64_t, B<25>>& d) {
    o << d.first;
    return o;
};

std::ostream& operator<< (std::ostream& o, const std::pair<uint64_t, B<50>>& d) {
    o << d.first;
    return o;
};

std::ostream& operator<< (std::ostream& o, const std::pair<uint64_t, B<100>>& d) {
    o << d.first;
    return o;
};

std::ostream& operator<< (std::ostream& o, const std::pair<uint64_t, B<250>>& d) {
    o << d.first;
    return o;
};

std::ostream& operator<< (std::ostream& o, const std::pair<uint64_t, B<500>>& d) {
    o << d.first;
    return o;
};

std::ostream& operator<< (std::ostream& o, const std::pair<uint64_t, KiB<1>>& d) {
    o << d.first;
    return o;
};

template <typename KeyType, typename ValueType>
void benchmarkMLRPQ(int runs, KeyType max_key, KeyType number_of_segments, KeyType max_segment_size) {
    // what i want to do: call this function with parameters to be passed to constructors, so i can automate e.g.
    // increasing datasize

    std::cout << "Data size: " << sizeof(ValueType) << "B" << std::endl;
    std::cout << "Upper limit for key: " << max_key * number_of_segments << std::endl;

    std::cout << "Setting radix: 1" << std::endl;
    benchmark::mlrpqOnlyAllInAllOut<KeyType, ValueType, 1> mlrpqOnlyAllInAllOut1(runs, max_key, number_of_segments, max_segment_size);

    std::cout << "Setting radix: 2" << std::endl;
    benchmark::mlrpqOnlyAllInAllOut<KeyType, ValueType, 2> mlrpqOnlyAllInAllOut2(runs, max_key, number_of_segments, max_segment_size);

    std::cout << "Setting radix: 3" << std::endl;
    benchmark::mlrpqOnlyAllInAllOut<KeyType, ValueType, 3> mlrpqOnlyAllInAllOut3(runs, max_key, number_of_segments, max_segment_size);

    std::cout << "Setting radix: 4" << std::endl;
    benchmark::mlrpqOnlyAllInAllOut<KeyType, ValueType, 4> mlrpqOnlyAllInAllOut4(runs, max_key, number_of_segments, max_segment_size);

    std::cout << "Setting radix: 5" << std::endl;
    benchmark::mlrpqOnlyAllInAllOut<KeyType, ValueType, 5> mlrpqOnlyAllInAllOut5(runs, max_key, number_of_segments, max_segment_size);


    // print out upper estimate of numbers

    // print resulting time as #numbers/microseconds so i get the measure "operations per ms"

}

int main(){
    typedef uint64_t KEY_TYPE;

    int runs = 3;

    KEY_TYPE min_key = size_t(0);
    KEY_TYPE max_key = size_t(1) << 18;
    KEY_TYPE max_segment_size = size_t(1) << 8;
    KEY_TYPE number_of_segments = size_t(1) << 8;
    /*
    benchmark::allInAllOut<KEY_TYPE, B<1>> allInAllOut1(3, max_key, max_segment_size, number_of_segments);


    benchmark::allInAllOutInterrupted<KEY_TYPE, B<250>> allInAllOutInterrupted1(3, KEY_TYPE(0), max_key, number_of_segments, max_segment_size);
    */
    benchmarkMLRPQ<KEY_TYPE, B<1>>(runs, max_key, number_of_segments, max_segment_size);
    benchmarkMLRPQ<KEY_TYPE, B<25>>(runs, max_key, number_of_segments, max_segment_size);
    benchmarkMLRPQ<KEY_TYPE, B<50>>(runs, max_key, number_of_segments, max_segment_size);
    benchmarkMLRPQ<KEY_TYPE, B<100>>(runs, max_key, number_of_segments, max_segment_size);
    benchmarkMLRPQ<KEY_TYPE, B<250>>(runs, max_key, number_of_segments, max_segment_size);
    benchmarkMLRPQ<KEY_TYPE, B<500>>(runs, max_key, number_of_segments, max_segment_size);
    benchmarkMLRPQ<KEY_TYPE, KiB<1>>(runs, max_key, number_of_segments, max_segment_size);
    /*

    benchmark::mlrpqOnlyAllInAllOutInterrupted<KEY_TYPE, B<1>, 3>
            mlrpqOnlyAllInAllOutInterrupted(3, min_key, max_key, number_of_segments, max_segment_size);
    */
    return 0;
}

void comparativeBenchmarks(){


}


