#include <gtest/gtest.h>
#include <any>
#include <string>
#include <vector>

#include "Engine.hpp"
#include "Wrapper.hpp"

class Subject {
public:
    int double_it(int x) { return x * 2; }
    double sum(double a, double b) { return a + b; }
    void set_sum(int arg1, int arg2) { last_called = arg1 + arg2; }
    std::string concat_num_to_string(const std::string& s, int n) { return s + std::to_string(n); }

    int last_called = 0;
};


TEST(EngineTest, RegisterAndExecuteCommand) {
    Engine engine;
    Subject subj;
    Wrapper wrapper(subj, &Subject::double_it, {{"x", 0}});
    engine.register_command("double_it", wrapper);

    auto result = engine.execute("double_it", {{"x", 5}});
    EXPECT_EQ(std::any_cast<int>(result), 10);
}

TEST(EngineTest, ExecuteNonExistentCommandThrows) {
    Engine engine;
    EXPECT_THROW(
        engine.execute("nonexistent", {}),
        std::invalid_argument
    );
}

TEST(EngineTest, ExecuteCommandWithWrongArgTypePropagatesError) {
    Engine engine;
    Subject subj;
    Wrapper wrapper(subj, &Subject::double_it, {{"x", 0}});
    engine.register_command("double_it", wrapper);

    EXPECT_THROW(
        engine.execute("double_it", {{"x", std::string("hello")}}),
        std::invalid_argument
    );
}

TEST(EngineTest, RegisterWithCtorArgs) {
    Engine engine;
    Subject subj;
    engine.register_command("double", &subj, &Subject::double_it, WrapperBase::ArgList{{"x", 0}});
}

TEST(EngineTest, MultipleCommands) {
    Engine engine;
    Subject subj;

    engine.register_command("double", &subj, &Subject::double_it, WrapperBase::ArgList{{"x", 0}});
    engine.register_command("add", &subj, &Subject::sum, {{"a", 0.0}, {"b", 0.0}});
    engine.register_command("set_last", &subj, &Subject::set_sum, {{"arg1", 0}, {"arg2", 0}});

    auto result1 = engine.execute("double", {{"x", 3}});
    EXPECT_EQ(std::any_cast<int>(result1), 6);

    auto result2 = engine.execute("add", {{"a", 2.5}, {"b", 3.5}});
    EXPECT_DOUBLE_EQ(std::any_cast<double>(result2), 6.0);

    engine.execute("set_last", {{"arg1", 10}, {"arg2", 20}});
    EXPECT_EQ(subj.last_called, 30);
}


TEST(EngineTest, RegisterWithConstRef) {
    Subject subj;
    Engine engine;
    engine.register_command("sum", subj, &Subject::set_sum, {{"arg1", 0}, {"arg2", 1}});
    auto result = engine.execute("sum", {{"arg1", 42}});
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(subj.last_called, 0); //original object is not changed
}

TEST(EngineTest, RegisterWithRawPtr) {
    auto* subj = new Subject();
    Engine engine;
    engine.register_command("sum", subj, &Subject::set_sum, {{"arg1", 0}, {"arg2", 1}});
    auto result = engine.execute("sum", {{"arg1", 42}});
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(subj->last_called, 43); //original object is changed
}

TEST(EngineTest, RegisterWithUniquePtr) {
    auto subj = std::make_unique<Subject>();
    Subject* observer = subj.get();

    Engine engine;
    engine.register_command("sum", std::move(subj), &Subject::set_sum, {{"arg1", 0}, {"arg2", 1}});
    auto result = engine.execute("sum", {{"arg1", 42}});
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(observer->last_called, 43); //original object is changed
}

TEST(EngineTest, RegisterWithSharedPtr) {
    std::shared_ptr<Subject> subj = std::make_shared<Subject>();
    Engine engine;
    engine.register_command("sum", subj, &Subject::set_sum, {{"arg1", 0}, {"arg2", 1}});
    auto result = engine.execute("sum", {{"arg1", 42}});
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(subj->last_called, 43); //original object is changed
}
