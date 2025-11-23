// test_shunting_yard.cpp
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../src/Tokenizer.hpp"
#include "../src/ShuntingYard.hpp"
#include "../src/Token.hpp"
#include "MockPluginRegistry.hpp"

using ::testing::Return;
using ::testing::ReturnRef;

static PluginResult dummy_eval(const double*, size_t) {
    return {0.0, nullptr};
}

TEST(ShuntingYardTest, SimpleAddition) {
    MockPluginRegistry mock;
    auto addInfo = IPluginRegistry::TokenInfo{
        2, 60, Associativity::Left, true, dummy_eval
    };

    EXPECT_CALL(mock, hasBinaryOperator("+")).WillRepeatedly(Return(true));
    EXPECT_CALL(mock, getBinaryOperator("+")).WillRepeatedly(ReturnRef(addInfo));

    auto tokens = tokenize("1 + 2");
    auto rpn = shuntingYard(tokens, mock);

    ASSERT_EQ(rpn.size(), 3);
    EXPECT_EQ(rpn[0].type, Token::NUMBER);
    EXPECT_EQ(rpn[1].type, Token::NUMBER);
    EXPECT_EQ(rpn[2].type, Token::BINARY_OPERATOR);
    EXPECT_EQ(rpn[2].lexeme, "+");
}

TEST(ShuntingYardTest, OperatorPrecedence) {
    MockPluginRegistry mock;
    auto addInfo = IPluginRegistry::TokenInfo{2, 60, Associativity::Left, true, dummy_eval};
    auto mulInfo = IPluginRegistry::TokenInfo{2, 70, Associativity::Left, true, dummy_eval};

    EXPECT_CALL(mock, hasBinaryOperator("+")).WillRepeatedly(Return(true));
    EXPECT_CALL(mock, hasBinaryOperator("*")).WillRepeatedly(Return(true));
    EXPECT_CALL(mock, getBinaryOperator("+")).WillRepeatedly(ReturnRef(addInfo));
    EXPECT_CALL(mock, getBinaryOperator("*")).WillRepeatedly(ReturnRef(mulInfo));

    auto tokens = tokenize("2 + 3 * 4");
    auto rpn = shuntingYard(tokens, mock);

    // RPN: 2 3 4 * +
    ASSERT_EQ(rpn.size(), 5);
    EXPECT_EQ(rpn[0].lexeme, "2");
    EXPECT_EQ(rpn[1].lexeme, "3");
    EXPECT_EQ(rpn[2].lexeme, "4");
    EXPECT_EQ(rpn[3].type, Token::BINARY_OPERATOR);
    EXPECT_EQ(rpn[3].lexeme, "*");
    EXPECT_EQ(rpn[4].type, Token::BINARY_OPERATOR);
    EXPECT_EQ(rpn[4].lexeme, "+");
}

TEST(ShuntingYardTest, RightAssociativePower) {
    MockPluginRegistry mock;
    auto powInfo = IPluginRegistry::TokenInfo{2, 100, Associativity::Right, true, dummy_eval};

    EXPECT_CALL(mock, hasBinaryOperator("^")).WillRepeatedly(Return(true));
    EXPECT_CALL(mock, getBinaryOperator("^")).WillRepeatedly(ReturnRef(powInfo));

    auto tokens = tokenize("2 ^ 3 ^ 2");
    auto rpn = shuntingYard(tokens, mock);

    // 2 3 2 ^ ^  → right-assoc: 2^(3^2)
    ASSERT_EQ(rpn.size(), 5);
    EXPECT_EQ(rpn[3].lexeme, "^");
    EXPECT_EQ(rpn[4].lexeme, "^");
}

TEST(ShuntingYardTest, FunctionCallWithParens) {
    MockPluginRegistry mock;
    auto sinInfo = IPluginRegistry::TokenInfo{1, 90, Associativity::Left, false, dummy_eval};

    EXPECT_CALL(mock, hasFunction("sin")).WillRepeatedly(Return(true));
    EXPECT_CALL(mock, getFunction("sin")).WillRepeatedly(ReturnRef(sinInfo));

    auto tokens = tokenize("sin(1.57)");
    auto rpn = shuntingYard(tokens, mock);

    ASSERT_EQ(rpn.size(), 2);
    EXPECT_EQ(rpn[0].type, Token::NUMBER);
    EXPECT_EQ(rpn[1].type, Token::FUNCTION);
    EXPECT_EQ(rpn[1].lexeme, "sin");
}

TEST(ShuntingYardTest, FunctionWithoutParens_Throws) {
    MockPluginRegistry mock;

    EXPECT_CALL(mock, hasFunction("sin")).WillRepeatedly(Return(true));
    EXPECT_CALL(mock, hasUnaryOperator("sin")).WillRepeatedly(Return(false));

    auto tokens = tokenize("sin 1.57"); // no parentheses

    EXPECT_THROW({
        shuntingYard(tokens, mock);
    }, std::runtime_error);
}

TEST(ShuntingYardTest, UnaryMinus) {
    MockPluginRegistry mock;
    auto unaryMinus = IPluginRegistry::TokenInfo{1, 90, Associativity::Right, true, dummy_eval};
    auto addInfo = IPluginRegistry::TokenInfo{2, 60, Associativity::Left, true, dummy_eval};

    EXPECT_CALL(mock, hasUnaryOperator("-")).WillRepeatedly(Return(true));
    EXPECT_CALL(mock, hasBinaryOperator("-")).WillRepeatedly(Return(false)); // not used here
    EXPECT_CALL(mock, hasBinaryOperator("+")).WillRepeatedly(Return(true));
    EXPECT_CALL(mock, getUnaryOperator("-")).WillRepeatedly(ReturnRef(unaryMinus));
    EXPECT_CALL(mock, getBinaryOperator("+")).WillRepeatedly(ReturnRef(addInfo));

    auto tokens = tokenize("-1 + 2");
    auto rpn = shuntingYard(tokens, mock);

    ASSERT_EQ(rpn.size(), 4);
    EXPECT_EQ(rpn[0].type, Token::NUMBER);
    EXPECT_EQ(rpn[0].value, 1.0);
    EXPECT_EQ(rpn[1].type, Token::UNARY_OPERATOR);
    EXPECT_EQ(rpn[1].lexeme, "-");
    EXPECT_EQ(rpn[2].type, Token::NUMBER);
    EXPECT_EQ(rpn[3].type, Token::BINARY_OPERATOR);
}

TEST(ShuntingYardTest, PowerVsUnaryMinus_PowerHasHigherPrecedence) {
    MockPluginRegistry mock;
    // ^ has higher precedence than unary -
    auto powInfo = IPluginRegistry::TokenInfo{2, 100, Associativity::Right, true, dummy_eval};
    auto unaryMinus = IPluginRegistry::TokenInfo{1, 90, Associativity::Right, true, dummy_eval};

    EXPECT_CALL(mock, hasBinaryOperator("^")).WillRepeatedly(Return(true));
    EXPECT_CALL(mock, hasUnaryOperator("-")).WillRepeatedly(Return(true));
    EXPECT_CALL(mock, getBinaryOperator("^")).WillRepeatedly(ReturnRef(powInfo));
    EXPECT_CALL(mock, getUnaryOperator("-")).WillRepeatedly(ReturnRef(unaryMinus));

    auto tokens = tokenize("-2 ^ 4");
    auto rpn = shuntingYard(tokens, mock);

    // Expected RPN: 2 4 ^ -
    // -(2^4)
    ASSERT_EQ(rpn.size(), 4);
    EXPECT_EQ(rpn[0].lexeme, "2");
    EXPECT_EQ(rpn[1].lexeme, "4");
    EXPECT_EQ(rpn[2].type, Token::BINARY_OPERATOR);
    EXPECT_EQ(rpn[2].lexeme, "^");
    EXPECT_EQ(rpn[3].type, Token::UNARY_OPERATOR);
    EXPECT_EQ(rpn[3].lexeme, "-");
}