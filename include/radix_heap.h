//
// Created by ndrshrzg on 10/16/17.
// copied from https://github.com/manpen/radix-heap
//

#include <algorithm>
#include <array>
#include <cassert>
#include <climits>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>
#include <vector>
#include <tuple>

#include <stxxl/vector>
#include <stxxl/queue>

#ifndef RADIX_HEAP_RADIX_HEAP_H
#define RADIX_HEAP_RADIX_HEAP_H

namespace radix_heap {
    namespace internal {
        template<bool Is64bit> class find_bucket_impl;

        template<>
        class find_bucket_impl<false> {
        public:
            static inline constexpr size_t find_bucket(uint32_t x, uint32_t last) {
                return x == last ? 0 : 32 - __builtin_clz(x ^ last);
            }
        };

        template<>
        class find_bucket_impl<true> {
        public:
            static inline constexpr size_t find_bucket(uint64_t x, uint64_t last) {
                return x == last ? 0 : 64 - __builtin_clzll(x ^ last);
            }
        };

        template<typename T>
        inline constexpr size_t find_bucket(T x, T last) {
            return find_bucket_impl<sizeof(T) == 8>::find_bucket(x, last);
        }

        template<typename KeyType, bool IsSigned> class encoder_impl_integer;

        template<typename KeyType>
        class encoder_impl_integer<KeyType, false> {
        public:
            typedef KeyType key_type;
            typedef KeyType unsigned_key_type;

            inline static constexpr unsigned_key_type encode(key_type x) {
                return x;
            }

            inline static constexpr key_type decode(unsigned_key_type x) {
                return x;
            }
        };

        template<typename KeyType>
        class encoder_impl_integer<KeyType, true> {
        public:
            typedef KeyType key_type;
            typedef typename std::make_unsigned<KeyType>::type unsigned_key_type;

            inline static constexpr unsigned_key_type encode(key_type x) {
                return static_cast<unsigned_key_type>(x) ^
                       (unsigned_key_type(1) << unsigned_key_type(std::numeric_limits<unsigned_key_type>::digits - 1));
            }

            inline static constexpr key_type decode(unsigned_key_type x) {
                return static_cast<key_type>
                (x ^ (unsigned_key_type(1) << (std::numeric_limits<unsigned_key_type>::digits - 1)));
            }
        };

        template<typename KeyType, typename UnsignedKeyType>
        class encoder_impl_decimal {
        public:
            typedef KeyType key_type;
            typedef UnsignedKeyType unsigned_key_type;

            inline static constexpr unsigned_key_type encode(key_type x) {
                return raw_cast<key_type, unsigned_key_type>(x) ^
                       ((-(raw_cast<key_type, unsigned_key_type>(x) >> (std::numeric_limits<unsigned_key_type>::digits - 1))) |
                        (unsigned_key_type(1) << (std::numeric_limits<unsigned_key_type>::digits - 1)));
            }

            inline static constexpr key_type decode(unsigned_key_type x) {
                return raw_cast<unsigned_key_type, key_type>
                        (x ^ (((x >> (std::numeric_limits<unsigned_key_type>::digits - 1)) - 1) |
                              (unsigned_key_type(1) << (std::numeric_limits<unsigned_key_type>::digits - 1))));
            }

        private:
            template<typename T, typename U>
            union raw_cast {
            public:
                constexpr raw_cast(T t) : t_(t) {}
                operator U() const { return u_; }

            private:
                T t_;
                U u_;
            };
        };

        template<typename KeyType>
        class encoder : public encoder_impl_integer<KeyType, std::is_signed<KeyType>::value> {};

        template<>
        class encoder<float> : public encoder_impl_decimal<float, uint32_t> {};
        template<>
        class encoder<double> : public encoder_impl_decimal<double, uint64_t> {};

        template <typename UIntDecType>
        class bucket_flags {
        public:
            typedef UIntDecType unsigned_key_type;

            bucket_flags() {clear();}

            void set_empty(size_t bucket) {
                assert(bucket <= num_buckets);

                flags_ &= ~((unsigned_key_type(1) << (bucket - 1)) * (bucket != 0));  // &= is bitwise and
            }

            void set_non_empty(unsigned int bucket) {
                assert(bucket <= num_buckets);

                flags_ |= (unsigned_key_type(1) << (bucket - 1)) * (bucket != 0); // |= is bitwise or
            }

            void clear() {
                flags_ = 0;
            }

            size_t find_first_non_empty() const {
                if (sizeof(flags_) == 8)
                    return __builtin_ffsll(flags_); // how do we know this is a long long int?

                return __builtin_ffs(flags_);
            }

        private:
            unsigned_key_type flags_; // quasi bitvector flags_
            constexpr static size_t num_buckets = (sizeof(flags_) == 8 ? 64 : 32);
        };
    }  // namespace internal

    template<typename KeyType, typename ValueType, typename EncoderType = internal::encoder<KeyType>>
    class pair_radix_heap {
    public:
        typedef KeyType key_type;
        typedef ValueType value_type;
        typedef EncoderType encoder_type;
        typedef typename encoder_type::unsigned_key_type unsigned_key_type;
        typedef typename std::vector<std::pair<key_type, value_type> >::const_iterator value_iterator;

        pair_radix_heap() : size_(0), last_(), buckets_() {
            buckets_min_.fill(std::numeric_limits<unsigned_key_type>::max());
        }

        void push(key_type key, value_type &value) {
            const unsigned_key_type& x = encoder_type::encode(key);
            assert(last_ <= x);
            ++size_;
            const size_t k = internal::find_bucket(x, last_);
            buckets_[k].push_back(std::pair<key_type , value_type >(x, value));
            bucket_flags_.set_non_empty(k);
            buckets_min_[k] = std::min(buckets_min_[k], x);
        }

        key_type min_key() const {
            assert(size_ > 0);

            const size_t i = buckets_[0].empty() * bucket_flags_.find_first_non_empty();
            return encoder_type::decode(buckets_min_[i]);
        }

        key_type top_key() {
            pull();
            return encoder_type::decode(last_);
        }

        value_type &top_value() {
            pull();
            return buckets_[0].back().second;
        }

        void pop() {
            pull();
            buckets_[0].pop_back();
            --size_;
        }

        size_t size() const {
            return size_;
        }

        bool empty() const {
            return size_ == 0;
        }

    private:
        //TODO Change stxxl::vector to stxxl::queue
        size_t size_;
        unsigned_key_type last_ = std::numeric_limits<unsigned_key_type>::max();

        std::array<stxxl::vector<std::pair<unsigned_key_type, value_type>>,
                std::numeric_limits<unsigned_key_type>::digits + 1> buckets_;

        std::array<unsigned_key_type,
                std::numeric_limits<unsigned_key_type>::digits + 1> buckets_min_;


        internal::bucket_flags<unsigned_key_type> bucket_flags_;


        void pull() {
            assert(size_ > 0);
            if (!buckets_[0].empty()) return;

            const size_t i = bucket_flags_.find_first_non_empty();
            last_ = buckets_min_[i];

            for (size_t j = 0; j < buckets_[i].size(); ++j) {



                const unsigned_key_type x = buckets_[i][j].first; // stxxl::queue does not have index operator []
                const size_t k = internal::find_bucket(x, last_); // calculate according bucket



                buckets_[k].push_back(buckets_[i][j]);




                bucket_flags_.set_non_empty(k);
                buckets_min_[k] = std::min(buckets_min_[k], x);
            }

            buckets_[i].clear();
            bucket_flags_.set_empty(i);
            buckets_min_[i] = std::numeric_limits<unsigned_key_type>::max();

        }
    };

}  // namespace radix_heap

#endif //RADIX_HEAP_RADIX_HEAP_H
