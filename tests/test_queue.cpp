#include <gtest/gtest.h>
#include "translator.h"

TEST(QueueTest, FifoOrderWithTwoStacks) {
    Queue<int> q;
    q.push(1);
    q.push(2);
    q.push(3);

    EXPECT_EQ(q.front(), 1);
    q.pop();

    q.push(4);
    EXPECT_EQ(q.front(), 2);
    q.pop();
    EXPECT_EQ(q.front(), 3);
    q.pop();
    EXPECT_EQ(q.front(), 4);
    q.pop();
    EXPECT_TRUE(q.empty());
}

TEST(QueueTest, CircularBufferCapacityLimit) {
    Queue<int> q(2);
    q.push(100);
    q.push(200);
    EXPECT_TRUE(q.full());
    EXPECT_THROW(q.push(300), std::overflow_error);

    q.pop();
    EXPECT_FALSE(q.full());
    q.push(300);
    EXPECT_EQ(q.front(), 200);
}