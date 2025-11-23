#pragma once
#include "../src/IPluginRegistry.hpp"
#include <gmock/gmock.h>

class MockPluginRegistry : public IPluginRegistry {
public:
    MOCK_METHOD(bool, hasFunction, (const std::string& name), (const, override));
    MOCK_METHOD(bool, hasUnaryOperator, (const std::string& name), (const, override));
    MOCK_METHOD(bool, hasBinaryOperator, (const std::string& name), (const, override));

    MOCK_METHOD(const TokenInfo&, getFunction, (const std::string& name), (const, override));
    MOCK_METHOD(const TokenInfo&, getUnaryOperator, (const std::string& name), (const, override));
    MOCK_METHOD(const TokenInfo&, getBinaryOperator, (const std::string& name), (const, override));
};