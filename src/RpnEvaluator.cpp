#include "RpnEvaluator.hpp"
#include "IPluginRegistry.hpp"
#include <stack>
#include <stdexcept>

double evaluateRpn(const std::vector<Token>& rpn, const IPluginRegistry& pm) {
    std::stack<double> values;

    for (const auto& token : rpn) {
        if (token.type == Token::NUMBER) {
            values.push(token.value);
        } else {
            const IPluginRegistry::TokenInfo* info = nullptr;

            switch (token.type) {
                case Token::FUNCTION:
                    info = &pm.getFunction(token.lexeme);
                    break;
                case Token::UNARY_OPERATOR:
                    info = &pm.getUnaryOperator(token.lexeme);
                    break;
                case Token::BINARY_OPERATOR:
                    info = &pm.getBinaryOperator(token.lexeme);
                    break;
                default:
                    throw std::runtime_error("Invalid token in RPN");
            }

            if (values.size() < static_cast<size_t>(info->arity)) {
                throw std::runtime_error("Not enough arguments for " + token.lexeme);
            }

            std::vector<double> args(info->arity);
            for (int i = info->arity - 1; i >= 0; --i) {
                args[i] = values.top();
                values.pop();
            }

            PluginResult result = info->evaluate(args.data(), args.size());
            if (result.error != nullptr) {
                throw std::runtime_error("Runtime error in " + token.lexeme + ": " + std::string(result.error));
            }
            values.push(result.value);
        }
    }

    if (values.size() != 1) {
        throw std::runtime_error("Invalid RPN expression");
    }

    return values.top();
}