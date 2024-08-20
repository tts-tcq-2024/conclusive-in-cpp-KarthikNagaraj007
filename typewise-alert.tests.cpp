#include "typewise-alert.h"
#include <gtest/gtest.h>
#include <sstream>

// Helper function to capture printed output
std::string captureOutput(void (*func)(TypewiseAlert::BreachType), TypewiseAlert::BreachType breachType) {
    std::ostringstream oss;
    std::streambuf* oldCoutStreamBuf = std::cout.rdbuf();
    std::cout.rdbuf(oss.rdbuf());

    func(breachType);

    std::cout.rdbuf(oldCoutStreamBuf);
    return oss.str();
}

// Test classifyTemperatureBreach function
TEST(TypewiseAlertTest, ClassifyTemperatureBreach) {
    // Test PASSIVE_COOLING
    EXPECT_EQ(TypewiseAlert::classifyTemperatureBreach(TypewiseAlert::CoolingType::PASSIVE_COOLING, 34.0), TypewiseAlert::BreachType::NORMAL);
    EXPECT_EQ(TypewiseAlert::classifyTemperatureBreach(TypewiseAlert::CoolingType::PASSIVE_COOLING, 36.0), TypewiseAlert::BreachType::TOO_HIGH);
    EXPECT_EQ(TypewiseAlert::classifyTemperatureBreach(TypewiseAlert::CoolingType::PASSIVE_COOLING, -1.0), TypewiseAlert::BreachType::TOO_LOW);

    // Test HI_ACTIVE_COOLING
    EXPECT_EQ(TypewiseAlert::classifyTemperatureBreach(TypewiseAlert::CoolingType::HI_ACTIVE_COOLING, 44.0), TypewiseAlert::BreachType::NORMAL);
    EXPECT_EQ(TypewiseAlert::classifyTemperatureBreach(TypewiseAlert::CoolingType::HI_ACTIVE_COOLING, 46.0), TypewiseAlert::BreachType::TOO_HIGH);
    EXPECT_EQ(TypewiseAlert::classifyTemperatureBreach(TypewiseAlert::CoolingType::HI_ACTIVE_COOLING, -1.0), TypewiseAlert::BreachType::TOO_LOW);

    // Test MED_ACTIVE_COOLING
    EXPECT_EQ(TypewiseAlert::classifyTemperatureBreach(TypewiseAlert::CoolingType::MED_ACTIVE_COOLING, 39.0), TypewiseAlert::BreachType::NORMAL);
    EXPECT_EQ(TypewiseAlert::classifyTemperatureBreach(TypewiseAlert::CoolingType::MED_ACTIVE_COOLING, 41.0), TypewiseAlert::BreachType::TOO_HIGH);
    EXPECT_EQ(TypewiseAlert::classifyTemperatureBreach(TypewiseAlert::CoolingType::MED_ACTIVE_COOLING, -1.0), TypewiseAlert::BreachType::TOO_LOW);
}

// Test sendToController function
TEST(TypewiseAlertTest, SendToController) {
    std::string output = captureOutput(TypewiseAlert::sendToController, TypewiseAlert::BreachType::TOO_HIGH);
    EXPECT_TRUE(output.find("feed") != std::string::npos);
    EXPECT_TRUE(output.find("1") != std::string::npos);
}

// Test sendToEmail function
TEST(TypewiseAlertTest, SendToEmail) {
    std::string output = captureOutput(TypewiseAlert::sendToEmail, TypewiseAlert::BreachType::TOO_HIGH);
    EXPECT_TRUE(output.find("To: a.b@c.com") != std::string::npos);
    EXPECT_TRUE(output.find("Hi, the temperature is too high") != std::string::npos);

    output = captureOutput(TypewiseAlert::sendToEmail, TypewiseAlert::BreachType::NORMAL);
    EXPECT_TRUE(output.empty());
}

// Test checkAndAlert function
TEST(TypewiseAlertTest, CheckAndAlert) {
    TypewiseAlert::BatteryCharacter batteryChar = { TypewiseAlert::CoolingType::PASSIVE_COOLING, "BrandX" };

    // Capture output for email alert
    std::string emailOutput = captureOutput([](TypewiseAlert::BreachType breachType) {
        TypewiseAlert::sendToEmail(breachType);
    }, TypewiseAlert::classifyTemperatureBreach(TypewiseAlert::CoolingType::PASSIVE_COOLING, 36.0));
    EXPECT_TRUE(emailOutput.find("To: a.b@c.com") != std::string::npos);
    EXPECT_TRUE(emailOutput.find("Hi, the temperature is too high") != std::string::npos);

    // Capture output for controller alert
    std::string controllerOutput = captureOutput([](TypewiseAlert::BreachType breachType) {
        TypewiseAlert::sendToController(breachType);
    }, TypewiseAlert::classifyTemperatureBreach(TypewiseAlert::CoolingType::PASSIVE_COOLING, 36.0));
    EXPECT_TRUE(controllerOutput.find("feed") != std::string::npos);
    EXPECT_TRUE(controllerOutput.find("1") != std::string::npos);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
