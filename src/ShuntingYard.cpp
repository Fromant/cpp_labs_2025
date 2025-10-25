#include "ShuntingYard.hpp"

#include <stack>
#include <stdexcept>

std::vector<Token> shuntingYard(const std::vector<Token>& tokens, const IPluginRegistry& pm) {
    std::stack<Token> ops;
    std::vector<Token> output;

    for (const auto& token : tokens) {
        if (token.type == Token::TokenType::NUMBER) {
            output.push_back(token);
        } else if (token.type == Token::TokenType::IDENTIFIER) {
            // Validate: must be known token
            if (!pm.hasToken(token.lexeme)) {
                throw std::runtime_error("Unknown token: " + token.lexeme);
            }
            const auto& curr = pm.getTokenInfo(token.lexeme);

            if (curr.is_operator) {
                while (!ops.empty() && ops.top().type == Token::TokenType::IDENTIFIER) {
                    const auto& topTok = ops.top();
                    if (!pm.hasToken(topTok.lexeme)) break;
                    const auto& prev = pm.getTokenInfo(topTok.lexeme);
                    if (!prev.is_operator) break;

                    bool shouldPop = false;
                    if (curr.associativity == Associativity::Left) {
                        shouldPop = (curr.precedence <= prev.precedence);
                    } else { // Right
                        shouldPop = (curr.precedence < prev.precedence);
                    }

                    if (!shouldPop) break;

                    output.push_back(ops.top());
                    ops.pop();
                }
            }

            ops.push(token);
        } else if (token.type == Token::TokenType::COMMA) {
            while (!ops.empty() && ops.top().type != Token::TokenType::LPAREN) {
                output.push_back(ops.top());
                ops.pop();
            }
        } else if (token.type == Token::TokenType::LPAREN) {
            ops.push(token);
        } else if (token.type == Token::TokenType::RPAREN) {
            while (!ops.empty() && ops.top().type != Token::TokenType::LPAREN) {
                output.push_back(ops.top());
                ops.pop();
            }
            if (ops.empty()) {
                throw std::runtime_error("Mismatched parentheses");
            }
            ops.pop(); // remove '('

            // If function is on top, pop it
            if (!ops.empty() && ops.top().type == Token::TokenType::IDENTIFIER) {
                const auto& fn = pm.getTokenInfo(ops.top().lexeme);
                if (!fn.is_operator) {
                    output.push_back(ops.top());
                    ops.pop();
                }
            }
        }
    }

    while (!ops.empty()) {
        if (ops.top().type == Token::TokenType::LPAREN || ops.top().type == Token::TokenType::RPAREN) {
            throw std::runtime_error("Mismatched parentheses");
        }
        output.push_back(ops.top());
        ops.pop();
    }

    return output;
}