#include <iostream>
#include <stxxl/vector>
#include "multilayer_radix_pq.h"

void TestQ(Q &q);
void TestBucket(bucket<int, std::string> &b);

int main() {
    bucket<int, std::string> b;
    Q q;

    TestQ(q);
    //TestBucket(b);

    return 0;
}

void TestBucket(bucket<int, std::string> &b)
{
    std::cout << "Testing bucket" << std::endl;
    std::cout << "Is empty" << std::endl;
    std::cout << b.empty() << std::endl;

    b.push(5, "five");
    b.push(8, "eight");
    std::cout << "Is empty" << std::endl;
    std::cout << b.empty() << std::endl;

}

void TestQ(Q &q) {
    std::cout << "Testing Q" << std::endl;
    std::cout << "Is empty: " << q.empty() << std::endl;

    q.push(std::pair<int, int>(1,2));
    q.push(std::pair<int, int>(1,1));
    q.push(std::pair<int, int>(3,2));
    q.push(std::pair<int, int>(6,4));

    std::cout << "Size is: " << q.size() << std::endl;
    std::cout << "Minimum is: " << q.min().first << ", " << q.min().second << std::endl;

    q.pop();

    std::cout << "Size is: " << q.size() << std::endl;
    std::cout << "Minimum is: " << q.min().first << ", " << q.min().second << std::endl;

    q.pop();
    std::cout << "Minimum is: " << q.min().first << ", " << q.min().second << std::endl;

    q.clear();
    std::cout << "Is empty: " << q.empty() << std::endl;
}