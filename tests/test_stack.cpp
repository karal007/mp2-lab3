#include <gtest/gtest.h>
#include "translator.h"

TEST(StackTest, LifoOrder) {
    Stack<int> s;
    EXPECT_TRUE(s.empty());

    s.push(10);
    s.push(20);
    s.push(30);

    EXPECT_EQ(s.size(), 3);
    EXPECT_EQ(s.top(), 30);

    s.pop();
    EXPECT_EQ(s.top(), 20);
    s.pop();
    EXPECT_EQ(s.top(), 10);
    s.pop();
    EXPECT_TRUE(s.empty());
}

TEST(StackTest, ThrowsWhenEmpty) {
    Stack<double> s;
    EXPECT_THROW(s.top(), std::out_of_range);
    EXPECT_THROW(s.pop(), std::out_of_range);
}