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
    class NumberGeneratorStepper{
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
        key_type total_count_;
        value_type val;

        std::vector<key_type> random_segment_sizes_;

        key_type overall_popped; //use (runs * segment_count) - overall_popped in last run to ensure all numbers were popped

    // data structures being benchmarked
        multilayer_radix_pq::multilayer_radix_pq<key_type, value_type, 5> mlrpq;
        radix_heap::pair_radix_heap<key_type, value_type> rpq;
        std::priority_queue<std::pair<key_type, value_type>, std::vector<std::pair<key_type, value_type>>, mlrpq_benchmark_comparator::CompareKey<key_type, value_type>> pq;


    public:
        NumberGeneratorStepper(key_type runs, key_type max_key, key_type max_segment_size, key_type number_of_segments, bool interrupted) :
                runs_(runs),
                max_key_(max_key),
                max_segment_size_(max_segment_size),
                number_of_segments_(number_of_segments),
                current_segment_start_(size_t(0)),
                interrupted_(interrupted)
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
                generate_step_segments();
                // generate new random keys
                auto step_numbers = generate_step_numbers();
                // start benchmarking
                std::cout << "---------------------------------" << std::endl;
                std::cout << "Step " << i << "\n" << "Generated:\t" << step_numbers.size() << " numbers." << std::endl;
                mlrpq_watch.start();
                res_mlrpq = mlrpqAllInAllOut(step_numbers_);
                mlrpq_watch.stop();
                std::cout << "mlrpq Step " << i << " time: " << mlrpq_watch.getCurrentTime() << "ms" << std::endl;
                rpq_watch.start();
                res_rpq = rpqAllInAllOut(step_numbers_);
                rpq_watch.stop();
                std::cout << "rpq Step " << i << " time: " << pq_watch.getCurrentTime() << "ms" << std::endl;
                pq_watch.start();
                res_pq = pqAllInAllOut(step_numbers_);
                pq_watch.stop();
                std::cout << "pq Step " << i << " time: " << pq_watch.getCurrentTime() << "ms" << std::endl;
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

        auto generate_step_segments(){
            NumberGenerator<key_type> gen(1, max_segment_size_, number_of_segments_);
            random_segment_sizes_ = gen.getGeneratedNumbers();
            std::for_each(random_segment_sizes_.begin(), random_segment_sizes_.end(), [&] (int n) {
                total_count_ += n;
            });
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

}; // end namespace benchmark








#endif //MULTILAYER_RADIX_PRIORITY_QUEUE_MLRPQ_BENCHMARK_H
