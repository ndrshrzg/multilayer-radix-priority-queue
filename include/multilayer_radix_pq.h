//
// Created by ndrshrzg on 10/26/17.
//

//#include <stxxl/queue>
#include <vector>
#include <array>
#include <cmath>
#include <limits>
#include <iterator>
#include <cassert>

#ifndef MULTILAYER_RADIX_PRIORITY_QUEUE_MULTILAYER_RADIX_PQ_H
#define MULTILAYER_RADIX_PRIORITY_QUEUE_MULTILAYER_RADIX_PQ_H

/// Outline:
///  Bitvector of size h (limited by C in base r?) indicating whether bucket is empty
///  Buckets (std::array) of size r pointing to
///  Blocks (stxxl::queue) containing the key, val pairs
///  As pointed out in the paper the bitvector (priority queue in paper) and buckets (disk pages)
///  will fit in internal memory


namespace multilayer_radix_pq {

    namespace internal {
        static constexpr auto calculateBucket(uint64_t key, uint64_t last, size_t r) {
            // TODO REVIEW IF THIS IF WORKING CORRECTLY, WHY ARE 128 and 129 PUSHED INTO SAME BUCKET???
            //Left very explicit for now to debug
            auto x = __builtin_clzll(key ^ last);
            size_t index_least_significant = (std::numeric_limits<uint64_t>::digits - __builtin_clzll(key ^ last)) - 1;
            size_t i = floor(index_least_significant / log2(r));
            auto shift = static_cast<size_t>(log2(r) * i);
            size_t j = (key >> shift);
            return std::pair<size_t, size_t>(i, j);
        };

        static constexpr auto findFirstNonEmpty(/*bucket_empty_flags_*/){};

    }; // end of namespace internal


    template<typename KeyType, typename ValueType, size_t RADIX_BITS>
    class multilayer_radix_pq {
    public:
        //limiting the queue size to avoid memory overflow
        //static constexpr auto block_size = size_t(1) << 18; // deprecated for nostxxl branch
        using key_type = KeyType;
        using value_type = ValueType;
        //using block_type = stxxl::queue<std::pair<key_type, value_type>, block_size>; // deprecated for nostxxl branch
        using block_type = std::vector<std::pair<key_type, value_type>>;
    private:
        static const int len = std::numeric_limits<key_type>::digits;
        static const size_t radix = size_t(1) << RADIX_BITS;
        static const size_t no_of_queues = static_cast<size_t>(radix * ceil(len / log2(radix)));
        int C_;
        key_type last_minimum_;
        std::array<std::array<block_type, no_of_queues>, len> buckets_;
        std::array<std::pair<int, std::array<bool, no_of_queues>>, len> bucket_empty_flags_;

    public:
        explicit multilayer_radix_pq(int C) : C_(C) {//, array_last_(){//}, buckets_() {
            bucket_empty_flags_ = {};
            last_minimum_ = std::numeric_limits<key_type>::min();
        };

        void push(key_type key, value_type val) {
            // TODO replace reinterpret_cast<>(key) with encoder
            // TODO special case of N bucket
            // TODO implement last
            assert(key >= last_minimum_);
            std::pair<uint64_t, uint64_t> pos = internal::calculateBucket(
                    reinterpret_cast<uint64_t>(key), last_minimum_, radix);
            /// IDEA: set empty_flag to += 1 to simultaneously count the number of elemens inside? this way
            /// the count of elemens can be decremented when poping an element
            bucket_empty_flags_[pos.first].first = bucket_empty_flags_[pos.first].second[pos.second] = 1;
            std::cout << "Pushing into B(" << pos.first << ", " << pos.second << ")." << std::endl;
            buckets_[pos.first][pos.second].push_back(std::pair<key_type, value_type>(key, val));
        }

        std::pair<key_type, value_type> pop() {
            //TODO get elem from first non empty bucket, update last_ and reorganize elems in bucket
            /// use find_first_non_empty then pull data -> remove element, if bucket empty update min and
            /// reorganize
            // find position of first non empty bucket
            std::pair<int, int> pos_minimum_element_ = top();
            // access element at first non empty bucket position
            std::pair<key_type, value_type> minimum_element_ = buckets_[pos_minimum_element_.first][pos_minimum_element_.second].back();
            // update bucket empty flags if vectors were emptied
            if (buckets_[pos_minimum_element_.first][pos_minimum_element_.second].empty())
                bucket_empty_flags_[pos_minimum_element_.first].second[pos_minimum_element_.second] = 0;
            for (int j = 0; j < no_of_queues; j++){
                if (buckets_[pos_minimum_element_.first][j].empty())
                {
                    bucket_empty_flags_[pos_minimum_element_.first].first = 0;
                    // TODO at this point reorganization needs to take place
                }

            }
            // pop element from first non empty bucket
            buckets_[pos_minimum_element_.first][pos_minimum_element_.second].pop_back();
            //update last_minimum_
            last_minimum_ = minimum_element_.first;

            return minimum_element_;
        }

        auto top() {
            //redefine as constexpr find_first_non_empty?
            for (int i = 0; i < len; i++) {
                if (bucket_empty_flags_[i].first) {
                    for (int j = 0; j < no_of_queues; j++){
                        if (bucket_empty_flags_[i].second[j]) {
                            return std::pair<int, int>(i,j);
                        }
                    }
                }
            }
            return std::pair<int,int>(-1,-1);
        }

        bool empty() {
            return (top() == std::pair<int,int>(-1, -1));
        }

        bool bucketEmpty(int k){
            return !bucket_empty_flags_[k].first;
        }

    };
}


#endif //MULTILAYER_RADIX_PRIORITY_QUEUE_MULTILAYER_RADIX_PQ_H
