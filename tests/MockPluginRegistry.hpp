#pragma once
#include "../src/IPluginRegistry.hpp"
#include <gmock/gmock.h>

class MockPluginRegistry : public IPluginRegistry {
public:
    MOCK_METHOD(bool, hasToken, (const std::string& name), (const, override));
    MOCK_METHOD(const TokenInfo&, getTokenInfo, (const std::string& name), (const, override));
};