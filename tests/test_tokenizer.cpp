#include <gtest/gtest.h>

#include "../src/Tokenizer.hpp"
#include "src/Token.hpp"

TEST(TokenizerTest, ParsesNumbers) {
    auto tokens = tokenize("42");
    ASSERT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].type, Token::TokenType::NUMBER);
    EXPECT_DOUBLE_EQ(tokens[0].value, 42.0);
}

TEST(TokenizerTest, ParsesFloats) {
    auto tokens = tokenize("3.1415");
    ASSERT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].type, Token::TokenType::NUMBER);
    EXPECT_DOUBLE_EQ(tokens[0].value, 3.1415);
}

TEST(TokenizerTest, ParsesOperatorsAsIdentifiers) {
    auto tokens = tokenize("+ - * / ^");
    ASSERT_EQ(tokens.size(), 5);
    EXPECT_EQ(tokens[0].type, Token::TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[0].lexeme, "+");
    EXPECT_EQ(tokens[1].lexeme, "-");
    EXPECT_EQ(tokens[2].lexeme, "*");
    EXPECT_EQ(tokens[3].lexeme, "/");
    EXPECT_EQ(tokens[4].lexeme, "^");
}

TEST(TokenizerTest, ParsesFunctionCalls) {
    auto tokens = tokenize("sin(90)");
    ASSERT_EQ(tokens.size(), 4);
    EXPECT_EQ(tokens[0].type, Token::TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[0].lexeme, "sin");
    EXPECT_EQ(tokens[1].type, Token::TokenType::LPAREN);
    EXPECT_EQ(tokens[2].type, Token::TokenType::NUMBER);
    EXPECT_EQ(tokens[2].value, 90.0);
    EXPECT_EQ(tokens[3].type, Token::TokenType::RPAREN);
}

TEST(TokenizerTest, ParsesComplexExpression) {
    auto tokens = tokenize("2^3 + sin(1.57)");
    ASSERT_EQ(tokens.size(), 8);
    EXPECT_EQ(tokens[0].lexeme, "2");
    EXPECT_EQ(tokens[1].lexeme, "^");
    EXPECT_EQ(tokens[2].lexeme, "3");
    EXPECT_EQ(tokens[3].lexeme, "+");
    EXPECT_EQ(tokens[4].lexeme, "sin");
    EXPECT_EQ(tokens[5].type, Token::TokenType::LPAREN);
    EXPECT_EQ(tokens[6].type, Token::TokenType::NUMBER);
    EXPECT_EQ(tokens[6].value, 1.57);
    EXPECT_EQ(tokens[7].type, Token::TokenType::RPAREN);
}