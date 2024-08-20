#include "typewise_alert.h"
#include <stdio.h>

// Initialize the static array of CoolingLimits inside the class
const TypewiseAlert::CoolingLimits* TypewiseAlert::getCoolingLimits() {
    static const CoolingLimits coolingLimits[] = {
        {CoolingType::PASSIVE_COOLING, 0, 35},
        {CoolingType::HI_ACTIVE_COOLING, 0, 45},
        {CoolingType::MED_ACTIVE_COOLING, 0, 40},
    };
    return coolingLimits;
}

TypewiseAlert::CoolingLimits TypewiseAlert::getLimitsForCoolingType(CoolingType coolingType) {
    const CoolingLimits* limits = getCoolingLimits();
    for (int i = 0; i < 3; ++i) {
        if (limits[i].coolingType == coolingType) {
            return limits[i];
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
