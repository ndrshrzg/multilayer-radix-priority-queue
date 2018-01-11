//
// Created by Andreas Herzog on 12/07/17.
// andreas.herzog@stud.uni-frankfurt.de
//

#include <queue>
#include "gtest/gtest.h"
#include "../include/multilayer_radix_pq.h"

TEST(mlrpqTest, EmptyWithoutPush){
    multilayer_radix_pq::multilayer_radix_pq<uint64_t, int, 3> mlrpq;

    bool res = mlrpq.empty();

    ASSERT_EQ(true, res);
}

TEST(mlrpqTest, NotEmptyAfterPush){
    multilayer_radix_pq::multilayer_radix_pq<uint64_t, int, 3> mlrpq;
    mlrpq.push(uint64_t(pow(2,0)),1);
    bool res = mlrpq.empty();

    ASSERT_EQ(false, res);
}

TEST(mlrpqTest, NotEmptyAfterPushIntoNBucket){
    multilayer_radix_pq::multilayer_radix_pq<uint64_t, int, 3> mlrpq;
    mlrpq.push(uint64_t(pow(2,24)),1);
    bool res = mlrpq.empty();

    ASSERT_EQ(false, res);
}

TEST(mlrpqTest, TopReturnsMinimumElement){
    multilayer_radix_pq::multilayer_radix_pq<uint64_t, int, 3> mlrpq;
    mlrpq.push(uint64_t(pow(2,0)),1);
    mlrpq.push(uint64_t(pow(2,1)),1);

    uint64_t res = mlrpq.top().first;
    uint64_t exp = 1;
    ASSERT_EQ(exp, res);
}

TEST(mlrpqTest, TopReturnsMinimumElementFromNBucket){
    multilayer_radix_pq::multilayer_radix_pq<uint64_t, int, 5> mlrpq;
    mlrpq.push(size_t(1)<<20, 0);
    mlrpq.push((size_t(1)<<20)+1, 0);

    uint64_t res = mlrpq.top().first;
    uint64_t exp = size_t(1)<<20;
    ASSERT_EQ(exp, res);
}

TEST(mlrpqTest, TopCrashesWhenEmpty){
    multilayer_radix_pq::multilayer_radix_pq<uint64_t, int, 5> mlrpq;
    ASSERT_DEATH(mlrpq.top(), ".");
}

TEST(mlrpqTest, CrashesWhenMonotonicityBroken){
    multilayer_radix_pq::multilayer_radix_pq<uint64_t, int, 5> mlrpq;
    mlrpq.push(128, 0);
    mlrpq.pop();

    ASSERT_DEATH(mlrpq.push(127, 0), ".");

}

TEST(mlrpqTest, PopRemovesMinimumElement){
    multilayer_radix_pq::multilayer_radix_pq<uint64_t, int, 5> mlrpq;
    mlrpq.push(128, 0);
    mlrpq.push(129, 0);
    mlrpq.pop();
    uint64_t res = mlrpq.top().first;
    uint64_t exp = 129;

    ASSERT_EQ(res, exp);

}

TEST(mlrpqTest, PopCrashesWhenEmpty){
    multilayer_radix_pq::multilayer_radix_pq<uint64_t, int, 5> mlrpq;

    ASSERT_DEATH(mlrpq.pop(), ".");


}

TEST(mlrpqTest, QueueReturnsCorrectArray){
    multilayer_radix_pq::multilayer_radix_pq<uint64_t, int, 3, true> mlrpq;
    std::priority_queue<uint64_t, std::vector<uint64_t>, std::greater<uint64_t>> pq;

    std::vector<uint64_t> ar{0,1,2,3,5,64,129,257,986,2049,16895,28675,2406987, 2406994,3698574,7845329,12896586,67895442138, 878954421389634};
    size_t size = ar.size();

    std::vector<uint64_t> res_pq;
    std::vector<uint64_t> res_mlrpq;

    for (int i=0; i < size; i++){
        pq.push(ar[i]);
        mlrpq.push(ar[i], 0);
    }

    for (int j=0; j<size; j++){

        const auto temp_pair = mlrpq.top();

        uint64_t temp_mlrpq = temp_pair.first;
        uint64_t temp_pq = pq.top();

        res_mlrpq.push_back(temp_mlrpq);
        res_pq.push_back(temp_pq);

        mlrpq.pop();
        pq.pop();

    }

    ASSERT_EQ(res_pq, res_mlrpq);

}

TEST(mlrpqTest, QueuesDifferentRadixReturnSameArray){

    using KEY_TYPE = uint32_t;

    multilayer_radix_pq::multilayer_radix_pq<KEY_TYPE, int, 3, true> mlrpq3;
    multilayer_radix_pq::multilayer_radix_pq<KEY_TYPE, int, 6, true> mlrpq6;
    multilayer_radix_pq::multilayer_radix_pq<KEY_TYPE, int, 8, true> mlrpq8;
    multilayer_radix_pq::multilayer_radix_pq<KEY_TYPE, int, 11, true> mlrpq11;

    std::priority_queue<KEY_TYPE, std::vector<KEY_TYPE>, std::greater<KEY_TYPE>> pq;

    bool eq = true;

    std::vector<KEY_TYPE> ar{0,1,2,3,5,64,129,257,986,2049,16895,28675,2406987,3698574,7845329,12896586};
    size_t size = ar.size();

    std::vector<KEY_TYPE> res_mlrpq3;
    std::vector<KEY_TYPE> res_mlrpq6;
    std::vector<KEY_TYPE> res_mlrpq8;
    std::vector<KEY_TYPE> res_mlrpq11;

    for (int i=0; i < size; i++){
        pq.push(ar[i]);
        mlrpq3.push(ar[i], 0);
        mlrpq6.push(ar[i], 0);
        mlrpq8.push(ar[i], 0);
        mlrpq11.push(ar[i], 0);
    }

    for (int j=0; j<size; j++){
        KEY_TYPE temp_mlrpq3 = mlrpq3.top().first;
        KEY_TYPE temp_mlrpq6 = mlrpq6.top().first;
        KEY_TYPE temp_mlrpq8 = mlrpq8.top().first;
        KEY_TYPE temp_mlrpq11 = mlrpq11.top().first;
        KEY_TYPE temp_pq = pq.top();

        if (
            temp_pq != temp_mlrpq3 |
            temp_pq != temp_mlrpq6 |
            temp_pq != temp_mlrpq8 |
            temp_pq != temp_mlrpq11)
        {
            eq = false;
        }


        mlrpq3.pop();
        mlrpq6.pop();
        mlrpq8.pop();
        mlrpq11.pop();
        pq.pop();

    }

    ASSERT_EQ(true, eq);

}