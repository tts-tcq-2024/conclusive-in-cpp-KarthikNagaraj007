#include "typewise-alert.h"
#include <gtest/gtest.h>
#include <sstream>

// Helper function to capture std::cout output
class OutputCapture {
public:
    OutputCapture() {
        oldBuffer = std::cout.rdbuf();
        std::cout.rdbuf(ss.rdbuf());
    }
    ~OutputCapture() {
        std::cout.rdbuf(oldBuffer);
    }
    std::string str() const {
        return ss.str();
    }
private:
    std::streambuf* oldBuffer;
    std::stringstream ss;
};

// Test classifyTemperatureBreach for different cooling types and temperatures
TEST(TypewiseAlertTest, TestClassifyTemperatureBreach) {
    using namespace TypewiseAlert;

    // Test PASSIVE_COOLING
    {
        CoolingLimits limits = getLimitsForCoolingType(CoolingType::PASSIVE_COOLING);
        EXPECT_EQ(limits.lowerLimit, 0);
        EXPECT_EQ(limits.upperLimit, 35);

        EXPECT_EQ(classifyTemperatureBreach(CoolingType::PASSIVE_COOLING, 20), BreachType::NORMAL);
        EXPECT_EQ(classifyTemperatureBreach(CoolingType::PASSIVE_COOLING, -5), BreachType::TOO_LOW);
        EXPECT_EQ(classifyTemperatureBreach(CoolingType::PASSIVE_COOLING, 40), BreachType::TOO_HIGH);
    }

    // Test HI_ACTIVE_COOLING
    {
        CoolingLimits limits = getLimitsForCoolingType(CoolingType::HI_ACTIVE_COOLING);
        EXPECT_EQ(limits.lowerLimit, 0);
        EXPECT_EQ(limits.upperLimit, 45);

        EXPECT_EQ(classifyTemperatureBreach(CoolingType::HI_ACTIVE_COOLING, 25), BreachType::NORMAL);
        EXPECT_EQ(classifyTemperatureBreach(CoolingType::HI_ACTIVE_COOLING, -10), BreachType::TOO_LOW);
        EXPECT_EQ(classifyTemperatureBreach(CoolingType::HI_ACTIVE_COOLING, 50), BreachType::TOO_HIGH);
    }

    // Test MED_ACTIVE_COOLING
    {
        CoolingLimits limits = getLimitsForCoolingType(CoolingType::MED_ACTIVE_COOLING);
        EXPECT_EQ(limits.lowerLimit, 0);
        EXPECT_EQ(limits.upperLimit, 40);

        EXPECT_EQ(classifyTemperatureBreach(CoolingType::MED_ACTIVE_COOLING, 30), BreachType::NORMAL);
        EXPECT_EQ(classifyTemperatureBreach(CoolingType::MED_ACTIVE_COOLING, -5), BreachType::TOO_LOW);
        EXPECT_EQ(classifyTemperatureBreach(CoolingType::MED_ACTIVE_COOLING, 45), BreachType::TOO_HIGH);
    }
}

// Test sending alerts
TEST(TypewiseAlertTest, TestSendAlerts) {
    using namespace TypewiseAlert;

    OutputCapture capture;

    // Test sendToController
    sendToController(BreachType::TOO_LOW);
    EXPECT_EQ(capture.str(), "feed : 1\n");
    capture = OutputCapture(); // Reset capture

    sendToController(BreachType::TOO_HIGH);
    EXPECT_EQ(capture.str(), "feed : 2\n");
    capture = OutputCapture(); // Reset capture

    sendToController(BreachType::NORMAL);
    EXPECT_EQ(capture.str(), "feed : 0\n");
    capture = OutputCapture(); // Reset capture

    // Test sendToEmail
    sendToEmail(BreachType::TOO_LOW);
    EXPECT_EQ(capture.str(), "To: a.b@c.com\nHi, the temperature is too low\n");
    capture = OutputCapture(); // Reset capture

    sendToEmail(BreachType::TOO_HIGH);
    EXPECT_EQ(capture.str(), "To: a.b@c.com\nHi, the temperature is too high\n");
    capture = OutputCapture(); // Reset capture

    sendToEmail(BreachType::NORMAL);
    EXPECT_EQ(capture.str(), "");
}

// Test checkAndAlert method
TEST(TypewiseAlertTest, TestCheckAndAlert) {
    using namespace TypewiseAlert;

    // Define battery characteristics
    BatteryCharacter batteryChar = {CoolingType::PASSIVE_COOLING, "BrandX"};

    // Capture output for different alert targets
    OutputCapture capture;

    // Test alert to controller
    checkAndAlert(AlertTarget::TO_CONTROLLER, batteryChar, 20);
    EXPECT_EQ(capture.str(), "feed : 0\n");
    capture = OutputCapture(); // Reset capture

    // Test alert to email
    checkAndAlert(AlertTarget::TO_EMAIL, batteryChar, 20);
    EXPECT_EQ(capture.str(), "");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
