//
// Created by ndrshrzg on 1/30/18.
//

#include "mlrpq_benchmark.h"
#include "../include/multilayer_radix_pq.h"

template <int size>
struct B{
    char data[size] = {0b00000000};
};


std::ostream& operator<< (std::ostream& o, const std::pair<uint64_t, B<1>>& d) {
    o << d.first;
    return o;
};

std::ostream& operator<< (std::ostream& o, const std::pair<uint64_t, B<16>>& d) {
    o << d.first;
    return o;
};

std::ostream& operator<< (std::ostream& o, const std::pair<uint64_t, B<64>>& d) {
    o << d.first;
    return o;
};

std::ostream& operator<< (std::ostream& o, const std::pair<uint64_t, B<128>>& d) {
    o << d.first;
    return o;
};

std::ostream& operator<< (std::ostream& o, const std::pair<uint64_t, B<256>>& d) {
    o << d.first;
    return o;
};

std::ostream& operator<< (std::ostream& o, const std::pair<uint64_t, B<512>>& d) {
    o << d.first;
    return o;
};

std::ostream& operator<< (std::ostream& o, const std::pair<uint64_t, B<1024>>& d) {
    o << d.first;
    return o;
};

template <typename KeyType, typename ValueType>
void benchmarkMLRPQ(int runs, KeyType min_key, KeyType max_key, KeyType number_of_segments, KeyType max_segment_size, int radix) {
    if (radix == 1){
        benchmark::mlrpqOnlyAllInAllOut<KeyType, ValueType, 1> mlrpqAIAO(runs, max_key, number_of_segments, max_segment_size);
        benchmark::mlrpqOnlyAllInAllOutInterrupted<KeyType , ValueType, 1> mlrpqOnlyInterrutped(runs, min_key, max_key, number_of_segments, max_segment_size);
    }
    if (radix == 2){
        benchmark::mlrpqOnlyAllInAllOut<KeyType, ValueType, 2> mlrpqAIAO(runs, max_key, number_of_segments, max_segment_size);
        benchmark::mlrpqOnlyAllInAllOutInterrupted<KeyType , ValueType, 2> mlrpqOnlyInterrutped(runs, min_key, max_key, number_of_segments, max_segment_size);
    }
    if (radix == 3){
        benchmark::mlrpqOnlyAllInAllOut<KeyType, ValueType, 3> mlrpqAIAO(runs, max_key, number_of_segments, max_segment_size);
        benchmark::mlrpqOnlyAllInAllOutInterrupted<KeyType , ValueType, 3> mlrpqOnlyInterrutped(runs, min_key, max_key, number_of_segments, max_segment_size);
    }
    if (radix == 4){
        benchmark::mlrpqOnlyAllInAllOut<KeyType, ValueType, 4> mlrpqAIAO(runs, max_key, number_of_segments, max_segment_size);
        benchmark::mlrpqOnlyAllInAllOutInterrupted<KeyType , ValueType, 4> mlrpqOnlyInterrutped(runs, min_key, max_key, number_of_segments, max_segment_size);
    }
    if (radix == 5){
        benchmark::mlrpqOnlyAllInAllOut<KeyType, ValueType, 5> mlrpqAIAO(runs, max_key, number_of_segments, max_segment_size);
        benchmark::mlrpqOnlyAllInAllOutInterrupted<KeyType , ValueType, 5> mlrpqOnlyInterrutped(runs, min_key, max_key, number_of_segments, max_segment_size);
    }
    if (radix == 6){
        benchmark::mlrpqOnlyAllInAllOut<KeyType, ValueType, 6> mlrpqAIAO(runs, max_key, number_of_segments, max_segment_size);
        benchmark::mlrpqOnlyAllInAllOutInterrupted<KeyType , ValueType, 6> mlrpqOnlyInterrutped(runs, min_key, max_key, number_of_segments, max_segment_size);
    }
    if (radix == 7){
        benchmark::mlrpqOnlyAllInAllOut<KeyType, ValueType, 7> mlrpqAIAO(runs, max_key, number_of_segments, max_segment_size);
        benchmark::mlrpqOnlyAllInAllOutInterrupted<KeyType , ValueType, 7> mlrpqOnlyInterrutped(runs, min_key, max_key, number_of_segments, max_segment_size);
    }
};

template <typename KeyType, typename ValueType>
void benchmarkCompetitive(int runs, KeyType min_key, KeyType max_key, KeyType number_of_segments, KeyType max_segment_size, int radix){
    if (radix == 1){
        benchmark::allInAllOut<KeyType, ValueType, 1> AIAO(runs, min_key, max_key, number_of_segments, max_segment_size);
        benchmark::allInAllOutInterrupted<KeyType, ValueType, 1> AIAOInterrupted(runs, min_key, max_key, number_of_segments, max_segment_size);
    }
    if (radix == 2){
        benchmark::allInAllOut<KeyType, ValueType, 2> AIAO(runs, min_key, max_key, number_of_segments, max_segment_size);
        benchmark::allInAllOutInterrupted<KeyType, ValueType, 2> AIAOInterrupted(runs, min_key, max_key, number_of_segments, max_segment_size);
    }
    if (radix == 3){
        benchmark::allInAllOut<KeyType, ValueType, 3> AIAO(runs, min_key, max_key, number_of_segments, max_segment_size);
        benchmark::allInAllOutInterrupted<KeyType, ValueType, 3> AIAOInterrupted(runs, min_key, max_key, number_of_segments, max_segment_size);
    }
    if (radix == 4){
        benchmark::allInAllOut<KeyType, ValueType, 4> AIAO(runs, min_key, max_key, number_of_segments, max_segment_size);
        benchmark::allInAllOutInterrupted<KeyType, ValueType, 4> AIAOInterrupted(runs, min_key, max_key, number_of_segments, max_segment_size);
    }
    if (radix == 5){
        benchmark::allInAllOut<KeyType, ValueType, 5> AIAO(runs, min_key, max_key, number_of_segments, max_segment_size);
        benchmark::allInAllOutInterrupted<KeyType, ValueType, 5> AIAOInterrupted(runs, min_key, max_key, number_of_segments, max_segment_size);
    }
    if (radix == 6){
        benchmark::allInAllOut<KeyType, ValueType, 6> AIAO(runs, min_key, max_key, number_of_segments, max_segment_size);
        benchmark::allInAllOutInterrupted<KeyType, ValueType, 6> AIAOInterrupted(runs, min_key, max_key, number_of_segments, max_segment_size);
    }
    if (radix == 7){
        benchmark::allInAllOut<KeyType, ValueType, 7> AIAO(runs, min_key, max_key, number_of_segments, max_segment_size);
        benchmark::allInAllOutInterrupted<KeyType, ValueType, 7> AIAOInterrupted(runs, min_key, max_key, number_of_segments, max_segment_size);
    }
};


int main(int argc, char *argv[]){
    typedef uint64_t KEY_TYPE;

    int runs = atoi(argv[1]);

    int max_key_bits = atoi(argv[3]);
    int max_segment_size_bits = atoi(argv[4]);
    int number_of_segments_bits = atoi(argv[5]);

    KEY_TYPE min_key = ((atoi(argv[2]) == 0) ? 0 : (size_t(1) << (atoi(argv[2]))));
    KEY_TYPE max_key = size_t(1) << max_key_bits;
    KEY_TYPE max_segment_size = size_t(1) << max_segment_size_bits;
    KEY_TYPE number_of_segments = size_t(1) << number_of_segments_bits;


    std::string choose_style = argv[6];
    std::string choose_size = argv[7];

    int radix = atoi(argv[8]);

    if (choose_style == "single"){
        if (choose_size == "tiny"){
            std::cout << "data_size;1" << std::endl;
            benchmarkMLRPQ<KEY_TYPE, B<1>>(runs, min_key, max_key, number_of_segments, max_segment_size, radix);
        }

        if (choose_size == "small"){
            std::cout << "data_size;1" << std::endl;
            benchmarkMLRPQ<KEY_TYPE, B<1>>(runs, min_key, max_key, number_of_segments, max_segment_size, radix);
            std::cout << "data_size;16" << std::endl;
            benchmarkMLRPQ<KEY_TYPE, B<16>>(runs, min_key, max_key, number_of_segments, max_segment_size, radix);
            std::cout << "data_size;64" << std::endl;
            benchmarkMLRPQ<KEY_TYPE, B<64>>(runs, min_key, max_key, number_of_segments, max_segment_size, radix);
            std::cout << "data_size;128" << std::endl;
            benchmarkMLRPQ<KEY_TYPE, B<128>>(runs, min_key, max_key, number_of_segments, max_segment_size, radix);
        }
        if (choose_size == "large"){
            std::cout << "data_size;256" << std::endl;
            benchmarkMLRPQ<KEY_TYPE, B<256>>(runs, min_key, max_key, number_of_segments, max_segment_size, radix);
            std::cout << "data_size;512" << std::endl;
            benchmarkMLRPQ<KEY_TYPE, B<512>>(runs, min_key, max_key, number_of_segments, max_segment_size, radix);
            std::cout << "data_size;1024" << std::endl;
            benchmarkMLRPQ<KEY_TYPE, B<1024>>(runs, min_key, max_key, number_of_segments, max_segment_size, radix);
        }

    }

    if (choose_style == "competitive") {
        if (choose_size == "small") {
            std::cout << "data_size;1" << std::endl;
            benchmarkCompetitive<KEY_TYPE, B<1>>(runs, min_key, max_key, number_of_segments, max_segment_size, radix);
            std::cout << "data_size;16" << std::endl;
            benchmarkCompetitive<KEY_TYPE, B<16>>(runs, min_key, max_key, number_of_segments, max_segment_size, radix);
            std::cout << "data_size;64" << std::endl;
            benchmarkCompetitive<KEY_TYPE, B<64>>(runs, min_key, max_key, number_of_segments, max_segment_size, radix);
            std::cout << "data_size;128" << std::endl;
            benchmarkCompetitive<KEY_TYPE, B<128>>(runs, min_key, max_key, number_of_segments, max_segment_size, radix);
        }

        if (choose_size == "large") {
            std::cout << "data_size;256" << std::endl;
            benchmarkCompetitive<KEY_TYPE, B<256>>(runs, min_key, max_key, number_of_segments, max_segment_size, radix);
            std::cout << "data_size;512" << std::endl;
            benchmarkCompetitive<KEY_TYPE, B<512>>(runs, min_key, max_key, number_of_segments, max_segment_size, radix);
            std::cout << "data_size;1024" << std::endl;
            benchmarkCompetitive<KEY_TYPE, B<1024>>(runs, min_key, max_key, number_of_segments, max_segment_size, radix);
        }
    }

    return 0;
}
