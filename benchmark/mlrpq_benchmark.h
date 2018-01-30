//
// Created by ndrshrzg on 1/30/18.
//

#include <random>
#include <chrono>
#include "../include/multilayer_radix_pq.h"

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
        std::chrono::steady_clock::duration overall_;
        std::chrono::steady_clock::duration current_;
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
            overall_ += stop_ - start_;
        };

        long getCurrentTime(){
            return std::chrono::duration_cast<std::chrono::milliseconds>(current_).count();
        }

        long getTotalTime(){
            return std::chrono::duration_cast<std::chrono::milliseconds>(overall_).count();
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

        auto pickOne(){
            return result_[0];
        }


    };

    template <typename KeyType, typename ValueType>
    class NumberGeneratorWrapper{
        using key_type = KeyType;
        using value_type = ValueType;

    private:
        // passed by constructor
        key_type runs_;
        key_type max_key_;
        key_type max_segment_size_;
        key_type number_of_segments_;
        key_type current_segment_start_;
        bool interrupted_; // if true, interrupt push / pop with intermittend pops and pushes

        // internals
        std::vector<key_type> step_numbers_;
        std::vector<key_type> result_;
        key_type total_count_;

        std::vector<key_type> random_segment_sizes_;

        key_type overall_popped; //use (runs * segment_count) - overall_popped in last run to ensure all numbers were popped

    // data structures being benchmarked
        multilayer_radix_pq::multilayer_radix_pq<key_type, value_type, 5> mlrpq;
        std::priority_queue<std::pair<key_type, value_type>, std::vector<std::pair<key_type, value_type>>, mlrpq_benchmark_comparator::CompareKey<key_type, value_type>> pq;


    public:
        NumberGeneratorWrapper(key_type runs, key_type max_key, key_type max_segment_size, key_type number_of_segments, bool interrupted) :
                runs_(runs),
                max_key_(max_key),
                max_segment_size_(max_segment_size),
                number_of_segments_(number_of_segments),
                current_segment_start_(size_t(0)),
                interrupted_(interrupted),
                result_({})
        {
            NumberGenerator<key_type> gen(1, max_segment_size_, number_of_segments_);
            random_segment_sizes_ = gen.getGeneratedNumbers();
            std::for_each(random_segment_sizes_.begin(), random_segment_sizes_.end(), [&] (int n) {
               total_count_ += n;
            });


            execute();

        };

    private:
        void execute(){
            mlrpq_benchmark_time::Stopwatch watch;
            for (int i = 1; i <= runs_; i++){
                auto step_numbers = generate_step_numbers();
                watch.start();
                mlrpqPushAllPullAll(step_numbers_);
                watch.stop();
                std::cout << "Step time: " << watch.getCurrentTime() << "ms" << std::endl;
            }
            std::cout << "Overall time: " << watch.getTotalTime() << "ms" << std::endl;
            if (std::is_sorted(result_.begin(), result_.end())){
                std::cout << "Array is sorted correctly" << std::endl;
            }
            else {
                std::cout << "Checking results failed" << std::endl;
            }
        }

        auto generate_step_numbers(){
            step_numbers_ = {};
            for (int i = 0; i < number_of_segments_; i++){
                NumberGenerator<key_type> gen(current_segment_start_, current_segment_start_+max_key_, random_segment_sizes_[i]);
                current_segment_start_ = gen.getMaxGenerated();
                auto generated_numbers = gen.getGeneratedNumbers();
                step_numbers_.insert(step_numbers_.end(), generated_numbers.begin(), generated_numbers.end());
            }
            return step_numbers_;
        }

        void mlrpqPushAllPullAll(std::vector<key_type> step_numbers_){
            auto tmp_min = std::numeric_limits<key_type>::max();
            for (int i = 0; i < step_numbers_.size(); i++){
                if (step_numbers_[i] < tmp_min) { tmp_min = step_numbers_[i]; };
                mlrpq.push(step_numbers_[i], 0);
            }
            while(!mlrpq.empty()){
                result_.push_back(mlrpq.top().first);
                mlrpq.pop();
            }
        }



    public:

        auto getTotalCount(){
            return total_count_;
        }





    };

}; // end namespace benchmark








#endif //MULTILAYER_RADIX_PRIORITY_QUEUE_MLRPQ_BENCHMARK_H
