#include "typewise-alert.h"
#include <gtest/gtest.h>

TEST(TypewiseAlertTest, ClassifyTemperatureBreach) {
    EXPECT_EQ(TypewiseAlert::classifyTemperatureBreach(TypewiseAlert::CoolingType::PASSIVE_COOLING, 25), TypewiseAlert::BreachType::NORMAL);
    EXPECT_EQ(TypewiseAlert::classifyTemperatureBreach(TypewiseAlert::CoolingType::PASSIVE_COOLING, 15), TypewiseAlert::BreachType::TOO_LOW);
    EXPECT_EQ(TypewiseAlert::classifyTemperatureBreach(TypewiseAlert::CoolingType::PASSIVE_COOLING, 35), TypewiseAlert::BreachType::TOO_HIGH);

    EXPECT_EQ(TypewiseAlert::classifyTemperatureBreach(TypewiseAlert::CoolingType::HI_ACTIVE_COOLING, 40), TypewiseAlert::BreachType::NORMAL);
    EXPECT_EQ(TypewiseAlert::classifyTemperatureBreach(TypewiseAlert::CoolingType::HI_ACTIVE_COOLING, 50), TypewiseAlert::BreachType::TOO_HIGH);

    EXPECT_EQ(TypewiseAlert::classifyTemperatureBreach(TypewiseAlert::CoolingType::MED_ACTIVE_COOLING, 20), TypewiseAlert::BreachType::NORMAL);
    EXPECT_EQ(TypewiseAlert::classifyTemperatureBreach(TypewiseAlert::CoolingType::MED_ACTIVE_COOLING, -5), TypewiseAlert::BreachType::TOO_LOW);
}
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
