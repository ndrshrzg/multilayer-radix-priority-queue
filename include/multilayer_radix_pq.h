//
// Created by Andreas Herzog on 12/07/17.
// andreas.herzog@stud.uni-frankfurt.de
//

//#include <stxxl/queue>
#include <queue>
#include <array>
#include <cmath>
#include <limits>
#include <iterator>
#include <cassert>
#include <climits>
#include <bitset>

#include <iostream>

#ifndef MULTILAYER_RADIX_PRIORITY_QUEUE_MULTILAYER_RADIX_PQ_H
#define MULTILAYER_RADIX_PRIORITY_QUEUE_MULTILAYER_RADIX_PQ_H

#define LIMITMEMORY
//#define COMPILERAGNOSTIC

#define dev_temp

namespace internal {

    template<typename KeyType>
    class switching{
    public:
        using key_type = KeyType;

        static constexpr size_t index_highest_significant (key_type key, key_type last){
#ifdef COMPILERAGNOSTIC
            auto key_xor_last = key ^ last;
            auto digits = std::numeric_limits<key_type>::digits;

            size_t index = 0;
            for (size_t i = sizeof(key_xor_last) * CHAR_BIT; i--; )
            {
                if ((key_xor_last >> i) == 1){
                    return (digits - index) - 1;
                }
                index++;
            }
            return (digits - index) - 1;
#else
            return (std::numeric_limits<uint64_t>::digits - __builtin_clzll(key ^ last)) - 1;
#endif
        }
        static constexpr auto index_least_significant(key_type empty_flag){
            return __builtin_ffsl(empty_flag);

        }

        static constexpr auto index_least_significant_zero_bucket(key_type empty_flag){
            if (empty_flag == 0){
                return -1;
            }
            else{
                return __builtin_ffsl(empty_flag)-1;
            }
        }
    };
}



namespace multilayer_radix_pq {
    //template<typename KeyType, typename ValueType, size_t RADIX_BITS, KeyType C = std::numeric_limits<KeyType>::max(), bool ALLOW_N_OVERFLOW = false>
    template<typename KeyType, typename ValueType, size_t RADIX_BITS, bool ALLOW_N_OVERFLOW = false>
    class multilayer_radix_pq {

    public:
        using key_type = KeyType;
        using value_type = ValueType;
#ifdef LIMITMEMORY //has no effect in no-stxxl branch
        //static constexpr auto block_size = size_t(1) << 18;
        using block_type = std::queue<std::pair<key_type, value_type>>;
#else
        using block_type = stxxl::queue<std::pair<key_type, value_type>>;
#endif


    private:
        static constexpr size_t radix_ = size_t(1) << RADIX_BITS;
        static const size_t no_of_buckets_ = radix_;
        /// TODO C as template parameter
        static const key_type C = std::numeric_limits<key_type>::max();
        static const auto C_BITS_ = sizeof(C)*8;
        static const size_t no_of_arrays_ = (C_BITS_/RADIX_BITS)+1;
        key_type last_minimum_;
        key_type N_bucket_minimum_;
        std::pair<int64_t, int64_t> current_minimum_index_;
        std::array<std::array<block_type, no_of_buckets_>, no_of_arrays_> buckets_;
        std::array<std::array<key_type, no_of_buckets_>, no_of_arrays_> bucket_minimum_;
        key_type bucket_empty_flags_first_level_;
        std::array<key_type, no_of_arrays_> bucket_empty_flags_second_level_;
        block_type n_bucket_;
        bool reseeding_n_flag_;
        bool first_push_flag_;

    public:
        explicit multilayer_radix_pq() :
                last_minimum_(std::numeric_limits<key_type>::min()),
                reseeding_n_flag_(false),
                first_push_flag_(true),
                N_bucket_minimum_(std::numeric_limits<key_type>::max()),
                current_minimum_index_({-1, -1}),
                bucket_empty_flags_first_level_(0),
                bucket_empty_flags_second_level_({0})
        {
            initializeBucketMinima(no_of_arrays_);
        };


    private:
        void initializeBucketMinima(size_t arr){
            // initialize array with bucket minima to be maximum
            for (int i=0; i < arr; i++){
                bucket_minimum_[i].fill(std::numeric_limits<key_type>::max());
            }
        }

        void reseedFromNBucket() {
            assert(!n_bucket_.empty());
            // re initialize bucket minima
            initializeBucketMinima(no_of_arrays_);
            // set reseeding from N flag
            reseeding_n_flag_ = 1;
            // retrieve minimum in N bucket for reorganization
            const key_type reorganization_minimum = N_bucket_minimum_;
            // assign new minimum
            last_minimum_ = reorganization_minimum;
            // reset N_bucket_minimum_
            N_bucket_minimum_ = std::numeric_limits<key_type>::max();
            // reorganize elements from N bucket

            if (ALLOW_N_OVERFLOW){
                // temporary N bucket for elements outside [m,m+C] range
                block_type temp_n_bucket;
                for (; !n_bucket_.empty(); n_bucket_.pop()) {
                    // retrieve arbitrary element from current bucket
                    std::pair<key_type, value_type> temp_element = n_bucket_.front();
                    // check if element is in range [m, m+C], if not it remains in N bucket

                    if ((temp_element.first - last_minimum_) < C) {
                        // push arbitrary element in mlrpq using the calculated minimum
                        push(temp_element.first, temp_element.second);

                    } else {
                        // element remains in N bucket after reseeding
                        // push element in temporary N bucket
                        temp_n_bucket.push(temp_element);
                        if (temp_element.first < N_bucket_minimum_) {
                            N_bucket_minimum_ = temp_element.first;
                        };
                    }
                }
                // set temporary N bucket as N bucket
                n_bucket_.swap(temp_n_bucket);
            }
            else{
                for (; !n_bucket_.empty(); n_bucket_.pop()) {
                    std::pair<key_type, value_type> temp_element = n_bucket_.front();
                    push(temp_element.first, temp_element.second);
                }

            }


            // set reseeding from N flag to false
            reseeding_n_flag_ = 0;
            // call pop again after reorganization
        }

        void updateBucketEmptyFlags(const std::pair<int64_t, int64_t> pos_update) {
            if (pos_update.first == 0){
                // after reorganization xor the bit of now empty bucket to be zero
                bucket_empty_flags_second_level_[pos_update.first] ^= (1 << pos_update.second);
                // if array is empty, xor the bit in first level flags to zero
                if (bucket_empty_flags_second_level_[pos_update.first] == 0){
                    bucket_empty_flags_first_level_ ^= (1 << pos_update.first);
                }
            }
            else{
                // offset the shift by -1, Buckets B(k, 0) k >= 1 are always empty, to reflect
                // this behavior in bit fields the shift is needed
                bucket_empty_flags_second_level_[pos_update.first] ^= (1 << (pos_update.second));
                // if array is empty, xor the bit in first level flags to zero
                if (bucket_empty_flags_second_level_[pos_update.first] == 0) {
                    bucket_empty_flags_first_level_ ^= (1 << (pos_update.first));
                }
            }
        }

        const std::pair <size_t, size_t> calculateBucket(uint64_t key, uint64_t last) const {
            if(key == last) {
                return {0, 0};
            }
            else{
                const size_t index_highest_significant = internal::switching<key_type>::index_highest_significant(key, last);
                const size_t mask = (size_t(1) << (index_highest_significant+1))-1;
                const size_t i = index_highest_significant/RADIX_BITS;
                const auto shift = static_cast<size_t>(log2(radix_) * i);
                const size_t j = ((key & mask) >> shift);
                return {i, j};
            }
        }

        // manpen: const this-pointer!
        std::pair<int64_t, int64_t> calculateMinimumIndex() {
            std::pair<int64_t, int64_t> res_bitwise {};

            res_bitwise.first = internal::switching<key_type>::index_least_significant_zero_bucket(bucket_empty_flags_first_level_);

            if (res_bitwise.first == 0){
                res_bitwise.second = internal::switching<key_type>::index_least_significant_zero_bucket(bucket_empty_flags_second_level_[res_bitwise.first]);
            }
            else{
                res_bitwise.second = internal::switching<key_type>::index_least_significant(bucket_empty_flags_second_level_[res_bitwise.first]);
                std::cout << res_bitwise.first << " " << res_bitwise.second << std::endl;
            }

            return res_bitwise;
        }


        void updateCurrentMinimumIndex(){
            // manpe.n: why not directly: current_minimum_index_ = calculateMinimumIndex()?
            // aherzog: assignment operator for std::pair has been removed after c++11
            std::pair<int64_t, int64_t> calculated_index = calculateMinimumIndex();
            current_minimum_index_.first = calculated_index.first;
            current_minimum_index_.second = calculated_index.second;
        }


    public:
        void push(key_type key, const value_type& val) {
            // manpen: replace static_cast<>(key) with encoder
            // encoder from radix-heap is not a drop in since it does not account for assertions against C
            // pushing 0 eg results in failure since the encoded version of 0 is a larger number than C
            // check whether monotonicity is upheld
            assert(key >= 0);
            assert(key >= last_minimum_);
            assert((key - last_minimum_) < C);
            // if
            if(!ALLOW_N_OVERFLOW){
                assert((key - last_minimum_) < C);
            }

            // if key minimum last minimum exceeds range [m, m+C] push into N  bucket
            if((key > C) && ((key - last_minimum_) < C)  && !reseeding_n_flag_){
                n_bucket_.push({key, val});
                if(key < N_bucket_minimum_) {
                    N_bucket_minimum_ = key;
                };
                first_push_flag_ = false;
                return;
            }
            // calculate bucket indices (array, bucket);
            const auto pos = calculateBucket(static_cast<uint64_t>(key), last_minimum_);

            // update bucket empty flags for calculated bucket
            bucket_empty_flags_first_level_ |= (1 << pos.first);
            bucket_empty_flags_second_level_[pos.first] |= (1 << pos.second); //TODO check if this works for flags larger than 32 bits on server, my computer fails

            // update bucket minimum if necessary
            if (key < bucket_minimum_[pos.first][pos.second]) {
                bucket_minimum_[pos.first][pos.second] = key;
            };

            // push key, value pair into calculated bucket
            buckets_[pos.first][pos.second].push({key, val});

            // manpen: You should avoid ifs that need to be checked for every
            // element but are only relevant for the first/last item. In this
            // case you can simply initialise current_min_index with num_lim::max,
            // or am I missing something? Also try to get rid of the reseeding case.

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


        void pop() {
            assert(!empty());
            // find position of first non empty bucket
            const auto pos_minimum_element_ = current_minimum_index_;

            // if regular buckets are empty
            if(pos_minimum_element_.first == -1){
                reseedFromNBucket();
                pop();
                return;
            }

            // if B(0, m0) empty
            if (pos_minimum_element_.first > 0) {
                // get minimum of current first non empty bucket
                const key_type reorganization_minimum = bucket_minimum_[pos_minimum_element_.first][pos_minimum_element_.second];

                auto& min_bucket = buckets_[pos_minimum_element_.first][pos_minimum_element_.second];

                // assign new minimum
                last_minimum_ = reorganization_minimum;

                // re-initialize bucket minima up to current bucket
                initializeBucketMinima(pos_minimum_element_.first);

                // reorganize bucket into B(0, m0) using reorganization_minimum
                for(    ;
                        !min_bucket.empty();
                        min_bucket.pop()) {
                    // retrieve arbitrary element from current bucket
                    const auto& temp_element = min_bucket.front();
                    // push arbitrary element in mlrpq using the calculated minimum
                    push(temp_element.first, temp_element.second);
                }

                // update flags for _old_ bucket
                updateBucketEmptyFlags(pos_minimum_element_);
                // reinitialize the bucket minimum of the just emptied bucket
                bucket_minimum_[pos_minimum_element_.first][pos_minimum_element_.second] = std::numeric_limits<key_type>::max();
                // call pop again after reorganization
                pop();
                return;
            }

            // retrieve element from bucket
            std::pair<key_type, value_type> minimum_element_ =
                    buckets_[pos_minimum_element_.first][pos_minimum_element_.second].front();
            // pop element from bucket
            buckets_[pos_minimum_element_.first][pos_minimum_element_.second].pop();

            if (buckets_[pos_minimum_element_.first][pos_minimum_element_.second].empty()){
                // check bucket empty flags and update if necessary
                updateBucketEmptyFlags(pos_minimum_element_);
            }

            // update current minimum index
            updateCurrentMinimumIndex();

        }


        const std::pair<key_type, value_type>& top() {
            assert(!empty());
            // manpe.n: Also, it seems that !empty() suffices as it includes the first condition
            // anherzog: the first condition is needed, otherwise it tries to reseed from an empty bucket
            if ((current_minimum_index_ == std::pair<int64_t, int64_t> (-1, -1)) && !empty()){
                reseedFromNBucket();
                top();
            }
            std::pair<key_type, value_type>& minimum_element = buckets_[current_minimum_index_.first][current_minimum_index_.second].front();
            return minimum_element;
        };


        const key_type top_const() const {
            if (current_minimum_index_== std::pair<int64_t, int64_t> (-1, -1) && !n_bucket_.empty()){
                return N_bucket_minimum_;
            }
            else{
                const std::pair<key_type, value_type> minimum_element = buckets_[current_minimum_index_.first][current_minimum_index_.second].front();
                return minimum_element.first;
            }
        };

        bool empty() const {
            return (current_minimum_index_ == std::pair<int64_t,int64_t>(-1, -1)) && n_bucket_.empty();
        }

    }; // end class multilayer_radix_pq
}


#endif //MULTILAYER_RADIX_PRIORITY_QUEUE_MULTILAYER_RADIX_PQ_H