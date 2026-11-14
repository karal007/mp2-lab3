#include <gtest/gtest.h>
#include "translator.h"

TEST(VectorTest, PushBackAndAccess) {
    Vector<int> v;
    EXPECT_TRUE(v.empty());
    EXPECT_EQ(v.size(), 0);

    for (int i = 0; i < 10; ++i) {
        v.push_back(i * 10);
    }

    EXPECT_EQ(v.size(), 10);
    EXPECT_EQ(v[0], 0);
    EXPECT_EQ(v.at(5), 50);
    EXPECT_EQ(v.back(), 90);
}

TEST(VectorTest, CopyAndMoveSemantics) {
    Vector<int> v1;
    v1.push_back(1);
    v1.push_back(2);

    Vector<int> v2 = v1;
    EXPECT_EQ(v2.size(), 2);
    EXPECT_EQ(v2[1], 2);

    Vector<int> v3 = std::move(v1);
    EXPECT_EQ(v3.size(), 2);
    EXPECT_TRUE(v1.empty());
}

TEST(VectorTest, ThrowsOnInvalidOperations) {
    Vector<int> v;
    EXPECT_THROW(v.pop_back(), std::out_of_range);
    EXPECT_THROW(v.back(), std::out_of_range);
    EXPECT_THROW(v.at(0), std::out_of_range);
}