#include <gtest/gtest.h>
#include "translator.h"

TEST(TranslatorTest, BasicArithmeticAndPrecedence) {
    Translator t1("2 + 3 * 4");
    EXPECT_EQ(t1.getPostfixString(), "2 3 4 * +");
    EXPECT_DOUBLE_EQ(t1.calculate(), 14.0);

    Translator t2("(2 + 3) * 4");
    EXPECT_EQ(t2.getPostfixString(), "2 3 + 4 *");
    EXPECT_DOUBLE_EQ(t2.calculate(), 20.0);
}

TEST(TranslatorTest, UnaryMinusAndDecimals) {
    Translator t("-5.5 + 2 * (-3 + 4.5)");
    EXPECT_DOUBLE_EQ(t.calculate(), -2.5);
}

TEST(TranslatorTest, PowerOperatorRightAssociativity) {
    Translator t("2 ^ 3 ^ 2");
    EXPECT_EQ(t.getPostfixString(), "2 3 2 ^ ^");
    EXPECT_DOUBLE_EQ(t.calculate(), 512.0);
}

TEST(TranslatorTest, SyntaxAndRuntimeErrors) {
    EXPECT_THROW(Translator("(2 + 3"), std::invalid_argument);
    EXPECT_THROW(Translator("2 + 3)"), std::invalid_argument);
    EXPECT_THROW(Translator("2 + * 3"), std::invalid_argument);
    EXPECT_THROW(Translator(""), std::invalid_argument);

    Translator div_zero("10 / (5 - 5)");
    EXPECT_THROW(div_zero.calculate(), std::runtime_error);
}