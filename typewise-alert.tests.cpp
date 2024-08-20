#include <gtest/gtest.h>
#include "typewise-alert.h"

class TypewiseAlertTest : public ::testing::Test {
protected:
    // Setup code (if needed) goes here
    void SetUp() override {
        // Code to set up before each test case, if needed
    }

    // Tear down code (if needed) goes here
    void TearDown() override {
        // Code to clean up after each test case, if needed
    }
};

// Test for getLimitsForCoolingType
TEST_F(TypewiseAlertTest, GetLimitsForCoolingType_ValidCoolingType) {
    TypewiseAlert::CoolingLimits limits = TypewiseAlert::getLimitsForCoolingType(TypewiseAlert::CoolingType::PASSIVE_COOLING);
    EXPECT_EQ(limits.coolingType, TypewiseAlert::CoolingType::PASSIVE_COOLING);
    EXPECT_EQ(limits.lowerLimit, 0);
    EXPECT_EQ(limits.upperLimit, 35);
}

TEST_F(TypewiseAlertTest, GetLimitsForCoolingType_UnknownCoolingType) {
    TypewiseAlert::CoolingLimits limits = TypewiseAlert::getLimitsForCoolingType(static_cast<TypewiseAlert::CoolingType>(999)); // Invalid type
    EXPECT_EQ(limits.coolingType, static_cast<TypewiseAlert::CoolingType>(999));
    EXPECT_EQ(limits.lowerLimit, 0);
    EXPECT_EQ(limits.upperLimit, 0);
}

// Test for inferBreach
TEST_F(TypewiseAlertTest, InferBreach_TooLow) {
    TypewiseAlert::BreachType breach = TypewiseAlert::inferBreach(-10, 0, 35);
    EXPECT_EQ(breach, TypewiseAlert::BreachType::TOO_LOW);
}

TEST_F(TypewiseAlertTest, InferBreach_TooHigh) {
    TypewiseAlert::BreachType breach = TypewiseAlert::inferBreach(40, 0, 35);
    EXPECT_EQ(breach, TypewiseAlert::BreachType::TOO_HIGH);
}

TEST_F(TypewiseAlertTest, InferBreach_Normal) {
    TypewiseAlert::BreachType breach = TypewiseAlert::inferBreach(20, 0, 35);
    EXPECT_EQ(breach, TypewiseAlert::BreachType::NORMAL);
}

// Test for classifyTemperatureBreach
TEST_F(TypewiseAlertTest, ClassifyTemperatureBreach) {
    TypewiseAlert::BreachType breach = TypewiseAlert::classifyTemperatureBreach(TypewiseAlert::CoolingType::PASSIVE_COOLING, 30);
    EXPECT_EQ(breach, TypewiseAlert::BreachType::NORMAL);

    breach = TypewiseAlert::classifyTemperatureBreach(TypewiseAlert::CoolingType::PASSIVE_COOLING, 40);
    EXPECT_EQ(breach, TypewiseAlert::BreachType::TOO_HIGH);

    breach = TypewiseAlert::classifyTemperatureBreach(TypewiseAlert::CoolingType::PASSIVE_COOLING, -10);
    EXPECT_EQ(breach, TypewiseAlert::BreachType::TOO_LOW);
}

// Test for checkAndAlert
// Note: This test would involve checking side effects or output, which may require mocking or redirection of output.

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
