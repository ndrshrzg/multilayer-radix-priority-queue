//
// Created by anherzog on 09.11.2017.
//

#include "gtest/gtest.h"
#include "../include/multilayer_radix_pq.h"

//TODO create a test fixture to avoid code duplication

TEST(mlrpqTest, EmptyWithoutPush){
    multilayer_radix_pq::multilayer_radix_pq<uint64_t, int, 3> mlrpq(4096);

    bool res = mlrpq.empty();

    ASSERT_EQ(true, res);
}

TEST(mlrpqTest, NotEmptyAfterPush){
    multilayer_radix_pq::multilayer_radix_pq<uint64_t, int, 3> mlrpq(4096);
    mlrpq.push(uint64_t(pow(2,0)),1);
    bool res = mlrpq.empty();

    ASSERT_EQ(false, res);
}

TEST(mlrpqTest, TopReturnsMinimumElement){
    multilayer_radix_pq::multilayer_radix_pq<uint64_t, int, 3> mlrpq(4096);
    mlrpq.push(uint64_t(pow(2,0)),1);
    mlrpq.push(uint64_t(pow(2,1)),1);

    std::pair<int, int> res = mlrpq.top();
    std::pair<int, int> exp(0,1);
    ASSERT_EQ(exp, res);
}

TEST(mlrpqTest, CalculateBucket){
    multilayer_radix_pq::multilayer_radix_pq<uint64_t, int, 3> mlrpq(4096);
    mlrpq.push(uint64_t(pow(2,7)+pow(2,8)),1);

    bool res = mlrpq.bucketEmpty(2);

    ASSERT_EQ(false, res);

}