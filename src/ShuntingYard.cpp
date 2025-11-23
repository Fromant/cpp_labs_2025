#include "ShuntingYard.hpp"
#include <stdexcept>

std::vector<Token> shuntingYard(const std::vector<Token>& tokens, const IPluginRegistry& pm) {
    std::vector<Token> output;
    std::vector<Token> ops;

    bool expectOperand = true;

    for (const auto& token : tokens) {
        if (token.type == Token::NUMBER) {
            output.push_back(token);
            expectOperand = false;
        }
        else if (token.type == Token::LPAREN) {
            ops.push_back(token);
            expectOperand = true;
        }
        else if (token.type == Token::RPAREN) {
            while (!ops.empty() && ops.back().type != Token::LPAREN) {
                output.push_back(ops.back());
                ops.pop_back();
            }
            if (ops.empty()) {
                throw std::runtime_error("Mismatched parentheses");
            }
            ops.pop_back(); // remove '('

            if (!ops.empty() && ops.back().type == Token::FUNCTION) {
                output.push_back(ops.back());
                ops.pop_back();
            }
            expectOperand = false;
        }
        else if (token.type == Token::COMMA) {
            while (!ops.empty() && ops.back().type != Token::LPAREN) {
                output.push_back(ops.back());
                ops.pop_back();
            }
            if (ops.empty()) {
                throw std::runtime_error("Misplaced comma");
            }
            expectOperand = true;
        }
        else if (token.type == Token::TokenType::IDENTIFIER) {
            std::string name = token.lexeme;

            if (expectOperand) {
                if (pm.hasUnaryOperator(name)) {
                    Token t = token;
                    t.type = Token::UNARY_OPERATOR;
                    ops.push_back(t);
                }
                else if (pm.hasFunction(name)) {
                    Token t = token;
                    t.type = Token::FUNCTION;
                    ops.push_back(t);
                    expectOperand = true;
                }
                else {
                    throw std::runtime_error("Unexpected token at operand position: " + name);
                }
            }
            else {
                if (!pm.hasBinaryOperator(name)) {
                    throw std::runtime_error("Unknown binary operator: " + name);
                }
                const auto& info = pm.getBinaryOperator(name);

                while (!ops.empty()) {
                    const Token& top = ops.back();
                    if (top.type == Token::LPAREN) break;

                    if (top.type != Token::BINARY_OPERATOR && top.type != Token::UNARY_OPERATOR)
                        break;

                    const auto* topInfo = (top.type == Token::BINARY_OPERATOR)
                                              ? &pm.getBinaryOperator(top.lexeme)
                                              : &pm.getUnaryOperator(top.lexeme);

                    bool shouldPop = false;
                    if (info.associativity == Associativity::Left) {
                        shouldPop = (topInfo->precedence >= info.precedence);
                    }
                    else {
                        shouldPop = (topInfo->precedence > info.precedence);
                    }

                    if (!shouldPop) break;

                    output.push_back(ops.back());
                    ops.pop_back();
                }

                Token t = token;
                t.type = Token::BINARY_OPERATOR;
                ops.push_back(t);
                expectOperand = true;
            }
        }
        else {
            throw std::runtime_error("Unknown token type");
        }
    }

    while (!ops.empty()) {
        if (ops.back().type == Token::LPAREN || ops.back().type == Token::RPAREN) {
            throw std::runtime_error("Mismatched parentheses");
        }
        output.push_back(ops.back());
        ops.pop_back();
    }

    return output;
}
