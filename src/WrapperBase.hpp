#pragma once
#include <any>
#include <vector>
#include <string>

class WrapperBase {
public:
    using ArgList = std::vector<std::pair<std::string, std::any>>;

    virtual std::any execute(const ArgList& args) const = 0;
    virtual ~WrapperBase() = default;
};