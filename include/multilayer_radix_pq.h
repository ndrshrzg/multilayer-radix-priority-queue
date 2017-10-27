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
    const int r_;

    static const int len = std::numeric_limits<key_type>::digits;
    //TODO at the moment this is just an array of queues, change to an array<T, len> with T = array<block_type, r_>
    std::array<block_type, len> buckets_;
    std::array<bool, len> bucket_flags_;

public:
    multilayer_radix_pq(int r) : r_(r)
    {
        std::cout << "Len of type: " << len << std::endl;
        bucket_flags_.fill(0);
    };

    void push(key_type key, value_type val)
    {
        //TODO calculate bucket, [i] instead of [0]
        int i = calculateBucket(key);
        std::cout << "calculated bucket for key: " << key << " is: " << i << std::endl;
        bucket_flags_[i] = 1;
        buckets_[i].push(std::pair<key_type, value_type>(key, val));
    }
    bool empty()
    {
        return buckets_[0].empty();
    }

    void testInternal()
    {
        std::cout << "Size of buckets_: " << sizeof(buckets_) << std::endl;
        std::cout << buckets_[0].empty() << std::endl;
        std::cout << buckets_[63].empty() << std::endl;
        std::cout << buckets_[64].empty() << std::endl; // no out of range error!

    }

private:
    int calculateBucket(key_type key)
    {
        //TODO calculate key in base r_
        // find least significant bit different from min in base r, return index of lsb and value as pair
        // so res.first is bucket index, res.second is block index
        int res = key % len;
        return res;
    }


};




#endif //MULTILAYER_RADIX_PRIORITY_QUEUE_MULTILAYER_RADIX_PQ_H
