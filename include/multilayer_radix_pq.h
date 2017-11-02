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
        static constexpr std::pair<size_t, size_t> calculateBucket(uint64_t key, uint64_t last, uint64_t r)
        {
            //Left very explicit for now to debug
            size_t index_least_significant = (std::numeric_limits<uint64_t>::digits - __builtin_clzll(key ^ last)) - 1;
            size_t i = static_cast<size_t>(ceil(index_least_significant / r));
            size_t shift = r*i;
            size_t j = (key >> shift+1);
            // Index is limited to 16 at the moment since instantiating more stxxl::queues crashes the system
            assert(j <= 16);
            return std::pair<size_t, size_t>(i,j);
        };
    }; // end of namespace internal


    template<typename KeyType, typename ValueType>//, size_t radix>
    class multilayer_radix_pq {
    public:
        using key_type = KeyType;
        using value_type = ValueType;
        using block_type = stxxl::queue<std::pair<key_type, value_type>>;
    private:
        int r_, C_, last_;
        static const int len = std::numeric_limits<key_type>::digits;
        std::array<std::array<block_type, 16>, len> buckets_;
        std::array<bool, len> bucket_flags_;

    public:
        multilayer_radix_pq(int r, int C) : r_(r), C_(C), last_(), buckets_() {
            bucket_flags_.fill(0);
        };

        void push(key_type key, value_type val) {
            std::pair<uint64_t, uint64_t> pos = internal::calculateBucket(key, last_, r_);
            bucket_flags_[pos.first] = 1;
            std::cout << "Pushing into B(" << pos.first << ", " << pos.second << ")." << std::endl;
            buckets_[pos.first].at(pos.second).push(std::pair<key_type, value_type>(key, val));
        }

        bool empty() {
            return buckets_[0].empty();
        }

    };
}


#endif //MULTILAYER_RADIX_PRIORITY_QUEUE_MULTILAYER_RADIX_PQ_H
