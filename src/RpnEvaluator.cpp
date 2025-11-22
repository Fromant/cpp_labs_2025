#include "RpnEvaluator.hpp"
#include <stack>
#include <stdexcept>


double evaluateRpn(const std::vector<Token>& rpn, const IPluginRegistry& pm) {
    std::stack<double> values;

    for (const auto& token : rpn) {
        if (token.type == Token::TokenType::NUMBER) {
            values.push(token.value);
        } else if (token.type == Token::TokenType::IDENTIFIER) {
            const auto& info = pm.getTokenInfo(token.lexeme);
            if (values.size() < static_cast<size_t>(info.arity)) {
                throw std::runtime_error("Not enough arguments for " + token.lexeme);
            }

            std::vector<double> args(info.arity);
            for (int i = info.arity - 1; i >= 0; --i) {
                args[i] = values.top();
                values.pop();
            }

            PluginResult result = info.evaluate(args.data(), args.size());
            if (result.error != nullptr) {
                throw std::runtime_error("Error: " + token.lexeme + result.error);
            }
            values.push(result.value);
        }
    }

    if (values.size() != 1) {
        throw std::runtime_error("Invalid expression");
    }

    return values.top();
}