#include <iostream>
#include <algorithm>
#include <random>
#include <functional>
#include "include/multilayer_radix_pq.h"

std::vector<uint64_t> random_array(size_t i, size_t j, size_t number_of_elements);

typedef struct data
{
    int x = 0;
} DATA;

int main() {
    data x;
    std::cout << "Initializing multilayer radix pq" << std::endl;
    multilayer_radix_pq::multilayer_radix_pq<uint64_t, DATA, 5> mlrpq;
    std::cout << "Generate random numbers in range i, j" << std::endl;
    std::vector<uint64_t> random_numbers_first_step = random_array(0, size_t(1) << 12, 10000);
    size_t vector_size_first_step = random_numbers_first_step.size();

    for (int i; i <= vector_size_first_step; i++){
        mlrpq.push(random_numbers_first_step[i], x);
    }

    std::vector<std::pair<uint64_t, DATA>> output_vector;
    std::cout << "Pop first 10% of elements" << std::endl;
    for (int j=0; j <= floor(vector_size_first_step/10); j++){
        std::pair<uint64_t, DATA> elem = mlrpq.top();
        output_vector.emplace_back(elem);
        mlrpq.pop();
    }
    std::cout << "mlrpq is empty: " << mlrpq.empty() << std::endl;

    std::cout << "Generate new set of numbers" << std::endl;
    std::vector<uint64_t> random_numbers_second_step = random_array(0, size_t(1) << 32, 2500);
    size_t vector_size_second_step = random_numbers_first_step.size();

    std::cout << "Push more elements into mlrpq if monotonicity is upheld" << std::endl;
    for (int i; i <= vector_size_second_step; i++){
        if (random_numbers_second_step[i] > size_t(1)<<12){
            mlrpq.push(random_numbers_second_step[i], x);
        }
    }

    std::cout << "Pop all elements" << std::endl;
    while (!mlrpq.empty()){
        std::pair<uint64_t, DATA> elem = mlrpq.top();
        output_vector.emplace_back(elem);
        mlrpq.pop();
    }

    std::cout << "mlrpq is empty: " << mlrpq.empty() << std::endl;


    return 0;
}


std::vector<uint64_t> random_array(size_t i, size_t j, size_t number_of_elements)
{
    std::random_device rnd_device;
    std::mt19937 mersenne_engine(rnd_device());
    std::uniform_int_distribution<uint64_t> dist(i, j);

    auto gen = std::bind(dist, mersenne_engine);
    std::vector<uint64_t> random_numbers(number_of_elements);
    std::generate(begin(random_numbers), end(random_numbers), gen);

    return random_numbers;


}