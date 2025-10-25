#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../src/RpnEvaluator.hpp"
#include "../src/Token.hpp"
#include "MockPluginRegistry.hpp"

using ::testing::ReturnRef;

static double add_eval(const double* args, size_t) {
    return args[0] + args[1];
}

static double sin_eval(const double* args, size_t) {
    return std::sin(args[0]);
}

TEST(RpnEvaluatorTest, BinaryOperator) {
    MockPluginRegistry mock;
    MockPluginRegistry::TokenInfo addInfo{2, 60, Associativity::Left, true, add_eval};

    EXPECT_CALL(mock, getTokenInfo("+")).WillRepeatedly(ReturnRef(addInfo));

    std::vector<Token> rpn = {
        {Token::TokenType::NUMBER, "2", 2.0},
        {Token::TokenType::NUMBER, "3", 3.0},
        {Token::TokenType::IDENTIFIER, "+"}
    };

    double result = evaluateRpn(rpn, mock);
    EXPECT_DOUBLE_EQ(result, 5.0);
}

TEST(RpnEvaluatorTest, Function) {
    MockPluginRegistry mock;
    MockPluginRegistry::TokenInfo sinInfo{1, 90, Associativity::Left, false, sin_eval};

    EXPECT_CALL(mock, getTokenInfo("sin")).WillRepeatedly(ReturnRef(sinInfo));

    std::vector<Token> rpn = {
        {Token::TokenType::NUMBER, "1.5708", 1.5708},
        {Token::TokenType::IDENTIFIER, "sin"}
    };

    double result = evaluateRpn(rpn, mock);
    EXPECT_NEAR(result, 1.0, 1e-4);
}

TEST(RpnEvaluatorTest, ComplexRpn) {
    MockPluginRegistry mock;
    MockPluginRegistry::TokenInfo addInfo{2, 60, Associativity::Left, true, add_eval};
    MockPluginRegistry::TokenInfo mulInfo{2, 70, Associativity::Left, true, [](const double* a, size_t) {
        return a[0] * a[1];
    }};

    EXPECT_CALL(mock, getTokenInfo("+")).WillRepeatedly(ReturnRef(addInfo));
    EXPECT_CALL(mock, getTokenInfo("*")).WillRepeatedly(ReturnRef(mulInfo));

    // RPN for 2 + 3 * 4 → 2 3 4 * +
    std::vector<Token> rpn = {
        {Token::TokenType::NUMBER, "2", 2.0},
        {Token::TokenType::NUMBER, "3", 3.0},
        {Token::TokenType::NUMBER, "4", 4.0},
        {Token::TokenType::IDENTIFIER, "*"},
        {Token::TokenType::IDENTIFIER, "+"}
    };

    double result = evaluateRpn(rpn, mock);
    EXPECT_DOUBLE_EQ(result, 14.0);
}