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


template<typename KeyType, typename ValueType, size_t radix>
class multilayer_radix_pq
{
public:
    using key_type = KeyType;
    using value_type = ValueType;
    using block_type = stxxl::queue<std::pair<key_type, value_type>>;
    using buckets_type = std::array<block_type, radix>;

private:

    static const int len = std::numeric_limits<key_type>::digits;

    std::array<buckets_type, len> buckets_;
    std::array<bool, len> bucket_flags_;

public:
    multilayer_radix_pq()
    {
        bucket_flags_.fill(0);
    };

    void push(key_type key, value_type val)
    {
        //TODO calculate bucket, [i][j] instead of [i][0] with i being index of least significant bit changed and j being key at index i
        std::pair<int, int> pos = calculateBucket(key);
        bucket_flags_[pos.first] = 1;
        buckets_[pos.first][pos.second].push(std::pair<key_type, value_type>(key, val));
    }
    bool empty()
    {
        return buckets_[0].empty();
    }

private:
    std::pair<int, int> calculateBucket(key_type key)
    {
        //TODO calculate key in base r_
        // find least significant bit different from min in base r, return index of lsb and value as pair
        // so res.first is bucket index, res.second is block index
        int i = key % len; // mock calculations
        int j = key % radix;
        return std::pair<int, int>(i,j);
    }


    int changeBase(key_type key, size_t r)
    {
        std::cout << "key: " << key << "base: " << r << std::endl;
        return key;
    }


};




#endif //MULTILAYER_RADIX_PRIORITY_QUEUE_MULTILAYER_RADIX_PQ_H
