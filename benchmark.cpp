//
// Created by Andreas Herzog on 12/08/17.
// andreas.herzog@stud.uni-frankfurt.de
//

#include "include/multilayer_radix_pq.h"

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


time_t benchMLRPQ(std::vector<uint64_t> random_numbers, DATA val);

time_t benchPQ(std::vector<uint64_t> random_numbers, DATA val);

int main()
{
    DATA val;
    std::vector<uint64_t> random_numbers;

    ///section generating random numbers
    std::cout << "generating random numbers" << std::endl;
    time_t generate_start = time(0);
    uint64_t N;
    uint64_t number_of_entries = 1 << 12;


    const uint64_t a = 0;
    const uint64_t b = 1 << 31;

    N = 1 << 24;

    uint64_t nrandomnumbers(N);

    srand((unsigned)time(0));
    for(int i = 1; i <= number_of_entries; i++)
    {
        N = a + rand() % b - a + 1;
        random_numbers.push_back(N);
    }

    time_t generate_end = time(0);


    ///section sorting array
    std::cout << "started sorting array" << std::endl;
    time_t sorting_start = time(0);
    std::sort(random_numbers.begin(), random_numbers.end());
    time_t sorting_end = time(0);

    ///benchmark mlrpq
    time_t time_mlrpq = benchMLRPQ(random_numbers, val);

    ///benchmark pq
    time_t time_pq = benchPQ(random_numbers, val);


    std::cout << "Time generating: " << generate_end-generate_start << std::endl;
    std::cout << "Time sorting: " << sorting_end-sorting_start << std::endl;
    std::cout << "Time mlrpq: " << time_mlrpq << std::endl;
    std::cout << "Time pq: " << time_pq << std::endl;

}

time_t benchPQ(std::vector<uint64_t> random_numbers, DATA val) {
    std::priority_queue<std::pair<uint64_t,DATA>, std::vector<std::pair<uint64_t,DATA>>, CompareKey> pq;
    time_t pq_start = time(0);
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
    time_t pq_end = time(0);

    return pq_end-pq_start;
}

time_t benchMLRPQ(std::vector<uint64_t> random_numbers, DATA val) {
    multilayer_radix_pq::multilayer_radix_pq<uint64_t, DATA, 5> mlrpq;
    time_t mlrpq_start = time(0);
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
    time_t mlrpq_end = time(0);

    return mlrpq_end-mlrpq_start;

}
