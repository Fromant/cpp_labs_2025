#pragma once

#include <string>
#include <utility>


struct Token {
    enum TokenType {
        NUMBER,
        IDENTIFIER, //operator or function
        LPAREN,
        RPAREN,
        COMMA
    };
    TokenType type;
    std::string lexeme; // for FUNCTION and OPERATOR raw number string
    double value = 0.0; // for NUMBER
};

