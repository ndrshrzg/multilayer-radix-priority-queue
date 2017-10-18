#include <iostream>
#include <stxxl/vector>
#include "multilayer_radix_pq.h"

int main() {

    N_queue<int, std::string> nq;

    std::cout << "Is empty: " << nq.empty() << std::endl;

    nq.push(3, "three");
    nq.push(5, "five");
    nq.push(7, "seven");
    nq.push(9, "nine");

    std::cout << "Size is: " << nq.size() << std::endl;
    std::cout << "Minimum is: " << nq.min() << std::endl;

    nq.pop();

    std::cout << "Size is: " << nq.size() << std::endl;
    std::cout << "Minimum is: " << nq.min() << std::endl;

    std::cout << "Is empty: " << nq.empty() << std::endl;






    return 0;
}