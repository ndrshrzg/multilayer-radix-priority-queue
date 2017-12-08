//
// Created by Andreas Herzog on 12/07/17.
// andreas.herzog@stud.uni-frankfurt.de
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

// avoid memory being used up by too large stxxl::queues
#define LIMITMEMORY


namespace multilayer_radix_pq {

    template<typename KeyType, typename ValueType, size_t RADIX_BITS>
    class multilayer_radix_pq {

    public:
        using key_type = KeyType;
        using value_type = ValueType;
#ifdef LIMITMEMORY
        static constexpr auto block_size = size_t(1) << 18;
        using block_type = stxxl::queue<std::pair<key_type, value_type>, block_size>;
#else
        using block_type = stxxl::queue<std::pair<key_type, value_type>>;
#endif

    private:
        static const size_t radix_ = size_t(1) << RADIX_BITS;
        static const size_t no_of_buckets_ = radix_;
        // estimate C as upper limit of key_type
        //static const size_t C = std::numeric_limits<key_type>::max();
        static const size_t C_ = size_t(1) << 20;
        static const size_t no_of_arrays_ = ceil(log2(C_)/RADIX_BITS) + 1;
        key_type last_minimum_;
        key_type N_bucket_minimum_;
        std::pair<int64_t, int64_t> current_minimum_index_;
        std::array<std::array<block_type, no_of_buckets_>, no_of_arrays_> buckets_;
        std::array<std::array<uint64_t, no_of_buckets_>, no_of_arrays_> bucket_minimum_;
        std::array<std::pair<bool, std::array<bool, no_of_buckets_>>, no_of_arrays_> bucket_empty_flags_;
        block_type n_bucket_;
        bool reseeding_n_flag_;
        bool first_push_flag_;


    public:
        explicit multilayer_radix_pq() {
            bucket_empty_flags_ = {};
            last_minimum_ = std::numeric_limits<key_type>::min();
            reseeding_n_flag_ = false;
            first_push_flag_ = true;
            initializeBucketMinima(no_of_arrays_, no_of_buckets_);
            N_bucket_minimum_ = std::numeric_limits<key_type>::max();
            current_minimum_index_.first = -1;
            current_minimum_index_.second = -1;

        };


        ~multilayer_radix_pq() = default;


    private:
        void initializeBucketMinima(size_t arr, size_t buck){
            // initialize array with bucket minima to be maximum
            for (int i=0; i < arr; i++){
                bucket_minimum_[i].fill(std::numeric_limits<key_type>::max());
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
            const key_type reorganization_minimum = N_bucket_minimum_;
            // assign new minimum
            last_minimum_ = reorganization_minimum;
            // reset N_bucket_minimum_
            N_bucket_minimum_ = std::numeric_limits<key_type>::max();
            // reorganize elements from N bucket
            for (; !n_bucket_.empty(); n_bucket_.pop()) {
                // retrieve arbitrary element from current bucket
                std::pair<key_type, value_type> temp_element = n_bucket_.front();
                // check if element is in range [m, m+C], if not it remains in N bucket
                if (temp_element.first < (last_minimum_ + C_)) {
                    // push arbitrary element in mlrpq using the calculated minimum
                    push(temp_element.first, temp_element.second);
                }
                    // element remains in N bucket after reseeding
                else {
                    // push element in temporary N bucket
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


        const std::pair <size_t, size_t> calculateBucket(uint64_t key, uint64_t last) const {
            if(key == last) {
                return {0, 0};
            }
            else{
                const size_t index_highest_significant = (std::numeric_limits<uint64_t>::digits - __builtin_clzll(key ^ last)) - 1;
                const size_t mask = (size_t(1) << (index_highest_significant+1))-1;
                const size_t i = floor(index_highest_significant / log2(radix_));
                const auto shift = static_cast<size_t>(log2(radix_) * i);
                const size_t j = ((key & mask) >> shift);
                return {i, j};
            }
        }


        std::pair<int64_t, int64_t> calculateMinimumIndex() {
            for (int i = 0; i < no_of_arrays_; i++) {
                if (bucket_empty_flags_[i].first) {
                    for (int j = 0; j <= no_of_buckets_; j++){
                        if (bucket_empty_flags_[i].second[j]) {
                            return std::pair<int64_t, int64_t>(i,j);
                        }
                    }
                }
            }
            return {-1,-1};
        }


        void updateCurrentMinimumIndex(){
            std::pair<int64_t, int64_t> calculated_index = calculateMinimumIndex();
            current_minimum_index_.first = calculated_index.first;
            current_minimum_index_.second = calculated_index.second;
        }


    public:
        void push(key_type key, const value_type& val) {
            // TODO replace reinterpret_cast<>(key) with encoder
            // check whether monotonicity is upheld
            assert(key >= 0);
            assert(key >= last_minimum_);
            // if key minimum last minimum exceeds range [m, m+C] push into N  bucket
            if((key - last_minimum_) > C_  & !reseeding_n_flag_){
                n_bucket_.push(std::pair<key_type, value_type> (key, val));
                if(key < N_bucket_minimum_) {N_bucket_minimum_ = key;};
                first_push_flag_ = false;
            }
            else{
                // calculate bucket indices (array, bucket)
                const auto pos = calculateBucket(
                        reinterpret_cast<uint64_t>(key), last_minimum_);
                // update bucket empty flags for calculated bucket
                bucket_empty_flags_[pos.first].first = true;
                bucket_empty_flags_[pos.first].second[pos.second] = true;
                // update bucket minimum if necessary
                if (key < bucket_minimum_[pos.first][pos.second]) {bucket_minimum_[pos.first][pos.second] = key;};
                // push key, value pair into calculated bucket
                buckets_[pos.first][pos.second].push({key, val});

                // set the current minimum index to calculated bucket of first ever push
                if (first_push_flag_) {
                    current_minimum_index_.first = pos.first;
                    current_minimum_index_.second = pos.second;
                }
                // when reseeding from N, current minimum index will always be (0, 0)
                else if (reseeding_n_flag_){
                    current_minimum_index_.first = 0;
                    current_minimum_index_.second = 0;
                }
                // update current minimum index without calling the expensive update function
                else{
                    if (pos.first <= current_minimum_index_.first){
                        if (pos.second < current_minimum_index_.second){
                            current_minimum_index_.first = pos.first;
                            current_minimum_index_.second = pos.second;
                        }
                    }
                }
                first_push_flag_ = false;
            }
        }


        void pop() {
            assert(!empty());
            // find position of first non empty bucket
            const auto pos_minimum_element_ = current_minimum_index_;
            // if regular buckets are empty
            if(pos_minimum_element_.first == -1){
                reseedFromNBucket();
                pop();
            }
            // if B(0, m0) empty
            else if (pos_minimum_element_.first > 0) {
                // get minimum of current first non empty bucket
                const key_type reorganization_minimum = bucket_minimum_[pos_minimum_element_.first][pos_minimum_element_.second];
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
                // update current minimum index
                updateCurrentMinimumIndex();
                return;
            }
        }


        const std::pair<key_type, value_type>& top() {
            assert(!empty());
            if ((current_minimum_index_ == std::pair<int64_t, int64_t> (-1, -1)) & !empty()){
                reseedFromNBucket();
                top();
            }
            //std::pair<int64_t, int64_t> pos_minimum_emelent = current_minimum_index_;
            std::pair<key_type, value_type>& minimum_element = buckets_[current_minimum_index_.first][current_minimum_index_.second].front();
            return minimum_element;
        };


        bool empty() const {
            return (current_minimum_index_ == std::pair<int64_t,int64_t>(-1, -1)) & n_bucket_.empty();
        }
    }; // end class multilayer_radix_pq
}


#endif //MULTILAYER_RADIX_PRIORITY_QUEUE_MULTILAYER_RADIX_PQ_H
