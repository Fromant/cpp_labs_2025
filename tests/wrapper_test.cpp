#include <gtest/gtest.h>
#include <any>
#include <string>
#include <vector>

#include "Wrapper.hpp"

class Subject {
public:
    int double_it(int x) { return x * 2; }
    double sum(double a, double b) { return a + b; }
    void set_sum(int arg1, int arg2) { last_called = arg1 + arg2; }
    std::string concat_num_to_string(const std::string& s, int n) { return s + std::to_string(n); }

    int last_called = 0;
};

TEST(WrapperTest, ConstructorValidArgs) {
    Subject subj;
    Wrapper wrapper(subj, &Subject::double_it, {{"x", 0}});
    EXPECT_NO_THROW(wrapper.execute({{"x", 5}}));
}

TEST(WrapperTest, ConstructorInvalidArgCountThrows) {
    Subject subj;
    const auto ctor = [&subj]() {
        Wrapper wrapper(subj, &Subject::double_it, {{"x", 0}, {"y", 0}});
    };
    EXPECT_THROW(
        ctor(),
        std::invalid_argument
    );
}

TEST(WrapperTest, ExecuteWithDefaultValues) {
    Subject subj;
    Wrapper wrapper(subj, &Subject::double_it, {{"x", 10}});
    auto result = wrapper.execute({});
    EXPECT_EQ(std::any_cast<int>(result), 20); // 10 * 2
}

TEST(WrapperTest, ExecuteWithProvidedValues) {
    Subject subj;
    Wrapper wrapper(subj, &Subject::double_it, {{"x", 0}});
    auto result = wrapper.execute({{"x", 7}});
    EXPECT_EQ(std::any_cast<int>(result), 14); // 7 * 2
}

TEST(WrapperTest, ExecuteWithWrongArgumentNameIgnored) {
    Subject subj;
    Wrapper wrapper(subj, &Subject::double_it, {{"x", 0}});
    auto result = wrapper.execute({{"wrong_name", 99}});
    EXPECT_EQ(std::any_cast<int>(result), 0);
}

TEST(WrapperTest, ExecuteWithTypeMismatchThrows) {
    Subject subj;
    Wrapper wrapper(subj, &Subject::double_it, {{"x", 0}});
    EXPECT_THROW(
        wrapper.execute({{"x", std::string("not_an_int")}}),
        std::invalid_argument
    );
}

TEST(WrapperTest, ExecuteVoidFunction) {
    Subject subj;
    Wrapper wrapper(&subj, &Subject::set_sum, {{"arg1", 0}, {"arg2", 0}});
    EXPECT_NO_THROW(wrapper.execute({{"arg1", 3}, {"arg2", 4}}));
    EXPECT_EQ(subj.last_called, 7); // 3+4
}

TEST(WrapperTest, ExecuteVoidFunctionReturnsEmptyAny) {
    Subject subj;
    Wrapper wrapper(subj, &Subject::set_sum, {{"arg1", 0}, {"arg2", 0}});
    auto result = wrapper.execute({{"arg1", 1}, {"arg2", 2}});
    EXPECT_FALSE(result.has_value()); //void = no value
}

TEST(WrapperTest, CopyConstructor) {
    Subject subj;
    Wrapper wrapper1(subj, &Subject::double_it, {{"x", 0}});
    Wrapper wrapper2(wrapper1);
    auto result = wrapper2.execute({{"x", 6}});
    EXPECT_EQ(std::any_cast<int>(result), 12);
}

TEST(WrapperTest, MoveConstructor) {
    Subject subj;
    Wrapper wrapper1(subj, &Subject::double_it, {{"x", 0}});
    Wrapper wrapper2(std::move(wrapper1));
    auto result = wrapper2.execute({{"x", 8}});
    EXPECT_EQ(std::any_cast<int>(result), 16);
}

TEST(WrapperTest, ExecuteWithTooManyArgsThrows) {
    Subject subj;
    Wrapper wrapper(subj, &Subject::double_it, {{"x", 0}});
    EXPECT_THROW(
        wrapper.execute({{"x", 5}, {"extra", 99}}), // extra arg
        std::invalid_argument
    );
}

TEST(WrapperTest, FunctionWithMultipleParameters) {
    Subject subj;
    Wrapper wrapper(subj, &Subject::sum, {{"a", 0.0}, {"b", 0.0}});
    auto result = wrapper.execute({{"a", 1.5}, {"b", 2.5}});
    EXPECT_DOUBLE_EQ(std::any_cast<double>(result), 4.0);
}

TEST(WrapperTest, FunctionWithStringParameter) {
    Subject subj;
    Wrapper wrapper(subj, &Subject::concat_num_to_string, {{"s", std::string("")}, {"n", 0}});
    auto result = wrapper.execute({{"s", std::string("Hello")}, {"n", 42}});
    EXPECT_EQ(std::any_cast<std::string>(result), "Hello42");
}

TEST(WrapperTest, DefaultValueForString) {
    Subject subj;
    Wrapper wrapper(subj, &Subject::concat_num_to_string, {{"s", std::string("Default")}, {"n", 0}});
    auto result = wrapper.execute({{"n", 100}});
    EXPECT_EQ(std::any_cast<std::string>(result), "Default100");
}

TEST(WrapperTest, ExecuteVoidFunctionWithNoReturn) {
    Subject subj;
    Wrapper wrapper(&subj, &Subject::set_sum, {{"arg1", 0}, {"arg2", 1}});
    auto result = wrapper.execute({{"arg1", 42}});
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(subj.last_called, 43);
}


TEST(WrapperTest, ConstructWithConstRef) {
    Subject subj;
    Wrapper wrapper(subj, &Subject::set_sum, {{"arg1", 0}, {"arg2", 1}}); //copies subj internally
    auto result = wrapper.execute({{"arg1", 42}});
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(subj.last_called, 0); //original object is not changed
}

TEST(WrapperTest, ConstructWithRawPtr) {
    auto* subj = new Subject();
    Wrapper wrapper(subj, &Subject::set_sum, {{"arg1", 0}, {"arg2", 1}}); //just stores raw ptr
    auto result = wrapper.execute({{"arg1", 42}});
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(subj->last_called, 43); //original object is changed

    delete subj; //after deleting executing wrapper is UB, likely SEGFAULT
}

TEST(WrapperTest, ConstructWithUniquePtr) {
    auto subj = std::make_unique<Subject>();
    Subject* observer = subj.get();

    Wrapper wrapper(std::move(subj), &Subject::set_sum, {{"arg1", 0}, {"arg2", 1}}); //unique ptrs have to be moved

    auto result = wrapper.execute({{"arg1", 42}});
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(observer->last_called, 43);
}

TEST(WrapperTest, ConstructWithSharedPtr) {
    std::shared_ptr<Subject> subj = std::make_shared<Subject>();
    Wrapper wrapper(subj, &Subject::set_sum, {{"arg1", 0}, {"arg2", 1}}); //copies shared ptr inside. Can be moved tho
    auto result = wrapper.execute({{"arg1", 42}});
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(subj->last_called, 43); //original object is changed
}