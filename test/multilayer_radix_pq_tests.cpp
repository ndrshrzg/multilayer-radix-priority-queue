//
// Created by anherzog on 09.11.2017.
//

#include "gtest/gtest.h"
#include "../include/multilayer_radix_pq.h"

TEST(mlrqTest, EmptyWithoutPush){
    multilayer_radix_pq::multilayer_radix_pq<uint64_t, int, 3> mlrq(4096);

    bool res = mlrq.empty();

    ASSERT_EQ(true, res);
}

TEST(mlrqTest, NotEmptyAfterPush){
    multilayer_radix_pq::multilayer_radix_pq<uint64_t, int, 3> mlrq(4096);
    mlrq.push(uint64_t(pow(2,0)),1);
    bool res = mlrq.empty();

    ASSERT_EQ(false, res);
}