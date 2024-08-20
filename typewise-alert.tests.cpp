#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "typewise-alert.h"

class MockTypewiseAlert : public TypewiseAlert {
public:
    MOCK_METHOD(void, sendToController, (BreachType breachType), (override));
    MOCK_METHOD(void, sendToEmail, (BreachType breachType), (override));
};

TEST(TypewiseAlertTest, ClassifyTemperatureBreach) {
    EXPECT_EQ(TypewiseAlert::classifyTemperatureBreach(TypewiseAlert::CoolingType::PASSIVE_COOLING, 25), TypewiseAlert::BreachType::NORMAL);
    EXPECT_EQ(TypewiseAlert::classifyTemperatureBreach(TypewiseAlert::CoolingType::PASSIVE_COOLING, 15), TypewiseAlert::BreachType::TOO_LOW);
    EXPECT_EQ(TypewiseAlert::classifyTemperatureBreach(TypewiseAlert::CoolingType::PASSIVE_COOLING, 35), TypewiseAlert::BreachType::TOO_HIGH);

    EXPECT_EQ(TypewiseAlert::classifyTemperatureBreach(TypewiseAlert::CoolingType::HI_ACTIVE_COOLING, 40), TypewiseAlert::BreachType::NORMAL);
    EXPECT_EQ(TypewiseAlert::classifyTemperatureBreach(TypewiseAlert::CoolingType::HI_ACTIVE_COOLING, 50), TypewiseAlert::BreachType::TOO_HIGH);

    EXPECT_EQ(TypewiseAlert::classifyTemperatureBreach(TypewiseAlert::CoolingType::MED_ACTIVE_COOLING, 20), TypewiseAlert::BreachType::NORMAL);
    EXPECT_EQ(TypewiseAlert::classifyTemperatureBreach(TypewiseAlert::CoolingType::MED_ACTIVE_COOLING, -5), TypewiseAlert::BreachType::TOO_LOW);
}

TEST(TypewiseAlertTest, CheckAndAlertToController) {
    MockTypewiseAlert mockAlert;

    EXPECT_CALL(mockAlert, sendToController(TypewiseAlert::BreachType::TOO_HIGH))
        .Times(1);

    TypewiseAlert::BatteryCharacter batteryChar = { TypewiseAlert::CoolingType::PASSIVE_COOLING, "BrandA" };
    mockAlert.checkAndAlert(TypewiseAlert::AlertTarget::TO_CONTROLLER, batteryChar, 50);
}

TEST(TypewiseAlertTest, CheckAndAlertToEmail) {
    MockTypewiseAlert mockAlert;

    EXPECT_CALL(mockAlert, sendToEmail(TypewiseAlert::BreachType::TOO_HIGH))
        .Times(1);

    TypewiseAlert::BatteryCharacter batteryChar = { TypewiseAlert::CoolingType::PASSIVE_COOLING, "BrandA" };
    mockAlert.checkAndAlert(TypewiseAlert::AlertTarget::TO_EMAIL, batteryChar, 50);
}

TEST(TypewiseAlertTest, NoAlertForNormalTemperature) {
    MockTypewiseAlert mockAlert;

    EXPECT_CALL(mockAlert, sendToEmail(::testing::_)).Times(0);

    TypewiseAlert::BatteryCharacter batteryChar = { TypewiseAlert::CoolingType::HI_ACTIVE_COOLING, "BrandB" };
    mockAlert.checkAndAlert(TypewiseAlert::AlertTarget::TO_EMAIL, batteryChar, 40);
}
