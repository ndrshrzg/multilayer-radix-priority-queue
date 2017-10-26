#include <iostream>
#include "multilayer_radix_pq.h"
#include "templated_priority_queue.h"

void templated_priority_queue_test();

int main() {

    //templated_priority_queue_test();

    multilayer_radix_pq<int, std::string> (64, 8);
    return 0;
}

void templated_priority_queue_test() {
    templated_priority_queue<int, std::__cxx11::string> nq;

    std::cout << "Is empty: " << nq.empty() << std::endl;

    nq.push(3, "three");
    nq.push(5, "five");
    nq.push(7, "seven");
    nq.push(9, "nine");
    nq.push(8, "eight");

    std::cout << "Size is: " << nq.size() << std::endl;
    std::cout << "Minimum is: " << nq.min() << std::endl;
    std::cout << "Value of Minimum is: " << nq.minVale() << std::endl;

    nq.pop();
    //nq.push(6, "six"); // monotonicity after first pop


    std::cout << "Size is: " << nq.size() << std::endl;
    std::cout << "Minimum is: " << nq.min() << std::endl;
    std::cout << "Value of Minimum is: " << nq.minVale() << std::endl;


    std::cout << "Is empty: " << nq.empty() << std::endl;
}