#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cmath>
#include "../src/RpnEvaluator.hpp"
#include "../src/Token.hpp"
#include "MockPluginRegistry.hpp"

using ::testing::ReturnRef;

static PluginResult add_eval(const double* args, size_t) {
    return {args[0] + args[1], nullptr};
}

static PluginResult sin_eval(const double* args, size_t) {
    return {std::sin(args[0]), nullptr};
}

static PluginResult unary_minus_eval(const double* args, size_t) {
    return {-args[0], nullptr};
}

static PluginResult pow_eval(const double* args, size_t) {
    return {std::pow(args[0], args[1]), nullptr};
}

TEST(RpnEvaluatorTest, BinaryOperator) {
    MockPluginRegistry mock;
    auto addInfo = IPluginRegistry::TokenInfo{2, 60, Associativity::Left, true, add_eval};
    EXPECT_CALL(mock, getBinaryOperator("+")).WillRepeatedly(ReturnRef(addInfo));

    std::vector<Token> rpn = {
        {Token::NUMBER, "2", 2.0},
        {Token::NUMBER, "3", 3.0},
        {Token::BINARY_OPERATOR, "+"}
    };

    double result = evaluateRpn(rpn, mock);
    EXPECT_DOUBLE_EQ(result, 5.0);
}

TEST(RpnEvaluatorTest, Function) {
    MockPluginRegistry mock;
    auto sinInfo = IPluginRegistry::TokenInfo{1, 90, Associativity::Left, false, sin_eval};
    EXPECT_CALL(mock, getFunction("sin")).WillRepeatedly(ReturnRef(sinInfo));

    std::vector<Token> rpn = {
        {Token::NUMBER, "1.5708", 1.5708},
        {Token::FUNCTION, "sin"}
    };

    double result = evaluateRpn(rpn, mock);
    EXPECT_NEAR(result, 1.0, 1e-4);
}

TEST(RpnEvaluatorTest, UnaryOperator) {
    MockPluginRegistry mock;
    auto minusInfo = IPluginRegistry::TokenInfo{1, 90, Associativity::Right, true, unary_minus_eval};
    EXPECT_CALL(mock, getUnaryOperator("-")).WillRepeatedly(ReturnRef(minusInfo));

    std::vector<Token> rpn = {
        {Token::NUMBER, "5", 5.0},
        {Token::UNARY_OPERATOR, "-"}
    };

    double result = evaluateRpn(rpn, mock);
    EXPECT_DOUBLE_EQ(result, -5.0);
}

TEST(RpnEvaluatorTest, ComplexExpression_MinusPower) {
    MockPluginRegistry mock;
    auto powInfo = IPluginRegistry::TokenInfo{2, 100, Associativity::Right, true, pow_eval};
    auto minusInfo = IPluginRegistry::TokenInfo{1, 90, Associativity::Right, true, unary_minus_eval};

    EXPECT_CALL(mock, getBinaryOperator("^")).WillRepeatedly(ReturnRef(powInfo));
    EXPECT_CALL(mock, getUnaryOperator("-")).WillRepeatedly(ReturnRef(minusInfo));

    // RPN for -(2^4): 2 4 ^ -
    std::vector<Token> rpn = {
        {Token::NUMBER, "2", 2.0},
        {Token::NUMBER, "4", 4.0},
        {Token::BINARY_OPERATOR, "^"},
        {Token::UNARY_OPERATOR, "-"}
    };

    double result = evaluateRpn(rpn, mock);
    EXPECT_DOUBLE_EQ(result, -16.0);
}