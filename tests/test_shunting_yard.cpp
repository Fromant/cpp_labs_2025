#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../src/Tokenizer.hpp"
#include "../src/ShuntingYard.hpp"
#include "MockPluginRegistry.hpp"

using ::testing::Return;
using ::testing::ReturnRef;

// Dummy evaluator (not called in shunting yard)
static double dummy_eval(const double*, size_t) { return 0.0; }

TEST(ShuntingYardTest, SimpleAddition) {
    MockPluginRegistry mock;
    MockPluginRegistry::TokenInfo addInfo{2, 60, Associativity::Left, true, dummy_eval};

    EXPECT_CALL(mock, hasToken("+")).WillRepeatedly(Return(true));
    EXPECT_CALL(mock, getTokenInfo("+")).WillRepeatedly(ReturnRef(addInfo));

    auto tokens = tokenize("1 + 2");
    auto rpn = shuntingYard(tokens, mock);

    ASSERT_EQ(rpn.size(), 3);
    EXPECT_EQ(rpn[0].lexeme, "1");
    EXPECT_EQ(rpn[1].lexeme, "2");
    EXPECT_EQ(rpn[2].lexeme, "+");
}

TEST(ShuntingYardTest, OperatorPrecedence) {
    MockPluginRegistry mock;
    MockPluginRegistry::TokenInfo addInfo{2, 60, Associativity::Left, true, dummy_eval};
    MockPluginRegistry::TokenInfo mulInfo{2, 70, Associativity::Left, true, dummy_eval};

    EXPECT_CALL(mock, hasToken("+")).WillRepeatedly(Return(true));
    EXPECT_CALL(mock, hasToken("*")).WillRepeatedly(Return(true));
    EXPECT_CALL(mock, getTokenInfo("+")).WillRepeatedly(ReturnRef(addInfo));
    EXPECT_CALL(mock, getTokenInfo("*")).WillRepeatedly(ReturnRef(mulInfo));

    auto tokens = tokenize("2 + 3 * 4");
    auto rpn = shuntingYard(tokens, mock);

    // RPN: 2 3 4 * +
    ASSERT_EQ(rpn.size(), 5);
    EXPECT_EQ(rpn[0].lexeme, "2");
    EXPECT_EQ(rpn[1].lexeme, "3");
    EXPECT_EQ(rpn[2].lexeme, "4");
    EXPECT_EQ(rpn[3].lexeme, "*");
    EXPECT_EQ(rpn[4].lexeme, "+");
}

TEST(ShuntingYardTest, RightAssociativePower) {
    MockPluginRegistry mock;
    MockPluginRegistry::TokenInfo powInfo{2, 80, Associativity::Right, true, dummy_eval};

    EXPECT_CALL(mock, hasToken("^")).WillRepeatedly(Return(true));
    EXPECT_CALL(mock, getTokenInfo("^")).WillRepeatedly(ReturnRef(powInfo));

    auto tokens = tokenize("2 ^ 3 ^ 2");
    auto rpn = shuntingYard(tokens, mock);

    // Right-assoc: 2^(3^2) → RPN: 2 3 2 ^ ^
    ASSERT_EQ(rpn.size(), 5);
    EXPECT_EQ(rpn[0].lexeme, "2");
    EXPECT_EQ(rpn[1].lexeme, "3");
    EXPECT_EQ(rpn[2].lexeme, "2");
    EXPECT_EQ(rpn[3].lexeme, "^"); // inner
    EXPECT_EQ(rpn[4].lexeme, "^"); // outer
}

TEST(ShuntingYardTest, FunctionCall) {
    MockPluginRegistry mock;
    MockPluginRegistry::TokenInfo sinInfo{1, 90, Associativity::Left, false, dummy_eval};

    EXPECT_CALL(mock, hasToken("sin")).WillRepeatedly(Return(true));
    EXPECT_CALL(mock, getTokenInfo("sin")).WillRepeatedly(ReturnRef(sinInfo));

    auto tokens = tokenize("sin(1.57)");
    auto rpn = shuntingYard(tokens, mock);

    ASSERT_EQ(rpn.size(), 2);
    EXPECT_EQ(rpn[0].lexeme, "1.57");
    EXPECT_EQ(rpn[1].lexeme, "sin");
}

TEST(ShuntingYardTest, NestedFunction) {
    MockPluginRegistry mock;
    MockPluginRegistry::TokenInfo sinInfo{1, 90, Associativity::Left, false, dummy_eval};
    MockPluginRegistry::TokenInfo cosInfo{1, 90, Associativity::Left, false, dummy_eval};

    EXPECT_CALL(mock, hasToken("sin")).WillRepeatedly(Return(true));
    EXPECT_CALL(mock, hasToken("cos")).WillRepeatedly(Return(true));
    EXPECT_CALL(mock, getTokenInfo("sin")).WillRepeatedly(ReturnRef(sinInfo));
    EXPECT_CALL(mock, getTokenInfo("cos")).WillRepeatedly(ReturnRef(cosInfo));

    auto tokens = tokenize("sin(cos(0.5))");
    auto rpn = shuntingYard(tokens, mock);

    // RPN: 0.5 cos sin
    ASSERT_EQ(rpn.size(), 3);
    EXPECT_EQ(rpn[0].lexeme, "0.5");
    EXPECT_EQ(rpn[1].lexeme, "cos");
    EXPECT_EQ(rpn[2].lexeme, "sin");
}