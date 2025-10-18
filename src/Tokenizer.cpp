#include <vector>
#include <cctype>
#include <stdexcept>
#include <string>

#include "Token.hpp"

std::vector<Token> tokenize(const std::string& input) {
    std::vector<Token> tokens;
    size_t i = 0;

    auto skipWhitespace = [&]() {
        while (i < input.size() && std::isspace(static_cast<unsigned char>(input[i]))) ++i;
    };

    while (i < input.size()) {

        skipWhitespace();

        if (i >= input.size()) break;
        char c = input[i];

        if (std::isdigit(c) || c == '.') {
            // Parse number: supports 123, 12.34, .5, 1e-3 etc.
            size_t start = i;
            while (i < input.size() && (std::isdigit(input[i]) ||
                   input[i] == '.' || input[i] == 'e' || input[i] == 'E' ||
                   input[i] == '+' || input[i] == '-')) {
                // Allow +/- only after 'e' or 'E'
                if ((input[i] == '+' || input[i] == '-') && i > start &&
                    (input[i-1] == 'e' || input[i-1] == 'E')) {
                    ++i;
                } else if (std::isdigit(input[i]) ||
                           input[i] == '.' || input[i] == 'e' || input[i] == 'E') {
                    ++i;
                } else {
                    break;
                }
            }
            std::string numStr = input.substr(start, i - start);
            try {
                size_t pos;
                double val = std::stod(numStr, &pos);
                if (pos != numStr.size()) {
                    throw std::invalid_argument("Invalid number");
                }
                tokens.emplace_back(Token::TokenType::NUMBER, numStr, val);
            } catch (...) {
                throw std::runtime_error("Invalid number at position " + std::to_string(start));
            }
        }
        else if (c == '+' || c == '-' || c == '*' || c == '/') {
            tokens.emplace_back(Token::TokenType::OPERATOR, c);
            ++i;
        }
        else if (c == '(') {
            tokens.emplace_back(Token::TokenType::LPAREN);
            ++i;
        }
        else if (c == ')') {
            tokens.emplace_back(Token::TokenType::RPAREN);
            ++i;
        }
        else if (std::isalpha(c)) {
            // Parse function name: [a-zA-Z_][a-zA-Z0-9_]*
            size_t start = i;
            while (i < input.size() && (std::isalnum(input[i]) || input[i] == '_')) {
                ++i;
            }
            std::string name = input.substr(start, i - start);
            tokens.emplace_back(Token::TokenType::FUNCTION, name);
        }
        else {
            throw std::runtime_error("Unexpected character: '" + std::string(1, c) + "'");
        }
    }

    return tokens;
}
