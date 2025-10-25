#include "Tokenizer.hpp"

#include <cctype>
#include <stdexcept>

std::vector<Token> tokenize(const std::string& expr) {
    std::vector<Token> tokens;
    size_t i = 0;

    auto skipSpaces = [&]() {
        while (i < expr.size() && std::isspace(static_cast<unsigned char>(expr[i]))) ++i;
    };

    auto isDigit = [](char c) { return std::isdigit(static_cast<unsigned char>(c)) || c == '.'; };

    while (i < expr.size()) {
        skipSpaces();
        if (i >= expr.size()) break;

        char c = expr[i];

        // Numbers
        if (isDigit(c)) {
            size_t start = i;
            while (i < expr.size() && isDigit(expr[i])) ++i;
            std::string numStr = expr.substr(start, i - start);
            try {
                double val = std::stod(numStr);
                tokens.push_back({Token::TokenType::NUMBER, numStr, val});
            } catch (...) {
                throw std::runtime_error("Invalid number: " + numStr);
            }
        }
        // Parentheses and comma
        else if (c == '(') {
            tokens.push_back({Token::TokenType::LPAREN, "("});
            ++i;
        } else if (c == ')') {
            tokens.push_back({Token::TokenType::RPAREN, ")"});
            ++i;
        } else if (c == ',') {
            tokens.push_back({Token::TokenType::COMMA, ","});
            ++i;
        }
        // Everything else: treat as identifier (including +, -, *, /, ^, @, etc.)
        else {
            size_t start = i;
            // Take **one character** as identifier (for symbols like +, ^)
            // But allow multi-char names like "sin", "max"
            if (!std::isalpha(c) && c != '_') {
                // Single-symbol token (e.g. '+', '^', '@')
                tokens.push_back({Token::TokenType::IDENTIFIER, std::string(1, c)});
                ++i;
            } else {
                // Multi-character identifier (e.g. "sin", "log")
                while (i < expr.size() && (std::isalnum(static_cast<unsigned char>(expr[i])) || expr[i] == '_')) {
                    ++i;
                }
                tokens.push_back({Token::TokenType::IDENTIFIER, expr.substr(start, i - start)});
            }
        }
    }

    return tokens;
}