#include "typewise-alert.h"
#include <gtest/gtest.h>
#include <tuple>

class ClassifyTemperatureBreachTest : public ::testing::TestWithParam<std::tuple<TypewiseAlert::CoolingType, double, TypewiseAlert::BreachType>> {
};

TEST_P(ClassifyTemperatureBreachTest, ClassifyTemperatureBreach) {
    TypewiseAlert::CoolingType coolingType;
    double temperatureInC;
    TypewiseAlert::BreachType expectedBreachType;
    
    std::tie(coolingType, temperatureInC, expectedBreachType) = GetParam();
    
    EXPECT_EQ(TypewiseAlert::classifyTemperatureBreach(coolingType, temperatureInC), expectedBreachType);
}

INSTANTIATE_TEST_SUITE_P(
    BreachClassificationTests,
    ClassifyTemperatureBreachTest,
    ::testing::Values(
        std::make_tuple(TypewiseAlert::CoolingType::PASSIVE_COOLING, 15.0, TypewiseAlert::BreachType::TOO_LOW),
        std::make_tuple(TypewiseAlert::CoolingType::PASSIVE_COOLING, 25.0, TypewiseAlert::BreachType::NORMAL),
        std::make_tuple(TypewiseAlert::CoolingType::PASSIVE_COOLING, 40.0, TypewiseAlert::BreachType::TOO_HIGH),
        
        std::make_tuple(TypewiseAlert::CoolingType::HI_ACTIVE_COOLING, -5.0, TypewiseAlert::BreachType::TOO_LOW),
        std::make_tuple(TypewiseAlert::CoolingType::HI_ACTIVE_COOLING, 30.0, TypewiseAlert::BreachType::NORMAL),
        std::make_tuple(TypewiseAlert::CoolingType::HI_ACTIVE_COOLING, 50.0, TypewiseAlert::BreachType::TOO_HIGH),

        std::make_tuple(TypewiseAlert::CoolingType::MED_ACTIVE_COOLING, 0.0, TypewiseAlert::BreachType::NORMAL),
        std::make_tuple(TypewiseAlert::CoolingType::MED_ACTIVE_COOLING, -1.0, TypewiseAlert::BreachType::TOO_LOW),
        std::make_tuple(TypewiseAlert::CoolingType::MED_ACTIVE_COOLING, 41.0, TypewiseAlert::BreachType::TOO_HIGH)
    )
);

// Mocking a test for sending alerts to the controller
TEST(TypewiseAlertTest, CheckAndAlertToController) {
    // Redirect stdout to a stringstream to check output
    testing::internal::CaptureStdout();
    
    TypewiseAlert::BatteryCharacter batteryChar = { TypewiseAlert::CoolingType::PASSIVE_COOLING, "BrandA" };
    TypewiseAlert::checkAndAlert(TypewiseAlert::AlertTarget::TO_CONTROLLER, batteryChar, 50);
    
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, "feed : 2\n");  // TOO_HIGH is expected to be 2
}

// Mocking a test for sending alerts to email
TEST(TypewiseAlertTest, CheckAndAlertToEmail) {
    // Redirect stdout to a stringstream to check output
    testing::internal::CaptureStdout();
    
    TypewiseAlert::BatteryCharacter batteryChar = { TypewiseAlert::CoolingType::PASSIVE_COOLING, "BrandA" };
    TypewiseAlert::checkAndAlert(TypewiseAlert::AlertTarget::TO_EMAIL, batteryChar, 50);
    
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, "To: a.b@c.com\nHi, the temperature is too high\n");
}

// Test to ensure normal temperatures do not trigger alerts
TEST(TypewiseAlertTest, NoAlertForNormalTemperature) {
    // Redirect stdout to a stringstream to check output
    testing::internal::CaptureStdout();
    
    TypewiseAlert::BatteryCharacter batteryChar = { TypewiseAlert::CoolingType::HI_ACTIVE_COOLING, "BrandB" };
    TypewiseAlert::checkAndAlert(TypewiseAlert::AlertTarget::TO_EMAIL, batteryChar, 40);
    
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, "");  // Expect no output since temperature is normal
}
