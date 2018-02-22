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
void benchmarkMLRPQ(int run, KeyType min_key, KeyType max_key, KeyType number_of_segments, KeyType max_segment_size, int radix) {
    if (radix == 1){
        benchmark::mlrpqOnlyAllInAllOut<KeyType, ValueType, 1> mlrpqAIAO(run, max_key, number_of_segments, max_segment_size);
        benchmark::mlrpqOnlyAllInAllOutInterrupted<KeyType , ValueType, 1> mlrpqOnlyInterrutped(run, min_key, max_key, number_of_segments, max_segment_size);
    }
    if (radix == 2){
        benchmark::mlrpqOnlyAllInAllOut<KeyType, ValueType, 2> mlrpqAIAO(run, max_key, number_of_segments, max_segment_size);
        benchmark::mlrpqOnlyAllInAllOutInterrupted<KeyType , ValueType, 2> mlrpqOnlyInterrutped(run, min_key, max_key, number_of_segments, max_segment_size);
    }
    if (radix == 3){
        benchmark::mlrpqOnlyAllInAllOut<KeyType, ValueType, 3> mlrpqAIAO(run, max_key, number_of_segments, max_segment_size);
        benchmark::mlrpqOnlyAllInAllOutInterrupted<KeyType , ValueType, 3> mlrpqOnlyInterrutped(run, min_key, max_key, number_of_segments, max_segment_size);
    }
    if (radix == 4){
        benchmark::mlrpqOnlyAllInAllOut<KeyType, ValueType, 4> mlrpqAIAO(run, max_key, number_of_segments, max_segment_size);
        benchmark::mlrpqOnlyAllInAllOutInterrupted<KeyType , ValueType, 4> mlrpqOnlyInterrutped(run, min_key, max_key, number_of_segments, max_segment_size);
    }
    if (radix == 5){
        benchmark::mlrpqOnlyAllInAllOut<KeyType, ValueType, 5> mlrpqAIAO(run, max_key, number_of_segments, max_segment_size);
        benchmark::mlrpqOnlyAllInAllOutInterrupted<KeyType , ValueType, 5> mlrpqOnlyInterrutped(run, min_key, max_key, number_of_segments, max_segment_size);
    }
    if (radix == 6){
        benchmark::mlrpqOnlyAllInAllOut<KeyType, ValueType, 6> mlrpqAIAO(run, max_key, number_of_segments, max_segment_size);
        benchmark::mlrpqOnlyAllInAllOutInterrupted<KeyType , ValueType, 6> mlrpqOnlyInterrutped(run, min_key, max_key, number_of_segments, max_segment_size);
    }
    if (radix == 7){
        benchmark::mlrpqOnlyAllInAllOut<KeyType, ValueType, 7> mlrpqAIAO(run, max_key, number_of_segments, max_segment_size);
        benchmark::mlrpqOnlyAllInAllOutInterrupted<KeyType , ValueType, 7> mlrpqOnlyInterrutped(run, min_key, max_key, number_of_segments, max_segment_size);
    }
    if (radix == 8){
        benchmark::mlrpqOnlyAllInAllOut<KeyType, ValueType, 8> mlrpqAIAO(run, max_key, number_of_segments, max_segment_size);
        benchmark::mlrpqOnlyAllInAllOutInterrupted<KeyType , ValueType, 8> mlrpqOnlyInterrutped(run, min_key, max_key, number_of_segments, max_segment_size);
    }
    if (radix == 9){
        benchmark::mlrpqOnlyAllInAllOut<KeyType, ValueType, 9> mlrpqAIAO(run, max_key, number_of_segments, max_segment_size);
        benchmark::mlrpqOnlyAllInAllOutInterrupted<KeyType , ValueType, 9> mlrpqOnlyInterrutped(run, min_key, max_key, number_of_segments, max_segment_size);
    }
    if (radix == 10){
        benchmark::mlrpqOnlyAllInAllOut<KeyType, ValueType, 10> mlrpqAIAO(run, max_key, number_of_segments, max_segment_size);
        benchmark::mlrpqOnlyAllInAllOutInterrupted<KeyType , ValueType, 10> mlrpqOnlyInterrutped(run, min_key, max_key, number_of_segments, max_segment_size);
    }
};

template <typename KeyType, typename ValueType>
void benchmarkCompetitive(int run, KeyType min_key, KeyType max_key, KeyType number_of_segments, KeyType max_segment_size, int radix){
    if (radix == 1){
        benchmark::allInAllOut<KeyType, ValueType, 1> AIAO(run, min_key, max_key, number_of_segments, max_segment_size);
        benchmark::allInAllOutInterrupted<KeyType, ValueType, 1> AIAOInterrupted(run, min_key, max_key, number_of_segments, max_segment_size);
    }
    if (radix == 2){
        benchmark::allInAllOut<KeyType, ValueType, 2> AIAO(run, min_key, max_key, number_of_segments, max_segment_size);
        benchmark::allInAllOutInterrupted<KeyType, ValueType, 2> AIAOInterrupted(run, min_key, max_key, number_of_segments, max_segment_size);
    }
    if (radix == 3){
        benchmark::allInAllOut<KeyType, ValueType, 3> AIAO(run, min_key, max_key, number_of_segments, max_segment_size);
        benchmark::allInAllOutInterrupted<KeyType, ValueType, 3> AIAOInterrupted(run, min_key, max_key, number_of_segments, max_segment_size);
    }
    if (radix == 4){
        benchmark::allInAllOut<KeyType, ValueType, 4> AIAO(run, min_key, max_key, number_of_segments, max_segment_size);
        benchmark::allInAllOutInterrupted<KeyType, ValueType, 4> AIAOInterrupted(run, min_key, max_key, number_of_segments, max_segment_size);
    }
    if (radix == 5){
        benchmark::allInAllOut<KeyType, ValueType, 5> AIAO(run, min_key, max_key, number_of_segments, max_segment_size);
        benchmark::allInAllOutInterrupted<KeyType, ValueType, 5> AIAOInterrupted(run, min_key, max_key, number_of_segments, max_segment_size);
    }
    if (radix == 6){
        benchmark::allInAllOut<KeyType, ValueType, 6> AIAO(run, min_key, max_key, number_of_segments, max_segment_size);
        benchmark::allInAllOutInterrupted<KeyType, ValueType, 6> AIAOInterrupted(run, min_key, max_key, number_of_segments, max_segment_size);
    }
    if (radix == 7){
        benchmark::allInAllOut<KeyType, ValueType, 7> AIAO(run, min_key, max_key, number_of_segments, max_segment_size);
        benchmark::allInAllOutInterrupted<KeyType, ValueType, 7> AIAOInterrupted(run, min_key, max_key, number_of_segments, max_segment_size);
    }
    if (radix == 8){
        benchmark::allInAllOut<KeyType, ValueType, 8> AIAO(run, min_key, max_key, number_of_segments, max_segment_size);
        benchmark::allInAllOutInterrupted<KeyType, ValueType, 8> AIAOInterrupted(run, min_key, max_key, number_of_segments, max_segment_size);
    }
    if (radix == 9){
        benchmark::allInAllOut<KeyType, ValueType, 9> AIAO(run, min_key, max_key, number_of_segments, max_segment_size);
        benchmark::allInAllOutInterrupted<KeyType, ValueType, 9> AIAOInterrupted(run, min_key, max_key, number_of_segments, max_segment_size);
    }
    if (radix == 10){
        benchmark::allInAllOut<KeyType, ValueType, 10> AIAO(run, min_key, max_key, number_of_segments, max_segment_size);
        benchmark::allInAllOutInterrupted<KeyType, ValueType, 10> AIAOInterrupted(run, min_key, max_key, number_of_segments, max_segment_size);
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
            for (int i = 1; i <= runs; i++){
                std::cout << "data_size;1" << std::endl;
                benchmarkMLRPQ<KEY_TYPE, B<1>>(i, min_key, max_key, number_of_segments, max_segment_size, radix);
            }

        }

        if (choose_size == "small"){
            for (int i = 1; i <= runs; i++){
                std::cout << "data_size;1" << std::endl;
                benchmarkMLRPQ<KEY_TYPE, B<1>>(i, min_key, max_key, number_of_segments, max_segment_size, radix);
                std::cout << "data_size;16" << std::endl;
                benchmarkMLRPQ<KEY_TYPE, B<16>>(i, min_key, max_key, number_of_segments, max_segment_size, radix);
                std::cout << "data_size;64" << std::endl;
                benchmarkMLRPQ<KEY_TYPE, B<64>>(i, min_key, max_key, number_of_segments, max_segment_size, radix);
                std::cout << "data_size;128" << std::endl;
                benchmarkMLRPQ<KEY_TYPE, B<128>>(i, min_key, max_key, number_of_segments, max_segment_size, radix);
            }
        }    
        
        if (choose_size == "large"){
            for (int i = 1; i <= runs; i++){
                std::cout << "data_size;256" << std::endl;
                benchmarkMLRPQ<KEY_TYPE, B<256>>(i, min_key, max_key, number_of_segments, max_segment_size, radix);
                std::cout << "data_size;512" << std::endl;
                benchmarkMLRPQ<KEY_TYPE, B<512>>(i, min_key, max_key, number_of_segments, max_segment_size, radix);
                std::cout << "data_size;1024" << std::endl;
                benchmarkMLRPQ<KEY_TYPE, B<1024>>(i, min_key, max_key, number_of_segments, max_segment_size, radix);
            }
        }
    }

    if (choose_style == "competitive") {
        if (choose_size == "small") {
            for (int i = 1; i <= runs; i++){
                std::cout << "data_size;1" << std::endl;
                benchmarkCompetitive<KEY_TYPE, B<1>>(i, min_key, max_key, number_of_segments, max_segment_size, radix);
                std::cout << "data_size;16" << std::endl;
                benchmarkCompetitive<KEY_TYPE, B<16>>(i, min_key, max_key, number_of_segments, max_segment_size, radix);
                std::cout << "data_size;64" << std::endl;
                benchmarkCompetitive<KEY_TYPE, B<64>>(i, min_key, max_key, number_of_segments, max_segment_size, radix);
                std::cout << "data_size;128" << std::endl;
                benchmarkCompetitive<KEY_TYPE, B<128>>(i, min_key, max_key, number_of_segments, max_segment_size, radix);
            }
        }

        if (choose_size == "large") {
            for (int i = 1; i <= runs; i++){
                std::cout << "data_size;256" << std::endl;
                benchmarkCompetitive<KEY_TYPE, B<256>>(i, min_key, max_key, number_of_segments, max_segment_size, radix);
                std::cout << "data_size;512" << std::endl;
                benchmarkCompetitive<KEY_TYPE, B<512>>(i, min_key, max_key, number_of_segments, max_segment_size, radix);
                std::cout << "data_size;1024" << std::endl;
                benchmarkCompetitive<KEY_TYPE, B<1024>>(i, min_key, max_key, number_of_segments, max_segment_size, radix);
            }
        }
    }

    return 0;
}
