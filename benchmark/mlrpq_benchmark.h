//
// Created by ndrshrzg on 1/30/18.
//

#include <random>
#include <algorithm>
#include <chrono>
#include "../include/multilayer_radix_pq.h"
#include "../libs/stxxl/include/stxxl/timer"
#include <stxxl/priority_queue>

#ifndef MULTILAYER_RADIX_PRIORITY_QUEUE_MLRPQ_BENCHMARK_H
#define MULTILAYER_RADIX_PRIORITY_QUEUE_MLRPQ_BENCHMARK_H

template <typename KeyType, typename ValueType>
class CompareKey
{
public:
    bool operator() (const std::pair<KeyType, ValueType> n1, const std::pair<KeyType, ValueType> n2) const { return n1.first > n2.first; }
    std::pair<KeyType, ValueType> min_value() const { return {std::numeric_limits<KeyType>::max(), ValueType()}; }
};

template <typename KeyType>
class CompareKeyNoPair
{
public:
    bool operator() (const KeyType n1, const KeyType n2) const { return n1 > n2; }
    KeyType min_value() const { return std::numeric_limits<KeyType>::max(); }
};

namespace benchmark {

    template <typename KeyType>
    class NumberGenerator {
        using key_type = KeyType;

    private:
        key_type range_min_, range_max_, count_;
        key_type max_generated_;
        key_type min_generated_;
        std::vector<key_type> result_;

    public:
        NumberGenerator(key_type min, key_type max, key_type count, double seed) :
                range_min_(min),
                range_max_(max),
                count_(count),
                max_generated_(std::numeric_limits<key_type>::min()),
                min_generated_(std::numeric_limits<key_type>::max())
        {

            std::random_device rd;
            generate(seed);

        };


        void generate(double seed) {
            //auto seed = std::chrono::steady_clock::now().time_since_epoch().count();

            //double seed = 123456;
            std::mt19937_64 gen(seed);
            std::uniform_int_distribution<key_type> dis(range_min_, range_max_);

            for (int i = 1; i <= count_; i++) {
                auto generated_number_ = dis(gen);

                if (max_generated_ < generated_number_) {
                    max_generated_ = generated_number_;
                };

                if (min_generated_ > generated_number_) {
                    min_generated_ = generated_number_;
                };

                result_.push_back(generated_number_);
            }

        }

        auto getGeneratedNumbers(){
            return result_;
        }

        auto getMaxGenerated(){
            return max_generated_;
        }

        auto getMinGenerated(){
            return min_generated_;
        }

    };


    template <typename KeyType, typename ValueType, size_t RADIX_BITS_>
    class allInAllOut{

        using key_type = KeyType;
        using value_type = ValueType;
        // data structures being benchmarked
        using mlrpq_type = multilayer_radix_pq::multilayer_radix_pq<key_type, value_type, RADIX_BITS_>;
        //using pq_type = std::priority_queue<std::pair<key_type, value_type>, std::vector<std::pair<key_type, value_type>>, CompareKey<key_type, value_type>>;
        typedef typename stxxl::PRIORITY_QUEUE_GENERATOR<std::pair<key_type, value_type>, CompareKey<key_type, value_type>, 16*1024*1024, 1024*1024>::result stxxl_pq_type;
        typedef typename stxxl_pq_type::block_type block_type;

    private:
        // passed by constructor
        key_type runs_;
        key_type min_key_;
        key_type max_key_;
        key_type max_segment_size_;
        key_type number_of_segments_;
        key_type current_segment_start_;

        // internals
        value_type val;

        key_type dummy_; //dummy value so pop operations are not optimized away


    public:
        allInAllOut(key_type runs, key_type min_key, key_type max_key, key_type number_of_segments, key_type max_segment_size) :
                runs_(runs),
                min_key_(min_key),
                max_key_(max_key),
                max_segment_size_(max_segment_size),
                number_of_segments_(number_of_segments),
                current_segment_start_(min_key)
        {
            // instantiate datastructures
            mlrpq_type mlrpq;
            //pq_type pq;
            const unsigned int mem_for_pools = 16 * 1024 * 1024;
            stxxl::read_write_pool<block_type> pool((mem_for_pools / 2) / block_type::raw_size,
                                                    (mem_for_pools / 2) / block_type::raw_size);
            stxxl_pq_type stxxl_pq(pool);


            double step_seed = 123456 * runs_; // change with chrono thing for random version
               
            std::cout << "code;competitiveAllInAllOut" << std::endl;
            std::cout << "radix;" << RADIX_BITS_ << std::endl;
            std::cout << "step_begin;" << runs_ << std::endl;
            execute(mlrpq, stxxl_pq, step_seed);
            std::cout << "step_end;" << runs_ << std::endl;

        };

    private:

        void execute(mlrpq_type& mlrpq, stxxl_pq_type& stxxl_pq, double step_seed){

            stxxl::timer mlrpq_watch(false);
            stxxl::timer stxxl_pq_watch(false);

            key_type size = 0;

            // generate new segment sizes
            NumberGenerator<key_type> gen_seg_sizes(1, max_segment_size_, number_of_segments_, step_seed);
            auto random_segment_sizes = gen_seg_sizes.getGeneratedNumbers();

            std::for_each(random_segment_sizes.begin(), random_segment_sizes.end(), [&] (key_type n) {
                size += n;
            });
    
            fill(mlrpq, stxxl_pq, step_seed);

	    current_segment_start_ = min_key_;

            stxxl::stats_data s1 = *stxxl::stats::get_instance();
            runStxxlPQ(stxxl_pq, stxxl_pq_watch, step_seed, random_segment_sizes, size);
            stxxl::stats_data stxxl_pq_iostat = stxxl::stats_data(*stxxl::stats::get_instance()) - s1;

            current_segment_start_ = min_key_;

            stxxl::stats_data s2 = *stxxl::stats::get_instance();
            runMLRPQ(mlrpq, mlrpq_watch, step_seed, random_segment_sizes, size);
            stxxl::stats_data mlrpq_iostat = stxxl::stats_data(*stxxl::stats::get_instance()) - s2;

            // output stats
            std::cout << "mlrpq_reads;" << mlrpq_iostat.get_reads() << std::endl;
            std::cout << "mlrpq_writes;" << mlrpq_iostat.get_writes() << std::endl;
            std::cout << "stxxl_pq_reads;" << stxxl_pq_iostat.get_reads() << std::endl;
            std::cout << "stxxl_pq_writes;" << stxxl_pq_iostat.get_writes() << std::endl;
            std::cout << "generated;" << size << std::endl;
            std::cout << "mlrpq_time;" << mlrpq_watch.mseconds() << std::endl;
            std::cout << "stxxl_pq_time;" << stxxl_pq_watch.mseconds() << std::endl;
        }

        void runMLRPQ(mlrpq_type& mlrpq, stxxl::timer &mlrpq_watch, double step_seed, const std::vector<key_type>& random_segment_sizes, const key_type size){
            for (int i = 0; i < number_of_segments_; i++){
                NumberGenerator<key_type> gen(current_segment_start_, current_segment_start_ + max_key_, random_segment_sizes[i], step_seed);
                current_segment_start_ = gen.getMaxGenerated();
                auto generated_numbers = gen.getGeneratedNumbers();
                mlrpq_watch.start();
                for (int j = 0; j < generated_numbers.size(); j++){
                    mlrpq.push(generated_numbers[i], val);
                }
                mlrpq_watch.stop();
            }

            // pop all
            mlrpq_watch.start();
            for (int i = 0; i < size; i++){
                dummy_ += mlrpq.top().first;
                mlrpq.pop();
            }
            mlrpq_watch.stop();
        }

        void runStxxlPQ(stxxl_pq_type& stxxl_pq, stxxl::timer &stxxl_pq_watch, double step_seed, const std::vector<key_type>& random_segment_sizes, const key_type size){
            for (int i = 0; i < number_of_segments_; i++){
                NumberGenerator<key_type> gen(current_segment_start_, current_segment_start_ + max_key_, random_segment_sizes[i], step_seed);
                current_segment_start_ = gen.getMaxGenerated();
                auto generated_numbers = gen.getGeneratedNumbers();
                stxxl_pq_watch.start();
                for (int j = 0; j < generated_numbers.size(); j++){
                    stxxl_pq.push({generated_numbers[i], val});
                }
                stxxl_pq_watch.stop();
            }

            // pop all
            stxxl_pq_watch.start();
            for (int i = 0; i < size; i++){
                dummy_ += stxxl_pq.top().first;
                stxxl_pq.pop();
            }
            stxxl_pq_watch.stop();
        }


        void fill(mlrpq_type& mlrpq, stxxl_pq_type& stxxl_pq, double step_seed){
            uint64_t size = size_t(1) << 25;
	    NumberGenerator<key_type> gen_fill(min_key_, max_key_, size, step_seed);
	    auto generated_fill_numbers = gen_fill.getGeneratedNumbers(); 
	    for (int i = 0; i < generated_fill_numbers.size(); i++){
		mlrpq.push(generated_fill_numbers[i], val);
		stxxl_pq.push({generated_fill_numbers[i], val});
	    }
	}

    };

    template <typename KeyType, typename ValueType, size_t RADIX_BITS_>
    class allInAllOutInterrupted{
        using key_type = KeyType;
        using value_type = ValueType;
        // data structures being benchmarked
        using mlrpq_type = multilayer_radix_pq::multilayer_radix_pq<key_type, value_type, RADIX_BITS_>;
        typedef typename stxxl::PRIORITY_QUEUE_GENERATOR<std::pair<key_type, value_type>, CompareKey<key_type, value_type>, 64*1024*1024, 1024*1024>::result stxxl_pq_type;
        typedef typename stxxl_pq_type::block_type block_type;

    private:
        // internals
        key_type runs_;
        key_type current_pushed_max_;
        key_type current_popped_max_;
        key_type offset_;

        key_type min_key_;
        key_type max_key_;
        key_type max_segment_size_;
        key_type number_of_segments_;

        key_type dummy_;

        value_type val;

    public:
        // constructor
        allInAllOutInterrupted(key_type runs,
                               key_type min_key,
                               key_type max_key,
                               key_type number_of_segments,
                               key_type max_segment_size) :
                runs_(runs),
                current_pushed_max_(min_key),
                current_popped_max_(size_t(0)),
                offset_(size_t(0)),
                min_key_(min_key),
                max_key_(max_key),
                max_segment_size_(max_segment_size),
                number_of_segments_(number_of_segments),
                dummy_(std::numeric_limits<key_type>::min())
        {
            // initialize result vectors
            // construct datastructures to pass them
            mlrpq_type mlrpq;
            const unsigned int mem_for_pools = 16 * 1024 * 1024;
            stxxl::read_write_pool<block_type> pool((mem_for_pools / 2) / block_type::raw_size,
                                                    (mem_for_pools / 2) / block_type::raw_size);
            stxxl_pq_type stxxl_pq(pool);
            
            double step_seed = 123456 * runs_;

            std::cout << "code;competitiveAllInAllOutInterrupted" << std::endl;
            std::cout << "radix;" << RADIX_BITS_ << std::endl;
            std::cout << "step_begin;" << runs_ << std::endl;
            execute(mlrpq, stxxl_pq, step_seed);
            std::cout << "step_end;" << runs_ << std::endl;
            

        }

    private:

        void execute(mlrpq_type& mlrpq, stxxl_pq_type& stxxl_pq, double step_seed){
            stxxl::timer mlrpq_watch(false);
            stxxl::timer stxxl_pq_watch(false);
            // replace this fixed seed with random double from chrono
            key_type size = 0;
            key_type popped_overall = size_t(0);

            // generate new segment sizes
            NumberGenerator<key_type> gen_seg_sizes(1, max_segment_size_, number_of_segments_, step_seed);
            auto random_segment_sizes = gen_seg_sizes.getGeneratedNumbers();

            NumberGenerator<key_type> gen_divisors(1, 10, random_segment_sizes.size(), step_seed);
            auto step_divisors = gen_divisors.getGeneratedNumbers();

            std::for_each(random_segment_sizes.begin(), random_segment_sizes.end(), [&] (int n) {
                size += n;
            });
            
            fill(mlrpq, stxxl_pq, step_seed);


            stxxl::stats_data mlrpq_iostat_start = *stxxl::stats::get_instance();
            // iterate through segments and push random numbers, pop according number of elements
            for (int i = 0; i < number_of_segments_; i++){
                NumberGenerator<key_type> gen(current_pushed_max_, current_pushed_max_ + max_key_, random_segment_sizes[i], step_seed);
                auto segment_numbers = gen.getGeneratedNumbers();
                auto count_numbers_segment_popped = (random_segment_sizes[i] + step_divisors[i] - 1) / step_divisors[i]; // ceil segment_size/divisor
                current_pushed_max_ = gen.getMaxGenerated();
                assert(current_pushed_max_ < std::numeric_limits<key_type>::max());

                // pushing segment keys into mlrpq
                mlrpq_watch.start();
                runMLRPQ(mlrpq, segment_numbers, count_numbers_segment_popped);
                mlrpq_watch.stop();
                // get the current maximum that was poppped again to ensure monotonous input in next segment
                popped_overall += count_numbers_segment_popped;
            }

            std::vector<key_type> segment_numbers_empty;
            // pop remaining numbers from queues
            mlrpq_watch.start();
            runMLRPQ(mlrpq, segment_numbers_empty, size-popped_overall);
            mlrpq_watch.stop();

            stxxl::stats_data mlrpq_iostat = stxxl::stats_data(*stxxl::stats::get_instance()) - mlrpq_iostat_start;

            current_pushed_max_ = min_key_;

            stxxl::stats_data stxxl_pq_iostat_start = *stxxl::stats::get_instance();

            for (int i = 0; i < number_of_segments_; i++) {
                NumberGenerator<key_type> gen(current_pushed_max_, current_pushed_max_ + max_key_, random_segment_sizes[i], step_seed);
                auto segment_numbers = gen.getGeneratedNumbers();
                auto count_numbers_segment_popped = (random_segment_sizes[i] + step_divisors[i] - 1) / step_divisors[i]; // ceil segment_size/divisor
                current_pushed_max_ = gen.getMaxGenerated();
                assert(current_pushed_max_ < std::numeric_limits<key_type>::max());

                stxxl_pq_watch.start();
                runStxxlPQ(stxxl_pq, segment_numbers, count_numbers_segment_popped);
                stxxl_pq_watch.stop();

                popped_overall += count_numbers_segment_popped;
            }

            // pop remaining numbers from queues
            stxxl_pq_watch.start();
            runStxxlPQ(stxxl_pq, segment_numbers_empty, size-popped_overall);
            stxxl_pq_watch.stop();

            stxxl::stats_data stxxl_pq_iostat = stxxl::stats_data(*stxxl::stats::get_instance()) - stxxl_pq_iostat_start;
            
	    //empty(mlrpq, stxxl_pq);

            std::cout << "mlrpq_reads;" << mlrpq_iostat.get_reads() << std::endl;
            std::cout << "mlrpq_writes;" << mlrpq_iostat.get_writes() << std::endl;
            std::cout << "stxxl_pq_reads;" << stxxl_pq_iostat.get_reads() << std::endl;
            std::cout << "stxxl_pq_writes;" << stxxl_pq_iostat.get_writes() << std::endl;
            std::cout << "generated;" << size << std::endl;
            std::cout << "mlrpq_time;" << mlrpq_watch.mseconds() << std::endl;
            std::cout << "stxxl_pq_time;" << stxxl_pq_watch.mseconds() << std::endl;
            // end of benchmarking procedure

        }

        void runMLRPQ(mlrpq_type& mlrpq, std::vector<key_type>& segment_numbers, size_t count_numbers_segment_popped){
            for (int i = 0; i < segment_numbers.size(); i++) {
                mlrpq.push(segment_numbers[i], val);
            }

            for (int i = 0; i < count_numbers_segment_popped; i++) {
                dummy_ += mlrpq.top().first;
                mlrpq.pop();
            }
        };

        void runStxxlPQ(stxxl_pq_type& stxxl_pq, std::vector<key_type>& segment_numbers, size_t count_numbers_segment_popped){
            for (int i = 0; i < segment_numbers.size(); i++) {
                stxxl_pq.push({segment_numbers[i], val});
            }

            for (int i = 0; i < count_numbers_segment_popped; i++) {
                dummy_ += stxxl_pq.top().first;
                stxxl_pq.pop();
            }
        };
        
        void fill(mlrpq_type& mlrpq, stxxl_pq_type& stxxl_pq, double step_seed){
            uint64_t size = size_t(1) << 25;
	    NumberGenerator<key_type> gen_fill(current_pushed_max_, current_pushed_max_ + max_key_, size, step_seed);
	    auto generated_fill_numbers = gen_fill.getGeneratedNumbers(); 
	    for (int i = 0; i < generated_fill_numbers.size(); i++){
		mlrpq.push(generated_fill_numbers[i], val);
		stxxl_pq.push({generated_fill_numbers[i], val});
	    }
	}

    };


    template <typename KeyType, typename ValueType, size_t RADIX_BITS_>
    class mlrpqOnlyAllInAllOut{ 
        using key_type = KeyType;
        using value_type = ValueType;

    private:
        // passed by constructor
        key_type runs_;
        key_type min_key_;
        key_type max_key_;
        key_type max_segment_size_;
        key_type number_of_segments_;

        // internals
        value_type val;
        key_type current_segment_start_;
        key_type dummy_;

        // data structures being benchmarked        
        
        using mlrpq_type = multilayer_radix_pq::multilayer_radix_pq<key_type, value_type, RADIX_BITS_>;
        // stxxl_pq used to validate results
        typedef typename stxxl::PRIORITY_QUEUE_GENERATOR<std::pair<key_type, value_type>, CompareKey<key_type, value_type>, 16*1024*1024, 1024*1024>::result stxxl_pq_type;
        typedef typename stxxl_pq_type::block_type block_type;


    public:
        mlrpqOnlyAllInAllOut(key_type runs,
                             key_type min_key,
                             key_type max_key,
                             key_type number_of_segments,
                             key_type max_segment_size) :
                runs_(runs),
                min_key_(min_key),
                max_key_(max_key),
                max_segment_size_(max_segment_size),
                number_of_segments_(number_of_segments),
                current_segment_start_(min_key),
                dummy_(std::numeric_limits<key_type>::min())
        {

            mlrpq_type mlrpq;

            const unsigned int mem_for_pools = 16 * 1024 * 1024;
            stxxl::read_write_pool<block_type> pool((mem_for_pools / 2) / block_type::raw_size,
                                                    (mem_for_pools / 2) / block_type::raw_size);
            stxxl_pq_type stxxl_pq(pool);

            double step_seed = 123456 * runs_;

            std::cout << "code;mlrpqAllInAllOut" << std::endl;
            std::cout << "radix;" << RADIX_BITS_ << std::endl;
            std::cout << "step_begin;" << runs_ << std::endl;
            execute(mlrpq, stxxl_pq, step_seed);
            std::cout << "step_end;" << runs_ << std::endl;
            
        };

    private:
        void execute(mlrpq_type& mlrpq, stxxl_pq_type& stxxl_pq, double step_seed){

            stxxl::timer mlrpq_watch(false);
            // replace this fixed seed with random double from chrono
            key_type size = 0;

            // generate new segment sizes
            NumberGenerator<key_type> gen_seg_sizes(1, max_segment_size_, number_of_segments_, step_seed);
            auto random_segment_sizes = gen_seg_sizes.getGeneratedNumbers();

            std::for_each(random_segment_sizes.begin(), random_segment_sizes.end(), [&] (int n) {
                size += n;
            });
            
            fill(mlrpq, step_seed);

            // start benchmarking
            current_segment_start_ = size_t(0);
            stxxl::stats_data s = *stxxl::stats::get_instance();
            run(mlrpq, mlrpq_watch, step_seed, random_segment_sizes, size);
            stxxl::stats_data iostat = stxxl::stats_data(*stxxl::stats::get_instance()) - s;

            empty(mlrpq);

            // validate that mlrpq returns correct array
            current_segment_start_ = size_t(0);
            mlrpq.reset();
            validate(mlrpq, stxxl_pq, step_seed, random_segment_sizes, size);

            // output stats
            std::cout << "reads;" << iostat.get_reads() << std::endl;
            std::cout << "writes;" << iostat.get_writes() << std::endl;
            std::cout << "generated;" << size << std::endl;
            std::cout << "mlrpq_time;" << mlrpq_watch.mseconds() << std::endl;
        }

        void validate(mlrpq_type& mlrpq, stxxl_pq_type& stxxl_pq, double step_seed, const std::vector<KeyType>& random_segment_sizes, const key_type size) {
            for (int i = 0; i < number_of_segments_; i++){
                NumberGenerator<key_type> gen(current_segment_start_, current_segment_start_ + max_key_, random_segment_sizes[i], step_seed);
                current_segment_start_ = gen.getMaxGenerated();
                auto generated_numbers = gen.getGeneratedNumbers();
                for (int i = 0; i < generated_numbers.size(); i++){
                    mlrpq.push(generated_numbers[i], val);
                    stxxl_pq.push({generated_numbers[i], val});
                }
            }

            for (int i = 0; i < size; i++){
                key_type mlrpq_tmp = mlrpq.top_key();
                key_type stxxl_pq_tmp = stxxl_pq.top().first;
                if (mlrpq_tmp != stxxl_pq_tmp) {
                    std::cout << "[ERROR] result checking failed" << std::endl;
                }
                mlrpq.pop();
                stxxl_pq.pop();
            }
            // assert that mrlpq is emptied
            assert(mlrpq.empty());
            assert(stxxl_pq.empty());
        }

        void run(mlrpq_type& mlrpq, stxxl::timer &mlrpq_watch, double step_seed, const std::vector<KeyType>& random_segment_sizes, const key_type size) {
            for (int i = 0; i < number_of_segments_; i++){
                NumberGenerator<key_type> gen(current_segment_start_, current_segment_start_ + max_key_, random_segment_sizes[i], step_seed);
                current_segment_start_ = gen.getMaxGenerated();
                auto generated_numbers = gen.getGeneratedNumbers();
                mlrpq_watch.start();
                for (int i = 0; i < generated_numbers.size(); i++){
                    mlrpq.push(generated_numbers[i], val);
                }
                mlrpq_watch.stop();
            }

            // pop all
            mlrpq_watch.start();
            for (int i = 0; i < size; i++){
                dummy_ += mlrpq.top().first;
                mlrpq.pop();
            }
            mlrpq_watch.stop();
        }
        
        void fill(mlrpq_type& mlrpq, double step_seed){
            uint64_t size = size_t(1) << 25;
	    NumberGenerator<key_type> gen_fill(min_key_, max_key_, size, step_seed);
	    auto generated_fill_numbers = gen_fill.getGeneratedNumbers(); 
	    for (int i = 0; i < generated_fill_numbers.size(); i++){
		mlrpq.push(generated_fill_numbers[i], val);
	    }
	}
	
	void empty(mlrpq_type& mlrpq){
	    
	    for (size_t i = 0; i < (size_t(1) << 25); i++){
	        mlrpq.pop();
	    }    
	    while(!mlrpq.empty()){
		mlrpq.pop();
	    }
	}


    };


    template <typename KeyType, typename ValueType, size_t RADIX_BITS_>
    class mlrpqOnlyAllInAllOutInterrupted{
        using key_type = KeyType;
        using value_type = ValueType;

        using mlrpq_type = multilayer_radix_pq::multilayer_radix_pq<key_type, value_type, RADIX_BITS_>;
        // stxxl_pq used to validate results
        typedef typename stxxl::PRIORITY_QUEUE_GENERATOR<std::pair<key_type, value_type>, CompareKey<key_type, value_type>, 16*1024*1024, 1024*1024>::result stxxl_pq_type;
        typedef typename stxxl_pq_type::block_type block_type;

    private:
        // internals
        key_type runs_;
        key_type current_pushed_max_;
        key_type current_popped_max_;
        key_type offset_;

        key_type min_key_;
        key_type max_key_;
        key_type max_segment_size_;
        key_type number_of_segments_;

        key_type dummy_;

        value_type val;

    public:
        // constructor
        mlrpqOnlyAllInAllOutInterrupted(key_type runs,
                               key_type min_key,
                               key_type max_key,
                               key_type number_of_segments,
                               key_type max_segment_size) : runs_(runs),
                                                                current_pushed_max_(min_key),
                                                                current_popped_max_(size_t(0)),
                                                                offset_(size_t(0)),
                                                                min_key_(min_key),
                                                                max_key_(max_key),
                                                                max_segment_size_(max_segment_size),
                                                                number_of_segments_(number_of_segments),
                                                                dummy_(std::numeric_limits<key_type>::min())
        {
            // initialize result vectors
            // construct datastructures to pass them
            mlrpq_type mlrpq;
            const unsigned int mem_for_pools = 16 * 1024 * 1024;
            stxxl::read_write_pool<block_type> pool((mem_for_pools / 2) / block_type::raw_size,
                                                        (mem_for_pools / 2) / block_type::raw_size);
            stxxl_pq_type stxxl_pq(pool);

            double step_seed = 123456 * runs_;


            std::cout << "code;mlrpqAllInAllOutInterrupted" << std::endl;
            std::cout << "radix;" << RADIX_BITS_ << std::endl;
            std::cout << "step_begin;" << runs_ << std::endl;
            execute(mlrpq, stxxl_pq, step_seed);
            std::cout << "step_end;" << runs_ << std::endl;
        
        }

    private:

        void execute(mlrpq_type& mlrpq, stxxl_pq_type& stxxl_pq, double step_seed){
            stxxl::timer mlrpq_watch(false);
            // replace this fixed seed with random double from chrono
            key_type size = 0;
            key_type popped_overall = size_t(0);

            // generate new segment sizes
            NumberGenerator<key_type> gen_seg_sizes(1, max_segment_size_, number_of_segments_, step_seed);
            auto random_segment_sizes = gen_seg_sizes.getGeneratedNumbers();

            NumberGenerator<key_type> gen_divisors(1, 10, random_segment_sizes.size(), step_seed);
            auto step_divisors = gen_divisors.getGeneratedNumbers();

            std::for_each(random_segment_sizes.begin(), random_segment_sizes.end(), [&] (int n) {
                size += n;
            });
            
            fill(mlrpq, step_seed);
            
            stxxl::stats_data iostat_start = *stxxl::stats::get_instance();
            // iterate through segments and push random numbers, pop according number of elements
            for (int i = 0; i < number_of_segments_; i++){
                NumberGenerator<key_type> gen(current_pushed_max_, current_pushed_max_ + max_key_, random_segment_sizes[i], step_seed);
                auto segment_numbers = gen.getGeneratedNumbers();
                auto count_numbers_segment_popped = (random_segment_sizes[i] + step_divisors[i] - 1) / step_divisors[i]; // ceil segment_size/divisor
                current_pushed_max_ = gen.getMaxGenerated();
                assert(current_pushed_max_ < std::numeric_limits<key_type>::max());

                // pushing segment keys into mlrpq
                mlrpq_watch.start();
                run(mlrpq, segment_numbers, count_numbers_segment_popped);
                mlrpq_watch.stop();
                // get the current maximum that was poppped again to ensure monotonous input in next segment
                popped_overall += count_numbers_segment_popped;
            }
            std::vector<key_type> segment_numbers_empty;
            // pop remaining numbers from queues
            mlrpq_watch.start();
            run(mlrpq, segment_numbers_empty, size-popped_overall);
            mlrpq_watch.stop();

            stxxl::stats_data iostat = stxxl::stats_data(*stxxl::stats::get_instance()) - iostat_start;

            std::cout << "generated;" << size << std::endl;
            std::cout << "mlrpq_time;" << mlrpq_watch.mseconds() << std::endl;
            std::cout << "reads;" << iostat.get_reads() << std::endl;
            std::cout << "writes;" << iostat.get_writes() << std::endl;
            // end of benchmarking procedure

            empty(mlrpq);
            mlrpq.reset();
            popped_overall = size_t(0);
            current_pushed_max_ = min_key_;

            // validation of segments
            for (int i = 0; i < number_of_segments_; i++){
                NumberGenerator<key_type> gen(current_pushed_max_, current_pushed_max_ + max_key_, random_segment_sizes[i], step_seed);
                auto segment_numbers = gen.getGeneratedNumbers();
                auto count_numbers_segment_popped = (random_segment_sizes[i] + step_divisors[i] - 1) / step_divisors[i]; // ceil segment_size/divisor
                current_pushed_max_ = gen.getMaxGenerated();
                validate(mlrpq, stxxl_pq, segment_numbers, count_numbers_segment_popped);
                popped_overall += count_numbers_segment_popped;
            }
            segment_numbers_empty;
            // validate remaining
            validate(mlrpq, stxxl_pq, segment_numbers_empty, size-popped_overall);

        }

        void run(mlrpq_type& mlrpq, std::vector<key_type>& segment_numbers, size_t count_numbers_segment_popped){
            for (int i = 0; i < segment_numbers.size(); i++) {
                mlrpq.push(segment_numbers[i], val);
            }

            for (int i = 0; i < count_numbers_segment_popped; i++) {
                dummy_ += mlrpq.top().first;
                mlrpq.pop();
            }
        };
        
        void fill(mlrpq_type& mlrpq, double step_seed){
            uint64_t size = size_t(1) << 25;
	    NumberGenerator<key_type> gen_fill(current_pushed_max_, current_pushed_max_ + max_key_, size, step_seed);
	    auto generated_fill_numbers = gen_fill.getGeneratedNumbers(); 
	    for (int i = 0; i < generated_fill_numbers.size(); i++){
		mlrpq.push(generated_fill_numbers[i], val);
	    }
	}
	
	void empty(mlrpq_type& mlrpq){
	    
	    for (size_t i = 0; i < (size_t(1) << 24); i++){
	        mlrpq.pop();
	    }    
	    while(!mlrpq.empty()){
		mlrpq.pop();
	    }
	}
        

        void validate(mlrpq_type& mlrpq, stxxl_pq_type& stxxl_pq, std::vector<key_type>& segment_numbers, size_t count_numbers_segment_popped){
            key_type mlrpq_tmp;
            key_type stxxl_pq_tmp;

            for (int i = 0; i < segment_numbers.size(); i++) {
                mlrpq.push(segment_numbers[i], val);
                stxxl_pq.push({segment_numbers[i], val});
            }

            for (int i = 0; i < count_numbers_segment_popped; i++) {
                mlrpq_tmp = mlrpq.top_key();
                stxxl_pq_tmp = stxxl_pq.top().first;

                if (mlrpq_tmp != stxxl_pq_tmp){
                    std::cout << "[ERROR] result checking failed" << std::endl;
                }

                mlrpq.pop();
                stxxl_pq.pop();
            }

        }

    };
    
    
        
    template <typename KeyType, typename ValueType, size_t RADIX_BITS_, size_t C_BITS_>
    class CAllInAllOut{ 
        using key_type = KeyType;
        using value_type = ValueType;

    private:
        // passed by constructor
        key_type runs_;
        key_type min_key_;
        key_type max_key_;
        key_type max_segment_size_;
        key_type number_of_segments_;

        // internals
        value_type val;
        key_type current_segment_start_;
        key_type dummy_;

        // data structures being benchmarked        
        
        using mlrpq_type = multilayer_radix_pq::multilayer_radix_pq<key_type, value_type, RADIX_BITS_, (size_t(1) << C_BITS_), size_t(1) << 10>;
        // stxxl_pq used to validate results
        typedef typename stxxl::PRIORITY_QUEUE_GENERATOR<std::pair<key_type, value_type>, CompareKey<key_type, value_type>, 16*1024*1024, 1024*1024>::result stxxl_pq_type;
        typedef typename stxxl_pq_type::block_type block_type;


    public:
        CAllInAllOut(key_type runs,
                             key_type min_key,
                             key_type max_key,
                             key_type number_of_segments,
                             key_type max_segment_size) :
                runs_(runs),
                min_key_(min_key),
                max_key_(max_key),
                max_segment_size_(max_segment_size),
                number_of_segments_(number_of_segments),
                current_segment_start_(min_key),
                dummy_(std::numeric_limits<key_type>::min())
        {

            mlrpq_type mlrpq;

            const unsigned int mem_for_pools = 16 * 1024 * 1024;
            stxxl::read_write_pool<block_type> pool((mem_for_pools / 2) / block_type::raw_size,
                                                    (mem_for_pools / 2) / block_type::raw_size);
            stxxl_pq_type stxxl_pq(pool);

            double step_seed = 123456 * runs_;

            std::cout << "code;mlrpqAllInAllOut" << std::endl;
            std::cout << "radix;" << RADIX_BITS_ << std::endl;
            std::cout << "step_begin;" << runs_ << std::endl;
            execute(mlrpq, stxxl_pq, step_seed);
            std::cout << "step_end;" << runs_ << std::endl;
            
        };

    private:
        void execute(mlrpq_type& mlrpq, stxxl_pq_type& stxxl_pq, double step_seed){

            stxxl::timer mlrpq_watch(false);
            // replace this fixed seed with random double from chrono
            key_type size = 0;

            // generate new segment sizes
            //NumberGenerator<key_type> gen_seg_sizes(1, max_segment_size_, number_of_segments_, step_seed);
            //auto random_segment_sizes = gen_seg_sizes.getGeneratedNumbers();

            std::vector<key_type> random_segment_sizes(number_of_segments_, max_segment_size_);


            std::for_each(random_segment_sizes.begin(), random_segment_sizes.end(), [&] (int n) {
                size += n;
            });
            
            fill(mlrpq, step_seed);

            // start benchmarking
            current_segment_start_ = min_key_;
            stxxl::stats_data s = *stxxl::stats::get_instance();
            run(mlrpq, mlrpq_watch, step_seed, random_segment_sizes, size);
            stxxl::stats_data iostat = stxxl::stats_data(*stxxl::stats::get_instance()) - s;

            empty(mlrpq);

            // validate that mlrpq returns correct array
            //current_segment_start_ = min_key_;
            //mlrpq.reset();
            //validate(mlrpq, stxxl_pq, step_seed, random_segment_sizes, size);

            // output stats
            std::cout << "reads;" << iostat.get_reads() << std::endl;
            std::cout << "writes;" << iostat.get_writes() << std::endl;
            std::cout << "generated;" << size << std::endl;
            std::cout << "mlrpq_time;" << mlrpq_watch.mseconds() << std::endl;
        }

        void validate(mlrpq_type& mlrpq, stxxl_pq_type& stxxl_pq, double step_seed, const std::vector<KeyType>& random_segment_sizes, const key_type size) {
            for (int i = 0; i < number_of_segments_; i++){
                NumberGenerator<key_type> gen(min_key_, max_key_, random_segment_sizes[i], step_seed);
                //current_segment_start_ = gen.getMaxGenerated();
                auto generated_numbers = gen.getGeneratedNumbers();
                for (int i = 0; i < generated_numbers.size(); i++){
                    mlrpq.push(generated_numbers[i], val);
                    stxxl_pq.push({generated_numbers[i], val});
                }
            }

            for (int i = 0; i < size; i++){
                key_type mlrpq_tmp = mlrpq.top_key();
                key_type stxxl_pq_tmp = stxxl_pq.top().first;
                if (mlrpq_tmp != stxxl_pq_tmp) {
                    std::cout << "[ERROR] result checking failed" << std::endl;
                }
                mlrpq.pop();
                stxxl_pq.pop();
            }
            // assert that mrlpq is emptied
            assert(mlrpq.empty());
            assert(stxxl_pq.empty());
        }

        void run(mlrpq_type& mlrpq, stxxl::timer &mlrpq_watch, double step_seed, const std::vector<KeyType>& random_segment_sizes, const key_type size) {
            for (int i = 0; i < number_of_segments_; i++){
                NumberGenerator<key_type> gen(min_key_, max_key_, random_segment_sizes[i], step_seed);
                //current_segment_start_ = gen.getMaxGenerated();
                auto generated_numbers = gen.getGeneratedNumbers();
                mlrpq_watch.start();
                for (int i = 0; i < generated_numbers.size(); i++){
                    mlrpq.push(generated_numbers[i], val);
                }
                mlrpq_watch.stop();
            }

            // pop all
            mlrpq_watch.start();
            for (int i = 0; i < size; i++){
                dummy_ += mlrpq.top().first;
                std::cout << dummy_ << std::endl;
                mlrpq.pop();
            }
            mlrpq_watch.stop();
            std::cout << "finished popping, mlrpq empty: " << mlrpq.empty() << std::endl;
        }
        
        void fill(mlrpq_type& mlrpq, double step_seed){
            uint64_t size = size_t(1) << 25;
	    NumberGenerator<key_type> gen_fill(min_key_, max_key_, size, step_seed);
	    auto generated_fill_numbers = gen_fill.getGeneratedNumbers(); 
	    for (int i = 0; i < generated_fill_numbers.size(); i++){
		mlrpq.push(generated_fill_numbers[i], val);
	    }
	}
	
	void empty(mlrpq_type& mlrpq){
	    std::cout << "emptying mlrpq" << std::endl;
	    
	    for (size_t i = 0; i < (size_t(1) << 25); i++){
	        std::cout << mlrpq.top_key() << std::endl;
	        mlrpq.pop();
	    }    
	    while(!mlrpq.empty()){
		mlrpq.pop();
	    }
	    std::cout << "mlrpq empty: " << mlrpq.empty() << std::endl;
	}


    };


}; // end namespace benchmark


#endif //MULTILAYER_RADIX_PRIORITY_QUEUE_MLRPQ_BENCHMARK_H
