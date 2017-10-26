//
// Created by ndrshrzg on 10/26/17.
//

#include <stxxl/queue>
#include <vector>
#include <cmath>

#ifndef MULTILAYER_RADIX_PRIORITY_QUEUE_MULTILAYER_RADIX_PQ_H
#define MULTILAYER_RADIX_PRIORITY_QUEUE_MULTILAYER_RADIX_PQ_H




//TODO: Datastructure buckets, enclosing datastructure multilayer_radix_pq using Nqueue and buckets
// Outline:
//  Bitvector of size h (limited by C in base r?) indicating whether bucket is empty
//  Buckets (std::array) of size r pointing to
//  Blocks (stxxl::queue) containing the key, val pairs
//  As pointed out in the paper the bitvector (priority queue in paper) and buckets (disk pages)
//  will fit in internal memory



template<typename KeyType, typename ValueType>
class multilayer_radix_pq
{
public:
    using key_type = KeyType;
    using value_type = ValueType;

private:
    const int c_;
    const int r_;
    //constexpr int R = r_;
    //constexpr int H = ceil(log(c_)/log(R));

    //static int H = ceil(log(c_)/log(r_));

    //TODO Bitvector member
    /*std::array< // Vector containing Buckets
            std::array<  //Buckets containing Blocks
                    stxxl::queue<  //blocks in EM
                            std::pair<key_type, value_type>>, c_>, r_> rpq;  //actual data stored
    */
     public:
    multilayer_radix_pq(int C, int r) : c_(C), r_(r) {};
};




#endif //MULTILAYER_RADIX_PRIORITY_QUEUE_MULTILAYER_RADIX_PQ_H
