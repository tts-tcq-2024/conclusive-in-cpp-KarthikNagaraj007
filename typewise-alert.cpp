#include "typewise_alert.h"
#include <stdio.h>

const TypewiseAlert::CoolingLimits coolingLimits[] = {
    {TypewiseAlert::CoolingType::PASSIVE_COOLING, 0, 35},
    {TypewiseAlert::CoolingType::HI_ACTIVE_COOLING, 0, 45},
    {TypewiseAlert::CoolingType::MED_ACTIVE_COOLING, 0, 40},
};

TypewiseAlert::CoolingLimits TypewiseAlert::getLimitsForCoolingType(CoolingType coolingType) {
    for (const auto& limits : coolingLimits) {
        if (limits.coolingType == coolingType) {
            return limits;
        }
    }
    // Default case: return 0, 0 limits if cooling type is unknown
    return {coolingType, 0, 0};
}

TypewiseAlert::BreachType TypewiseAlert::inferBreach(double value, double lowerLimit, double upperLimit) {
    if (value < lowerLimit) {
        return BreachType::TOO_LOW;
    }
    if (value > upperLimit) {
        return BreachType::TOO_HIGH;
    }
    return BreachType::NORMAL;
}

TypewiseAlert::BreachType TypewiseAlert::classifyTemperatureBreach(CoolingType coolingType, double temperatureInC) {
    CoolingLimits limits = getLimitsForCoolingType(coolingType);
    return inferBreach(temperatureInC, limits.lowerLimit, limits.upperLimit);
}

void TypewiseAlert::sendToController(BreachType breachType) {
    const unsigned short header = 0xfeed;
    printf("%x : %x\n", header, static_cast<int>(breachType));
}

void TypewiseAlert::sendToEmail(BreachType breachType) {
    const char* recipient = "a.b@c.com";
    if (breachType != BreachType::NORMAL) {
        printf("To: %s\n", recipient);
        printf("Hi, the temperature is %s\n", breachType == BreachType::TOO_LOW ? "too low" : "too high");
    }
}

void TypewiseAlert::checkAndAlert(AlertTarget alertTarget, const BatteryCharacter& batteryChar, double temperatureInC) {
    BreachType breachType = classifyTemperatureBreach(batteryChar.coolingType, temperatureInC);

    if (alertTarget == AlertTarget::TO_CONTROLLER) {
        sendToController(breachType);
    } else {
        sendToEmail(breachType);
    }
}
