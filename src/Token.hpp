#pragma once

#include <string>
#include <utility>


struct Token {
    enum TokenType {
        NUMBER,
        OPERATOR,
        FUNCTION,
        LPAREN,
        RPAREN
    };
    TokenType type;
    std::string lexeme; // for FUNCTION and raw number string
    char op = 0;        // for OPERATOR: '+', '-', '*', '/'
    double value = 0.0; // for NUMBER

    explicit Token(TokenType t, std::string  l = "", double v = 0.0)
        : type(t), lexeme(std::move(l)), value(v) {}

    Token(TokenType t, char o)
        : type(t), op(o) {}
};

