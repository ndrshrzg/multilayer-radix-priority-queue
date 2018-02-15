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

std::ostream& operator<< (std::ostream& o, const std::pair<uint64_t, B<1024>>& d) {
    o << d.first;
    return o;
};

template <typename KeyType, typename ValueType>
void benchmarkMLRPQ(int runs, KeyType max_key, KeyType number_of_segments, KeyType max_segment_size) {

    benchmark::mlrpqOnlyAllInAllOut<KeyType, ValueType, 1> mlrpqOnlyAllInAllOut1(runs, max_key, number_of_segments, max_segment_size);

    benchmark::mlrpqOnlyAllInAllOut<KeyType, ValueType, 2> mlrpqOnlyAllInAllOut2(runs, max_key, number_of_segments, max_segment_size);

    benchmark::mlrpqOnlyAllInAllOut<KeyType, ValueType, 3> mlrpqOnlyAllInAllOut3(runs, max_key, number_of_segments, max_segment_size);

    benchmark::mlrpqOnlyAllInAllOut<KeyType, ValueType, 4> mlrpqOnlyAllInAllOut4(runs, max_key, number_of_segments, max_segment_size);

    benchmark::mlrpqOnlyAllInAllOut<KeyType, ValueType, 5> mlrpqOnlyAllInAllOut5(runs, max_key, number_of_segments, max_segment_size);


}

template<typename KeyType, typename ValueType>
void benchmarkMLRPQInterrupted(int runs, KeyType min_key, KeyType max_key, KeyType number_of_segments, KeyType max_segment_size){

    benchmark::mlrpqOnlyAllInAllOutInterrupted<KeyType, ValueType, 1> mlrpqOnlyInterrutped1(runs, min_key, max_key, number_of_segments, max_segment_size);

    benchmark::mlrpqOnlyAllInAllOutInterrupted<KeyType, ValueType, 2> mlrpqOnlyInterrutped2(runs, min_key, max_key, number_of_segments, max_segment_size);

    benchmark::mlrpqOnlyAllInAllOutInterrupted<KeyType, ValueType, 3> mlrpqOnlyInterrutped3(runs, min_key, max_key, number_of_segments, max_segment_size);

    benchmark::mlrpqOnlyAllInAllOutInterrupted<KeyType, ValueType, 4> mlrpqOnlyInterrutped4(runs, min_key, max_key, number_of_segments, max_segment_size);

    benchmark::mlrpqOnlyAllInAllOutInterrupted<KeyType, ValueType, 5> mlrpqOnlyInterrutped5(runs, min_key, max_key, number_of_segments, max_segment_size);

}

template <typename KeyType, typename ValueType>
void benchmarkCompetitive(int runs, KeyType max_key, KeyType number_of_segments, KeyType max_segment_size){
    benchmark::allInAllOut<KeyType, ValueType, 5> allInAllOut1(runs, max_key, number_of_segments, max_segment_size);
};

template <typename KeyType, typename ValueType>
void benchmarkCompetitiveInterrupted(int runs, KeyType min_key, KeyType max_key, KeyType number_of_segments, KeyType max_segment_size){
    benchmark::allInAllOutInterrupted<KeyType, ValueType, 5> allInAllOutInterrupted1(runs, min_key, max_key, number_of_segments, max_segment_size);
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

    if (choose_style == "single"){
        if (choose_size == "tiny"){
            std::cout << "data_size;1" << std::endl;
            benchmarkMLRPQ<KEY_TYPE, B<1>>(runs, max_key, number_of_segments, max_segment_size);
            std::cout << "data_size;1" << std::endl;
            benchmarkMLRPQInterrupted<KEY_TYPE , B<1>>(runs, min_key, max_key, number_of_segments, max_segment_size);
        }

        if (choose_size == "small"){
            std::cout << "data_size;1" << std::endl;
            benchmarkMLRPQ<KEY_TYPE, B<1>>(runs, max_key, number_of_segments, max_segment_size);
            std::cout << "data_size;25" << std::endl;
            benchmarkMLRPQ<KEY_TYPE, B<25>>(runs, max_key, number_of_segments, max_segment_size);
            std::cout << "data_size;50" << std::endl;
            benchmarkMLRPQ<KEY_TYPE, B<50>>(runs, max_key, number_of_segments, max_segment_size);
            std::cout << "data_size;100" << std::endl;
            benchmarkMLRPQ<KEY_TYPE, B<100>>(runs, max_key, number_of_segments, max_segment_size);

            std::cout << "data_size;1" << std::endl;
            benchmarkMLRPQInterrupted<KEY_TYPE , B<1>>(runs, min_key, max_key, number_of_segments, max_segment_size);
            std::cout << "data_size;25" << std::endl;
            benchmarkMLRPQInterrupted<KEY_TYPE , B<25>>(runs, min_key, max_key, number_of_segments, max_segment_size);
            std::cout << "data_size;50" << std::endl;
            benchmarkMLRPQInterrupted<KEY_TYPE , B<50>>(runs, min_key, max_key, number_of_segments, max_segment_size);
            std::cout << "data_size;100" << std::endl;
            benchmarkMLRPQInterrupted<KEY_TYPE , B<100>>(runs, min_key, max_key, number_of_segments, max_segment_size);
        }
        if (choose_size == "large"){
            std::cout << "data_size;250" << std::endl;
            benchmarkMLRPQ<KEY_TYPE, B<250>>(runs, max_key, number_of_segments, max_segment_size);
            std::cout << "data_size;500" << std::endl;
            benchmarkMLRPQ<KEY_TYPE, B<500>>(runs, max_key, number_of_segments, max_segment_size);
            std::cout << "data_size;1024" << std::endl;
            benchmarkMLRPQ<KEY_TYPE, B<1024>>(runs, max_key, number_of_segments, max_segment_size);

            std::cout << "data_size;250" << std::endl;
            benchmarkMLRPQInterrupted<KEY_TYPE , B<250>>(runs, min_key, max_key, number_of_segments, max_segment_size);
            std::cout << "data_size;500" << std::endl;
            benchmarkMLRPQInterrupted<KEY_TYPE , B<500>>(runs, min_key, max_key, number_of_segments, max_segment_size);
            std::cout << "data_size;1024" << std::endl;
            benchmarkMLRPQInterrupted<KEY_TYPE , B<1024>>(runs, min_key, max_key, number_of_segments, max_segment_size);
        }

    }

    if (choose_style == "competitive") {
        if (choose_size == "small") {
            std::cout << "data_size;1" << std::endl;
            benchmarkCompetitive<KEY_TYPE, B<1>>(runs, max_key, number_of_segments, max_segment_size);
            std::cout << "data_size;25" << std::endl;
            benchmarkCompetitive<KEY_TYPE, B<25>>(runs, max_key, number_of_segments, max_segment_size);
            std::cout << "data_size;50" << std::endl;
            benchmarkCompetitive<KEY_TYPE, B<50>>(runs, max_key, number_of_segments, max_segment_size);
            std::cout << "data_size;100" << std::endl;
            benchmarkCompetitive<KEY_TYPE, B<100>>(runs, max_key, number_of_segments, max_segment_size);

            std::cout << "data_size;1" << std::endl;
            benchmarkCompetitiveInterrupted<KEY_TYPE, B<1>>(runs, min_key, max_key, number_of_segments, max_segment_size);
            std::cout << "data_size;25" << std::endl;
            benchmarkCompetitiveInterrupted<KEY_TYPE, B<25>>(runs, min_key, max_key, number_of_segments, max_segment_size);
            std::cout << "data_size;50" << std::endl;
            benchmarkCompetitiveInterrupted<KEY_TYPE, B<50>>(runs, min_key, max_key, number_of_segments, max_segment_size);
            std::cout << "data_size;100" << std::endl;
            benchmarkCompetitiveInterrupted<KEY_TYPE, B<100>>(runs, min_key, max_key, number_of_segments, max_segment_size);
        }

        if (choose_size == "large") {
            std::cout << "data_size;250" << std::endl;
            benchmarkCompetitive<KEY_TYPE, B<250>>(runs, max_key, number_of_segments, max_segment_size);
            std::cout << "data_size;500" << std::endl;
            benchmarkCompetitive<KEY_TYPE, B<500>>(runs, max_key, number_of_segments, max_segment_size);
            std::cout << "data_size;1024" << std::endl;
            benchmarkCompetitive<KEY_TYPE, B<1024>>(runs, max_key, number_of_segments, max_segment_size);

            std::cout << "data_size;250" << std::endl;
            benchmarkCompetitiveInterrupted<KEY_TYPE, B<250>>(runs, min_key, max_key, number_of_segments, max_segment_size);
            std::cout << "data_size;500" << std::endl;
            benchmarkCompetitiveInterrupted<KEY_TYPE, B<500>>(runs, min_key, max_key, number_of_segments, max_segment_size);
            std::cout << "data_size;1024" << std::endl;
            benchmarkCompetitiveInterrupted<KEY_TYPE, B<1024>>(runs, min_key, max_key, number_of_segments, max_segment_size);
        }
    }

    return 0;
}
