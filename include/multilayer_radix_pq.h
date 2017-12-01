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
        //TODO move this inside class since it depends on runtime
        static constexpr std::pair<size_t, size_t> calculateBucket(uint64_t key, uint64_t last, size_t r) {
            //Left very explicit for now to debug
            if(key == last) {
                return {0, 0};
            }
            else{
                const size_t index_highest_significant = (std::numeric_limits<uint64_t>::digits - __builtin_clzll(key ^ last)) - 1;
                const size_t mask = size_t(1) << (index_highest_significant);
                const size_t i = floor(index_highest_significant / log2(r));
                const auto shift = static_cast<size_t>(log2(r) * i);
                const size_t j = ((key & mask) >> shift);
                return {i, j};
            }
        };

        //TODO replace with internal array bucket_minimum[][] that keeps track of bucket minimum
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

        //static constexpr auto findFirstNonEmpty(/*bucket_empty_flags_*/){};

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
        static const size_t radix = size_t(1) << RADIX_BITS;
        static const size_t no_of_buckets_ = radix-1;
        // estimate C as upper limit of key_type
        static const size_t C = std::numeric_limits<key_type>::max();
        static const int no_of_arrays_ = ceil(log2(C)/RADIX_BITS);
        key_type last_minimum_;
        bool reseeding_n_flag_;
        std::array<std::array<block_type, no_of_buckets_>, no_of_arrays_> buckets_;
        std::array<std::pair<int, std::array<bool, no_of_buckets_>>, no_of_arrays_> bucket_empty_flags_;
        block_type n_bucket_;

    public:
        explicit multilayer_radix_pq() {
            bucket_empty_flags_ = {};
            last_minimum_ = std::numeric_limits<key_type>::min();
            reseeding_n_flag_ = true;
        };

        void push(key_type key, const value_type& val) {
            // TODO replace reinterpret_cast<>(key) with encoder
            // check whether monotonicity is upheld
            assert(key >= last_minimum_);
            // if key minimum last minimum exceeds range [m, m+C] push into N  bucket
            if((key - last_minimum_) > C  & !reseeding_n_flag_){
                n_bucket_.push_back(std::pair<key_type, value_type> (key, val));
            }
            else{
                // calculate bucket indices (array, bucket)
                std::pair<uint64_t, uint64_t> pos = internal::calculateBucket(
                        reinterpret_cast<uint64_t>(key), last_minimum_, radix);
                // update bucket empty flags for calculated bucket
                bucket_empty_flags_[pos.first].first = true;
                bucket_empty_flags_[pos.first].second[pos.second] = true;
                // push key, value pair into calculated bucket
                buckets_[pos.first][pos.second].push_back(std::pair<key_type, value_type>(key, val));
            }
        }

        void pop() {

            assert(!empty());

            // find position of first non empty bucket
            std::pair<int, int> pos_minimum_element_ = index_top_element();
            // if regular buckets are empty
            if(pos_minimum_element_.first == -1){
                // check if N bucket holds elements
                if (n_bucket_.empty()) {
                    std::cout << "mlrpq empty" << std::endl;
                    return;
                }

                // temporary N bucket for elements outside [m,m+C] range
                block_type temp_n_bucket;
                // set reseeding from N flag
                reseeding_n_flag_ = 1;
                // find minimum in N bucket for reorganization
                key_type reorganization_minimum = internal::scanBucketForMinimum<key_type, block_type>(n_bucket_);
                // assign new minimum
                last_minimum_ = reorganization_minimum;
                // reorganize elements from N bucket

                for(; !n_bucket_.empty(); n_bucket_.pop_back()) {
                    // retrieve arbitrary element from current bucket
                    std::pair<key_type, value_type> temp_element = n_bucket_.back();
                    // check if element is in range [m, m+C], if not it remains in N bucket
                    if (temp_element.first < (last_minimum_ + C)) {
                        // push arbitrary element in mlrpq using the calculated minimum
                        push(temp_element.first, temp_element.second);
                    }
                        // element remains in N bucket after reseeding
                    else {
                        // push element in temporary N bucket
                        temp_n_bucket.push_back(temp_element);
                    }
                }

                // set temporary N bucket as N bucket
                n_bucket_.swap(temp_n_bucket);
                // set reseeding from N flag to false
                reseeding_n_flag_ = 0;
                // call pop again after reorganization
                pop();
            }
                // if B(0, m0) empty
            else if (pos_minimum_element_.first > 0) {
                // calculate minimum of current first non empty bucket
                key_type reorganization_minimum = internal::scanBucketForMinimum<key_type, block_type>(
                        buckets_[pos_minimum_element_.first][pos_minimum_element_.second]);

                // assign new minimum
                last_minimum_ = reorganization_minimum;

                // reorganize bucket into B(0, m0) using reorganization_minimum
                for(    ;
                        !buckets_[pos_minimum_element_.first][pos_minimum_element_.second].empty();
                        buckets_[pos_minimum_element_.first][pos_minimum_element_.second].pop_back()) {
                    // retrieve arbitrary element from current bucket
                    std::pair<key_type, value_type> temp_element = buckets_[pos_minimum_element_.first][pos_minimum_element_.second].back();
                    // push arbitrary element in mlrpq using the calculated minimum
                    push(temp_element.first, temp_element.second);
                }
                // update flags for _old_ bucket
                updateBucketEmptyFlags(pos_minimum_element_);
                // call pop again after reorganization
                pop();
            }
            else{
                // retrieve element from bucket
                std::pair<key_type, value_type> minimum_element_ =
                        buckets_[pos_minimum_element_.first][pos_minimum_element_.second].back();
                // pop element from bucket
                buckets_[pos_minimum_element_.first][pos_minimum_element_.second].pop_back();
                // check bucket empty flags and update if necessary
                updateBucketEmptyFlags(pos_minimum_element_);
                return;
            }
        }

        // update bucket empty flags if vectors were emptied
        void updateBucketEmptyFlags(const std::pair<int, int> pos_update) {
            // check if bucket is empty
            if (buckets_[pos_update.first][pos_update.second].empty()){
                bucket_empty_flags_[pos_update.first].second[pos_update.second] = 0;
                // if bucket became empty, check if whole array is empty
                // temporary variable to count empty buckets
                int temp = 0;
                for (int j = 0; j < no_of_buckets_; j++){
                    // if bucket is empty increase temp variable
                    if (buckets_[pos_update.first][j].empty()){ temp += 1; }
                }
                // if number of empty buckets equals total number of buckets assign array flag to zero
                if(temp == no_of_buckets_) { bucket_empty_flags_[pos_update.first].first = 0;}
            }
        }

        //TODO top() should access the top element
        std::pair<int,int> index_top_element() {
            //redefine as constexpr find_first_non_empty?
            for (int i = 0; i < no_of_arrays_; i++) {
                if (bucket_empty_flags_[i].first) {
                    for (int j = 0; j <= no_of_buckets_; j++){
                        if (bucket_empty_flags_[i].second[j]) {
                            return std::pair<int, int>(i,j);
                        }
                    }
                }
            }
            return std::pair<int,int>(-1,-1);
        }


        std::pair<key_type, value_type> top() {
            std::pair<int, int> pos_minimum_emelent = index_top_element();
            std::pair<key_type, value_type> minimum_element = buckets_[pos_minimum_emelent.first][pos_minimum_emelent.second].back();
            return minimum_element;
        };


        bool empty() {
            return (index_top_element() == std::pair<int,int>(-1, -1));
        }
    }; // end class multilayer_radix_pq
}


#endif //MULTILAYER_RADIX_PRIORITY_QUEUE_MULTILAYER_RADIX_PQ_H
