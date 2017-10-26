//
// Created by ndrshrzg on 10/26/17.
//

#include <stxxl/queue>
#include <vector>
#include <cmath>

#ifndef MULTILAYER_RADIX_PRIORITY_QUEUE_MULTILAYER_RADIX_PQ_H
#define MULTILAYER_RADIX_PRIORITY_QUEUE_MULTILAYER_RADIX_PQ_H




//TODO: Datastructure buckets, enclosing datastructure multilayer_radix_pq using Nqueue and buckets
/// Outline:
///  Bitvector of size h (limited by C in base r?) indicating whether bucket is empty
///  Buckets (std::array) of size r pointing to
///  Blocks (stxxl::queue) containing the key, val pairs
///  As pointed out in the paper the bitvector (priority queue in paper) and buckets (disk pages)
///  will fit in internal memory



template<typename KeyType, typename ValueType>
class multilayer_radix_pq
{
public:
    using key_type = KeyType;
    using value_type = ValueType;
    using block_type = stxxl::queue<std::pair<key_type, value_type>>;


private:
    //TODO Bitvector member

    const double C_;
    const double r_;

    constexpr int calculateH(double i, double r)
    {
        return (ceil(log(i)/log(r)));
    }
    //TODO Array needs to be sized according to calculation of H
    std::array<std::vector<block_type>, 5> buckets_;

    //std::array<std::vector<block_type>, calculateH(C_, r_)> buckets_;

public:
    multilayer_radix_pq(int C, int r) : C_(C), r_(r)
    {
        std::cout << "H: " << ceil(log(C)/log(r)) << std::endl;
        std::cout << "r: " << r_ << std::endl;
    };

    void push(key_type key, value_type val)
    {
        //TODO calculate bucket, [i][j] instead of [0][0]
        buckets_[0].at(0).push(std::pair<key_type, value_type >(key, val));
    }

    void testInternal()
    {
        std::cout << sizeof(buckets_[0].at(0)) << std::endl;
    }


};




#endif //MULTILAYER_RADIX_PRIORITY_QUEUE_MULTILAYER_RADIX_PQ_H
