//
// Created by Andreas Herzog on 12/08/17.
// andreas.herzog@stud.uni-frankfurt.de
//

#include "include/multilayer_radix_pq.h"
#include <iostream>
#include <algorithm>
#include <chrono>


typedef struct data
{
    float large[250][250];
} DATA;


class CompareKey
{
public:
    bool operator()(std::pair<uint64_t, DATA> n1,std::pair<uint64_t, DATA> n2) {
        return n1.first>n2.first;
    }
};


std::pair<std::chrono::steady_clock::time_point, std::chrono::steady_clock::time_point> benchMLRPQ(std::vector<uint64_t> random_numbers, DATA val);

std::pair<std::chrono::steady_clock::time_point, std::chrono::steady_clock::time_point> benchPQ(std::vector<uint64_t> random_numbers, DATA val);

int main()
{
    DATA val;
    std::vector<uint64_t> random_numbers;

    ///section generating random numbers
    std::cout << "generating random numbers" << std::endl;
    std::chrono::steady_clock::time_point generate_start = std::chrono::steady_clock::now();
    uint64_t N;
    uint64_t number_of_entries = 1 << 12;


    const uint64_t a = 0;
    const uint64_t b = 1 << 24;

    N = 1 << 24;

    uint64_t nrandomnumbers(N);

    srand((unsigned)time(0));
    for(int i = 1; i <= number_of_entries; i++)
    {
        N = a + rand() % b - a + 1;
        random_numbers.push_back(N);
    }

    std::chrono::steady_clock::time_point generate_end = std::chrono::steady_clock::now();


    ///section sorting array
    std::cout << "started sorting array" << std::endl;
    std::chrono::steady_clock::time_point sorting_start = std::chrono::steady_clock::now();
    std::sort(random_numbers.begin(), random_numbers.end());
    std::chrono::steady_clock::time_point sorting_end = std::chrono::steady_clock::now();

    ///benchmark pq
    std::cout << "started benchmarking pq" << std::endl;
    auto time_pq = benchPQ(random_numbers, val);
    std::cout << "fnished benchmarking pq" << std::endl;

    ///benchmark mlrpq
    std::cout << "started benchmarking mlrpq" << std::endl;
    auto time_mlrpq = benchMLRPQ(random_numbers, val);
    std::cout << "finished benchmarking mlrpq" << std::endl;




    std::cout << "Time generating:\t" << std::chrono::duration_cast<std::chrono::microseconds>(generate_end - generate_start).count() << "us" << std::endl;
    std::cout << "Time sorting:\t\t" << std::chrono::duration_cast<std::chrono::microseconds>(sorting_end - sorting_start).count() << "us" << std::endl;
    std::cout << "Time mlrpq:\t\t\t" << std::chrono::duration_cast<std::chrono::microseconds>(time_mlrpq.first - time_mlrpq.second).count() << "us" << std::endl;
    std::cout << "Time pq:\t\t\t" << std::chrono::duration_cast<std::chrono::microseconds>(time_pq.first - time_pq.second).count() << "us" << std::endl;

}

std::pair<std::chrono::steady_clock::time_point, std::chrono::steady_clock::time_point> benchPQ(std::vector<uint64_t> random_numbers, DATA val) {
    std::priority_queue<std::pair<uint64_t,DATA>, std::vector<std::pair<uint64_t,DATA>>, CompareKey> pq;
    std::chrono::steady_clock::time_point pq_start = std::chrono::steady_clock::now();
    uint64_t control = ::std::numeric_limits<unsigned long>::min();
    for (int j=0; j <= random_numbers.size(); j++){
        auto tmp = std::make_pair(random_numbers[j], val);
        pq.push(tmp);
    }

    while(!pq.empty()){
        uint64_t temp = pq.top().first;
        pq.pop();
        if (temp < control){ std::cout << "fail" << std::endl;}
        else{control = temp;};
    }
    std::chrono::steady_clock::time_point pq_end = std::chrono::steady_clock::now();

    std::pair<std::chrono::steady_clock::time_point,std::chrono::steady_clock::time_point> out;
    out.first = pq_end;
    out.second = pq_start;

    return out;
}

std::pair<std::chrono::steady_clock::time_point, std::chrono::steady_clock::time_point> benchMLRPQ(std::vector<uint64_t> random_numbers, DATA val) {
    multilayer_radix_pq::multilayer_radix_pq<uint64_t, DATA, 5> mlrpq;
    std::chrono::steady_clock::time_point mlrpq_start = std::chrono::steady_clock::now();
    uint64_t control = ::std::numeric_limits<unsigned long>::min();
    for (int j=0; j <= random_numbers.size(); j++){
        uint64_t tmp = random_numbers[j];
        mlrpq.push(tmp, val);
    }

    while(!mlrpq.empty()){
        uint64_t temp = mlrpq.top().first;
        mlrpq.pop();
        if (temp < control){ std::cout << "fail" << std::endl;}
        else{control = temp;};
    }
    std::chrono::steady_clock::time_point mlrpq_end = std::chrono::steady_clock::now();

    std::pair<std::chrono::steady_clock::time_point,std::chrono::steady_clock::time_point> out;
    out.first = mlrpq_end;
    out.second = mlrpq_start;

    return out;

}
