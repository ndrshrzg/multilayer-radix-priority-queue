//
// Created by ndrshrzg on 10/26/17.
//

#include <stxxl/queue>
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

    template<typename KeyType, typename ValueType, size_t RADIX_BITS>
    class multilayer_radix_pq {
    public:
        //limiting the queue size to avoid memory overflow
        static constexpr auto block_size = size_t(1) << 18;
        using key_type = KeyType;
        using value_type = ValueType;
        using block_type = stxxl::queue<std::pair<key_type, value_type>, block_size>;
        //using block_type = std::vector<std::pair<key_type, value_type>>;
    private:
        static const size_t radix = size_t(1) << RADIX_BITS;
        static const size_t no_of_buckets_ = radix;
        // estimate C as upper limit of key_type
        //static const size_t C = std::numeric_limits<key_type>::max();
        static const size_t C = size_t(1) << 18;
        static const size_t no_of_arrays_ = ceil(log2(C)/RADIX_BITS);
        key_type last_minimum_;
        key_type N_bucket_minimum_;
        bool reseeding_n_flag_;
        std::array<std::array<block_type, no_of_buckets_>, no_of_arrays_> buckets_;
        std::array<std::pair<bool, std::array<bool, no_of_buckets_>>, no_of_arrays_> bucket_empty_flags_;
        std::array<std::array<uint64_t, no_of_buckets_>, no_of_arrays_> bucket_minimum_;
        block_type n_bucket_;

    public:
        // constructor
        explicit multilayer_radix_pq() {
            bucket_empty_flags_ = {};
            last_minimum_ = std::numeric_limits<key_type>::min();
            reseeding_n_flag_ = false;
            initializeBucketMinima(no_of_arrays_, no_of_buckets_);
            N_bucket_minimum_ = std::numeric_limits<key_type>::max();
        };


        void initializeBucketMinima(size_t arr, size_t buck){
            // initialize array with bucket minima to be maximum
            for (int i=0; i < arr; i++){
                bucket_minimum_[i].fill(std::numeric_limits<key_type>::max());
            }
        }


        void push(key_type key, const value_type& val) {
            // TODO replace reinterpret_cast<>(key) with encoder
            // check whether monotonicity is upheld
            assert(key >= 0);
            assert(key >= last_minimum_);
            // if key minimum last minimum exceeds range [m, m+C] push into N  bucket
            if((key - last_minimum_) > C  & !reseeding_n_flag_){
                n_bucket_.push(std::pair<key_type, value_type> (key, val));
                if(key < N_bucket_minimum_) {N_bucket_minimum_ = key;};
                // console output for debugging
                //std::cout << "pushing into N bucket" << std::endl;
            }
            else{
                // calculate bucket indices (array, bucket)
                std::pair<uint64_t, uint64_t> pos = calculateBucket(
                        reinterpret_cast<uint64_t>(key), last_minimum_);
                // update bucket empty flags for calculated bucket
                bucket_empty_flags_[pos.first].first = true;
                bucket_empty_flags_[pos.first].second[pos.second] = true;
                // update bucket minimum if necessary
                if (key < bucket_minimum_[pos.first][pos.second]) {bucket_minimum_[pos.first][pos.second] = key;};
                // console output for debugging
                //std::cout << "pushing " << key << " into B(" << pos.first <<", " << pos.second << ")" << std::endl;
                // push key, value pair into calculated bucket
                buckets_[pos.first][pos.second].push(std::pair<key_type, value_type>(key, val));
            }
        }


        std::pair <size_t, size_t> calculateBucket(uint64_t key, uint64_t last){
            if(key == last) {
                return {0, 0};
            }
            else{
                const size_t index_highest_significant = (std::numeric_limits<uint64_t>::digits - __builtin_clzll(key ^ last)) - 1;
                const size_t mask = (size_t(1) << (index_highest_significant+1))-1;
                const size_t i = floor(index_highest_significant / log2(radix));
                const auto shift = static_cast<size_t>(log2(radix) * i);
                const size_t j = ((key & mask) >> shift);
                return {i, j};
            }
        }

        void reseedFromNBucket() {
            // re initialize bucket minima
            initializeBucketMinima(no_of_arrays_, no_of_buckets_);
            // temporary N bucket for elements outside [m,m+C] range
            block_type temp_n_bucket;
            // set reseeding from N flag
            reseeding_n_flag_ = 1;
            // retrieve minimum in N bucket for reorganization
            //key_type reorganization_minimum = bucket_minimum_[pos_minimum_element_.first][pos_minimum_element_.second];
            key_type reorganization_minimum = N_bucket_minimum_;
            // assign new minimum
            last_minimum_ = reorganization_minimum;
            // reset N_bucket_minimum_
            N_bucket_minimum_ = std::numeric_limits<key_type>::max();
            // reorganize elements from N bucket
            for (; !n_bucket_.empty(); n_bucket_.pop()) {
                // retrieve arbitrary element from current bucket
                std::pair<key_type, value_type> temp_element = n_bucket_.front();
                // check if element is in range [m, m+C], if not it remains in N bucket
                if (temp_element.first < (last_minimum_ + C)) {
                    // push arbitrary element in mlrpq using the calculated minimum
                    push(temp_element.first, temp_element.second);
                }
                    // element remains in N bucket after reseeding
                else {
                    // push element in temporary N bucket
                    // console output for debugging
                    //std::cout << "pushing " << temp_element.first << " into N bucket during reseeding phase" << std::endl;
                    temp_n_bucket.push(temp_element);
                    if (temp_element.first < N_bucket_minimum_) { N_bucket_minimum_ = temp_element.first; };
                }
            }
            // set temporary N bucket as N bucket
            n_bucket_.swap(temp_n_bucket);
            // set reseeding from N flag to false
            reseeding_n_flag_ = 0;
            // call pop again after reorganization
        }


        void pop() {
            assert(!empty());
            // find position of first non empty bucket
            std::pair<int64_t, int64_t> pos_minimum_element_ = index_top_element();
            // if regular buckets are empty
            if(pos_minimum_element_.first == -1){
                reseedFromNBucket();
                pop();
            }
            // if B(0, m0) empty
            else if (pos_minimum_element_.first > 0) {
                // get minimum of current first non empty bucket
                key_type reorganization_minimum = bucket_minimum_[pos_minimum_element_.first][pos_minimum_element_.second];
                // assign new minimum
                last_minimum_ = reorganization_minimum;
                // re-initialize bucket minima up to current bucket
                initializeBucketMinima(pos_minimum_element_.first, no_of_buckets_);
                // reorganize bucket into B(0, m0) using reorganization_minimum
                for(    ;
                        !buckets_[pos_minimum_element_.first][pos_minimum_element_.second].empty();
                        buckets_[pos_minimum_element_.first][pos_minimum_element_.second].pop()) {
                    // retrieve arbitrary element from current bucket
                    std::pair<key_type, value_type> temp_element = buckets_[pos_minimum_element_.first][pos_minimum_element_.second].front();
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
                        buckets_[pos_minimum_element_.first][pos_minimum_element_.second].front();
                // pop element from bucket
                buckets_[pos_minimum_element_.first][pos_minimum_element_.second].pop();
                // check bucket empty flags and update if necessary
                updateBucketEmptyFlags(pos_minimum_element_);
                return;
            }
        }

        // update bucket empty flags if vectors were emptied
        void updateBucketEmptyFlags(const std::pair<int64_t, int64_t> pos_update) {
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


        std::pair<int64_t, int64_t> index_top_element() {
            //redefine as constexpr find_first_non_empty?
            for (int i = 0; i < no_of_arrays_; i++) {
                if (bucket_empty_flags_[i].first) {
                    for (int j = 0; j <= no_of_buckets_; j++){
                        if (bucket_empty_flags_[i].second[j]) {
                            return std::pair<int64_t, int64_t>(i,j);
                        }
                    }
                }
            }
            return std::pair<int64_t, int64_t>(-1,-1);
        }


        std::pair<key_type, value_type> top() {
            assert(!empty());
            if ((index_top_element() == std::pair<int64_t, int64_t> (-1, -1)) & !empty()){
                reseedFromNBucket();
                top();
            }
            std::pair<int64_t, int64_t> pos_minimum_emelent = index_top_element();
            std::pair<key_type, value_type> minimum_element = buckets_[pos_minimum_emelent.first][pos_minimum_emelent.second].front();
            return minimum_element;
        };


        bool empty() {
            bool ret = (index_top_element() == std::pair<int64_t,int64_t>(-1, -1)) & n_bucket_.empty();
            return ret;
        }
    }; // end class multilayer_radix_pq
}


#endif //MULTILAYER_RADIX_PRIORITY_QUEUE_MULTILAYER_RADIX_PQ_H
