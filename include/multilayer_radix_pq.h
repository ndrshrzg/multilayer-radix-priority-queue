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
            //Left very explicit for now to debug
            //TODO : needs review, if last = 128, push(130) pushes into B(0, 130) instead of B(0, 2)
            if(key == last) {
                return std::pair<size_t, size_t>(0, 0);
            }
            else{
                size_t index_highest_significant = (std::numeric_limits<uint64_t>::digits - __builtin_clzll(key ^ last)) - 1;
                size_t i = floor(index_highest_significant / log2(r));
                auto shift = static_cast<size_t>(log2(r) * i);
                size_t j = (key >> shift);
                return std::pair<size_t, size_t>(i, j);
            }
        };

        template<typename key_type, typename block_type>
        static constexpr key_type scanBucketForMinimum(block_type bucket){
            key_type reorganization_minimum = std::numeric_limits<key_type>::max();
            while (!(bucket.empty())){
                if (bucket.back().first < reorganization_minimum){
                    reorganization_minimum = bucket.back().first;
                }
                bucket.pop_back();
            }
            return reorganization_minimum;
        }

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
        explicit multilayer_radix_pq(int C) : C_(C) {
            bucket_empty_flags_ = {};
            last_minimum_ = std::numeric_limits<key_type>::min();
        };

        void push(key_type key, value_type val) {
            // TODO replace reinterpret_cast<>(key) with encoder
            // TODO special case of N bucket
            assert(key >= last_minimum_);
            std::pair<uint64_t, uint64_t> pos = internal::calculateBucket(
                    reinterpret_cast<uint64_t>(key), last_minimum_, radix);
            bucket_empty_flags_[pos.first].first = bucket_empty_flags_[pos.first].second[pos.second] = 1;
            std::cout << "Pushing into B(" << pos.first << ", " << pos.second << ")." << std::endl; // output
            buckets_[pos.first][pos.second].push_back(std::pair<key_type, value_type>(key, val));
        }

        std::pair<key_type, value_type> pop() {
            // find position of first non empty bucket
            std::pair<int, int> pos_minimum_element_ = top();
            // if B(0, m0) empty
            if (pos_minimum_element_.first != 0) {
                std::cout << "begin reorganization phase" << std::endl;
                // calculate minimum of current first non empty bucket
                key_type reorganization_minimum = internal::scanBucketForMinimum<key_type, block_type>(buckets_[pos_minimum_element_.first][pos_minimum_element_.second]);
                // assign new minimum
                last_minimum_ = reorganization_minimum;
                // reorganize bucket into B(0, m0) using reorganization_minimum
                while(!buckets_[pos_minimum_element_.first][pos_minimum_element_.second].empty()){
                    // retrieve arbitrary element from current bucket
                    std::pair<key_type, value_type> temp_element = buckets_[pos_minimum_element_.first][pos_minimum_element_.second].back();
                    // push arbitrary element in mlrpq using the calculated minimum
                    push(temp_element.first, temp_element.second);
                    // remove element from old bucket
                    buckets_[pos_minimum_element_.first][pos_minimum_element_.second].pop_back();
                }
                // update flags for _old_ bucket
                updateBucketEmptyFlags(pos_minimum_element_);
                std::cout << "end reorganization phase" << std::endl;
                // call pop again after reorganization
                pop();
            }
            else{
                // retrieve element from bucket
                std::pair<key_type, value_type> minimum_element_ =
                        buckets_[pos_minimum_element_.first][pos_minimum_element_.second].back();
                // pop element from bucket
                buckets_[pos_minimum_element_.first][pos_minimum_element_.second].pop_back();
                // print popped element to console
                std::cout << "Popped: (" << minimum_element_.first << ", " << minimum_element_.second.x << ")."
                          << std::endl;
                // check bucket empty flags and update if necessary
                updateBucketEmptyFlags(pos_minimum_element_);
                // return minimum element
                return minimum_element_;
            }
        }

        // update bucket empty flags if vectors were emptied
        void updateBucketEmptyFlags(const std::pair<int, int> pos_update) {
            // check if bucket is empty
            if (buckets_[pos_update.first][pos_update.second].empty()){
                bucket_empty_flags_[pos_update.first].second[pos_update.second] = 0;
                // if bucket became empty, check if whole array is empty
                int temp = 0;
                for (int j = 0; j < no_of_queues; j++){
                    if (buckets_[pos_update.first][j].empty()){ temp += 1; }
                }
                if(temp == no_of_queues) { bucket_empty_flags_[pos_update.first].first = 0;
                    std::cout << "Array " << pos_update.first << " empty" << std::endl;} // output
            }
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

        //this method only exists for a unit test which is weird
        bool bucketEmpty(int k){
            return !bucket_empty_flags_[k].first;
        }

    };
}


#endif //MULTILAYER_RADIX_PRIORITY_QUEUE_MULTILAYER_RADIX_PQ_H
