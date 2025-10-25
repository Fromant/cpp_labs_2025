#include "RpnEvaluator.hpp"
#include <stack>
#include <stdexcept>


double evaluateRpn(const std::vector<Token>& rpn, const PluginManager& pm) {
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

            double result = info.evaluate(args.data(), args.size());
            values.push(result);
        }
    }

    if (values.size() != 1) {
        throw std::runtime_error("Invalid expression");
    }

    return values.top();
}