//
// Created by ndrshrzg on 1/30/18.
//

#include <random>
#include <chrono>
#include "../include/multilayer_radix_pq.h"
#include "../include/radix_heap.h"

#ifndef MULTILAYER_RADIX_PRIORITY_QUEUE_MLRPQ_BENCHMARK_H
#define MULTILAYER_RADIX_PRIORITY_QUEUE_MLRPQ_BENCHMARK_H

namespace mlrpq_benchmark_comparator{
    template <typename KeyType, typename ValueType>
    class CompareKey
    {
    public:
        bool operator() (const std::pair<KeyType, ValueType> n1, const std::pair<KeyType, ValueType> n2) const { return n1.first > n2.first; }
    };
}

namespace mlrpq_benchmark_time{
    class Stopwatch{
    private:
        std::chrono::steady_clock::duration current_;
        std::chrono::steady_clock::duration overall_;
        std::chrono::steady_clock::time_point start_;
        std::chrono::steady_clock::time_point stop_;

    public:
        Stopwatch(){};


        void start(){
            start_ = std::chrono::steady_clock::now();
        };
        void stop(){
            stop_ = std::chrono::steady_clock::now();
            current_ = stop_ - start_;
            overall_ += current_;
        };

        long getCurrentTime(){
            return std::chrono::duration_cast<std::chrono::nanoseconds>(current_).count();
        }
        long getTotalTime(){
            return std::chrono::duration_cast<std::chrono::nanoseconds>(overall_).count();
        }

    };
}


namespace benchmark {
    template <typename KeyType>
    class NumberGenerator {
        using key_type = KeyType;

    private:
        key_type range_min_, range_max_, count_;
        key_type max_generated_;
        std::vector<key_type> result_;

    public:
        NumberGenerator(key_type min, key_type max, key_type count) :
                range_min_(min),
                range_max_(max),
                count_(count),
                max_generated_(std::numeric_limits<key_type>::min())
        {

            std::random_device rd;
            generate();

        };


        void generate() {
            auto seed = std::chrono::steady_clock::now().time_since_epoch().count();
            std::mt19937_64 gen(seed);
            std::uniform_int_distribution<key_type> dis(range_min_, range_max_);

            for (int i = 1; i <= count_; i++) {
                auto generated_number_ = dis(gen);

                if (max_generated_ < generated_number_) {
                    max_generated_ = generated_number_;
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

    };

    template <typename KeyType, typename ValueType>
    class allInAllOut{
        using key_type = KeyType;
        using value_type = ValueType;

    private:
        // passed by constructor
        key_type runs_;
        key_type max_key_;
        key_type max_segment_size_;
        key_type number_of_segments_;
        key_type current_segment_start_;

        // internals
        std::vector<key_type> step_numbers_;
        key_type total_count_;
        value_type val;

        std::vector<key_type> random_segment_sizes_;

        // data structures being benchmarked
        multilayer_radix_pq::multilayer_radix_pq<key_type, value_type, 5> mlrpq;
        radix_heap::pair_radix_heap<key_type, value_type> rpq;
        std::priority_queue<std::pair<key_type, value_type>, std::vector<std::pair<key_type, value_type>>, mlrpq_benchmark_comparator::CompareKey<key_type, value_type>> pq;


    public:
        allInAllOut(key_type runs, key_type max_key, key_type max_segment_size, key_type number_of_segments) :
                runs_(runs),
                max_key_(max_key),
                max_segment_size_(max_segment_size),
                number_of_segments_(number_of_segments),
                current_segment_start_(size_t(0))
        {

            execute();

        };

    private:
        void execute(){
            mlrpq_benchmark_time::Stopwatch mlrpq_watch;
            mlrpq_benchmark_time::Stopwatch rpq_watch;
            mlrpq_benchmark_time::Stopwatch pq_watch;


            for (int i = 1; i <= runs_; i++){
                std::vector<key_type> res_mlrpq;
                std::vector<key_type> res_rpq;
                std::vector<key_type> res_pq;
                // generate new segment sizes
                random_segment_sizes_ = {};
                generateStepSegments();
                // generate new random keys
                auto step_numbers = generateStepNumbers();
                // start benchmarking
                std::cout << "---------------------------------" << std::endl;
                std::cout << "Step " << i << "\n" << "Generated:\t" << step_numbers.size() << " numbers." << std::endl;
                mlrpq_watch.start();
                res_mlrpq = mlrpqAllInAllOut(step_numbers);
                mlrpq_watch.stop();
                std::cout << "mlrpq Step " << i << " time:\t\t" << mlrpq_watch.getCurrentTime() << "\tns" << std::endl;
                rpq_watch.start();
                res_rpq = rpqAllInAllOut(step_numbers);
                rpq_watch.stop();
                std::cout << "rpq Step " << i << " time:\t\t" << pq_watch.getCurrentTime() << "\tns" << std::endl;
                pq_watch.start();
                res_pq = pqAllInAllOut(step_numbers);
                pq_watch.stop();
                std::cout << "pq Step " << i << " time:\t\t\t" << pq_watch.getCurrentTime() << "\tns" << std::endl;
                // check results of step against std::pq (assumed to be correct)
                if (res_mlrpq == res_pq){
                    std::cout << "Arrays equal and sorted correctly" << std::endl;
                }
                else {
                    std::cout << "Checking results failed" << std::endl;
                }
                std::cout << "End of step " << i << "\n---------------------------------" << std::endl;
            }
        }

        auto generateStepSegments(){
            NumberGenerator<key_type> gen(1, max_segment_size_, number_of_segments_);
            random_segment_sizes_ = gen.getGeneratedNumbers();
            std::for_each(random_segment_sizes_.begin(), random_segment_sizes_.end(), [&] (int n) {
                total_count_ += n;
            });

            return random_segment_sizes_;
        }

        auto generateStepNumbers(){
            step_numbers_ = {};
            for (int i = 0; i < number_of_segments_; i++){
                NumberGenerator<key_type> gen(current_segment_start_, current_segment_start_+max_key_, random_segment_sizes_[i]);
                current_segment_start_ = gen.getMaxGenerated();
                auto generated_numbers = gen.getGeneratedNumbers();
                step_numbers_.insert(step_numbers_.end(), generated_numbers.begin(), generated_numbers.end());
            }
            return step_numbers_;
        }

        std::vector<key_type> mlrpqAllInAllOut(std::vector<key_type> step_numbers_){
            std::vector<key_type> tmp_res;
            for (int i = 0; i < step_numbers_.size(); i++){
                mlrpq.push(step_numbers_[i], val);
            }
            while(!mlrpq.empty()){
                tmp_res.push_back(mlrpq.top().first);
                mlrpq.pop();
            }
            return tmp_res;
        }

        std::vector<key_type> pqAllInAllOut(std::vector<key_type> step_numbers_){
            std::vector<key_type> tmp_res;
            for (int i = 0; i < step_numbers_.size(); i++){
                pq.push({step_numbers_[i], val});
            }
            while(!pq.empty()){
                tmp_res.push_back(pq.top().first);
                pq.pop();
            }
            return tmp_res;
        }

        std::vector<key_type> rpqAllInAllOut(std::vector<key_type> step_numbers_){
            std::vector<key_type> tmp_res;
            for (int i = 0; i < step_numbers_.size(); i++){
                rpq.push(step_numbers_[i], val);
            }
            while(!rpq.empty()){
                tmp_res.push_back(rpq.top_key());
                rpq.pop();
            }
            return tmp_res;
        }

    public:

        auto getTotalCount(){
            return total_count_;
        }

    };

    template <typename KeyType, typename ValueType>
    class allInAllOutInterrupted{
        using key_type = KeyType;
        using value_type = ValueType;

        using mlrpq_type = multilayer_radix_pq::multilayer_radix_pq<key_type, value_type, 5>;
        using rpq_type = radix_heap::pair_radix_heap<key_type, value_type>;
        using pq_type = std::priority_queue<std::pair<key_type, value_type>, std::vector<std::pair<key_type, value_type>>, mlrpq_benchmark_comparator::CompareKey<key_type, value_type>>;

    private:
        // internals
        key_type runs_;
        key_type current_pushed_max_;
        key_type current_popped_max_;
        key_type offset_;

        value_type val;

        // result vectors
        std::vector<key_type> res_mlrpq_;
        std::vector<key_type> res_rpq_;
        std::vector<key_type> res_pq_;


    public:
        // constructor
        allInAllOutInterrupted(key_type runs,
                               key_type key_min,
                               key_type key_max,
                               key_type number_of_segments,
                               key_type maximum_segment_size) : runs_(runs),
                                                                current_pushed_max_(key_min),
                                                                current_popped_max_(size_t(0)),
                                                                offset_(size_t(0))
        {
            // initialize result vectors
            for (int i = 1; i <= runs_; i++){
                // construct datastructures to pass them
                mlrpq_type mlrpq;
                rpq_type rpq;
                pq_type pq;
                // initialize result arrays
                res_mlrpq_ = {};
                res_rpq_ = {};
                res_pq_ = {};
                std::cout << "----------------------------\nStarting step " << i << "\n------------------" << std::endl;
                // call wrapper function for a step
                wrapperAllInAllOutInterrupted(key_max, key_min, number_of_segments, maximum_segment_size, mlrpq, rpq, pq);
            }
        }


    private:

        void wrapperAllInAllOutInterrupted(key_type key_max, key_type key_min,
                                           key_type number_of_segments, key_type maximum_segment_size,
                                           mlrpq_type& mlrpq, rpq_type& rpq, pq_type& pq){
            // instantiate stopwatches
            mlrpq_benchmark_time::Stopwatch mlrpq_watch;
            mlrpq_benchmark_time::Stopwatch rpq_watch;
            mlrpq_benchmark_time::Stopwatch pq_watch;

            // set current_pushed_max_ to 0
            current_pushed_max_ = key_min;
            offset_ = size_t(0);

            // generate random segment sizes and divisors for number to be popped
            benchmark::NumberGenerator<key_type> gen_step_segment_sizes(1, maximum_segment_size, number_of_segments);
            auto step_segment_sizes = gen_step_segment_sizes.getGeneratedNumbers();
            NumberGenerator<key_type> gen_divisors(1, 10, step_segment_sizes.size());
            auto step_divisors = gen_divisors.getGeneratedNumbers();

            // keep number of pushed and popped items in memory to completely empty the queue afterwards
            key_type pushed_overall = size_t(0);
            key_type popped_overall = size_t(0);
            std::for_each(step_segment_sizes.begin(), step_segment_sizes.end(), [&] (int n){
                pushed_overall += n;
            });

            // iterate through segments and push random numbers, pop according number of elements
            for (int i = 0; i < number_of_segments; i++){
                NumberGenerator<key_type> gen(current_pushed_max_, offset_+key_max, step_segment_sizes[i]);
                auto segment_numbers = gen.getGeneratedNumbers();
                auto count_numbers_segment_popped = (step_segment_sizes[i] + step_divisors[i] - 1) / step_divisors[i]; // ceil segment_size/divisor
                current_pushed_max_ = gen.getMaxGenerated();
                assert(current_pushed_max_ < std::numeric_limits<key_type>::max());

                // pushing segment keys into mlrpq
                mlrpq_watch.start();
                for (int i = 0; i < segment_numbers.size(); i++) {
                    mlrpq.push(segment_numbers[i], val);
                }
                for (int i = 0; i < count_numbers_segment_popped; i++) {
                    res_mlrpq_.push_back(mlrpq.top().first);
                    mlrpq.pop();
                }
                mlrpq_watch.stop();
                // pushing segment keys into pq
                pq_watch.start();
                for (int i = 0; i < segment_numbers.size(); i++) {
                    pq.push({segment_numbers[i], val});
                }
                for (int i = 0; i < count_numbers_segment_popped; i++) {
                    res_pq_.push_back(pq.top().first);
                    pq.pop();
                }
                pq_watch.stop();
                // pushing segment keys into rpq
                rpq_watch.start();
                for (int i = 0; i < segment_numbers.size(); i++) {
                    rpq.push(segment_numbers[i], val);
                }
                for (int i = 0; i < count_numbers_segment_popped; i++) {
                    res_rpq_.push_back(rpq.top_key());
                    rpq.pop();
                }
                rpq_watch.stop();

                // get the current maximum that was poppped again to ensure monotonous input in next segment
                current_popped_max_ = *std::max_element(res_pq_.begin(), res_pq_.end());
                //std::cout << pq.top().first << std::endl;
                //std::cout << mlrpq.top().first << std::endl;
                offset_ = current_pushed_max_;
                popped_overall += count_numbers_segment_popped;

            }
            // pop remaining numbers from queues
            mlrpq_watch.start();
            for (int i = 0; i < (pushed_overall-popped_overall); i++) {
                res_mlrpq_.push_back(mlrpq.top().first);
                mlrpq.pop();
            }
            mlrpq_watch.stop();
            rpq_watch.start();
            for (int i = 0; i < (pushed_overall-popped_overall); i++) {
                res_rpq_.push_back(rpq.top_key());
                rpq.pop();
            }
            rpq_watch.stop();
            pq_watch.start();
            for (int i = 0; i < (pushed_overall-popped_overall); i++) {
                res_pq_.push_back(pq.top().first);
                pq.pop();
            }
            pq_watch.stop();
            std::cout << "numbers in this step: " << pushed_overall << std::endl;
            std::cout << "mlrpq step time: \t" << mlrpq_watch.getTotalTime() << std::endl;
            std::cout << "rpq step time: \t\t" << rpq_watch.getTotalTime() << std::endl;
            std::cout << "pq step time: \t\t" << pq_watch.getTotalTime() << std::endl;
            std::cout << "mlrpq empty: " << mlrpq.empty() << std::endl;
            std::cout << "rpq empty: " << rpq.empty() << std::endl;
            std::cout << "pq empty: " << pq.empty() << std::endl;
            std::string eq = (res_mlrpq_ == res_pq_) ? "true" : "false";
            std::cout << "Results are equal: " << eq << std::endl;

            /// DEBUGGING
            if (!(res_mlrpq_ == res_pq_)){
                std::cout << ((res_pq_.size() == res_mlrpq_.size()) ? "equally sized" : "not equally sized") << std::endl;
                for (int i = 0; i < res_pq_.size(); i++){
                    std::cout << "pq:\t" << res_pq_[i] << "\tmlrpq:\t" << res_mlrpq_[i] << std::endl;
                }
            }
            ///



        }

    };

}; // end namespace benchmark








#endif //MULTILAYER_RADIX_PRIORITY_QUEUE_MLRPQ_BENCHMARK_H
