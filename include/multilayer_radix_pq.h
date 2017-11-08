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


namespace multilayer_radix_pq {

    namespace internal {
        static constexpr std::pair<size_t, size_t> calculateBucket(uint64_t key, uint64_t last, size_t r)
        {
            //Left very explicit for now to debug
            size_t index_least_significant = (std::numeric_limits<uint64_t>::digits - __builtin_clzll(key ^ last)) - 1;
            size_t i = static_cast<size_t>(floor(index_least_significant / log2(r)));
            size_t shift = log2(r)*i;
            size_t j = (key >> shift);
            return std::pair<size_t, size_t>(i,j);
        };
    }; // end of namespace internal


    template<typename KeyType, typename ValueType, size_t radix>
    class multilayer_radix_pq {
    public:
        //limiting the queue size to avoid memory overflow
        static constexpr auto block_size = size_t(1) << 18;
        using key_type = KeyType;
        using value_type = ValueType;
        using block_type = stxxl::queue<std::pair<key_type, value_type>, block_size>;
    private:
        static const int len = std::numeric_limits<key_type>::digits;
        static const size_t NoOfQueues = radix*ceil(len/log2(radix));
        int C_;
        uint64_t last_;
        std::array<std::array<block_type, NoOfQueues>, len> buckets_;
        std::array<bool, len> bucket_flags_;

    public:
        multilayer_radix_pq(int C) : C_(C), last_(){//}, buckets_() {
            bucket_flags_.fill(0);
            std::cout << NoOfQueues << std::endl;
        };

        void push(key_type key, value_type val) {
            // TODO replace reinterpret_cast<>(key) with encoder
            // TODO special case of N bucket
            std::pair<uint64_t, uint64_t> pos = internal::calculateBucket(reinterpret_cast<uint64_t>(key), last_, radix);
            bucket_flags_[pos.first] = 1;
            std::cout << "Pushing into B(" << pos.first << ", " << pos.second << ")." << std::endl;
            buckets_[pos.first].at(pos.second).push(std::pair<key_type, value_type>(key, val));
        }

        bool empty() {
            //TODO not working since blocks are intialized and buckets_ therefore not empty
            return buckets_.empty();
        }

    };
}


#endif //MULTILAYER_RADIX_PRIORITY_QUEUE_MULTILAYER_RADIX_PQ_H
