#include "Calculator.hpp"
#include "Tokenizer.hpp"
#include "ShuntingYard.hpp"
#include "RpnEvaluator.hpp"
#include "PluginManager.hpp"

double Calculator::evaluate(const std::string& expr) {
    auto tokens = tokenize(expr);
    auto rpn = shuntingYard(tokens, pm);
    return evaluateRpn(rpn, pm);
}
