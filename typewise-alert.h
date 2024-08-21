#pragma once
#include <string>

class TypewiseAlert {
public:
    enum class CoolingType {
        PASSIVE_COOLING,
        HI_ACTIVE_COOLING,
        MED_ACTIVE_COOLING
    };

    enum class BreachType {
        NORMAL,
        TOO_LOW,
        TOO_HIGH
    };

    enum class AlertTarget {
        TO_CONTROLLER,
        TO_EMAIL
    };

    struct BatteryCharacter {
        CoolingType coolingType;
        std::string brand;
    };

    static BreachType classifyTemperatureBreach(CoolingType coolingType, double temperatureInC);
    static void checkAndAlert(AlertTarget alertTarget, const BatteryCharacter& batteryChar, double temperatureInC);
    static BreachType inferBreach(double value, double lowerLimit, double upperLimit);

private:
    struct CoolingLimits {
        CoolingType coolingType;
        int lowerLimit;
        int upperLimit;
    };
    
    static CoolingLimits getLimitsForCoolingType(CoolingType coolingType);
    static void sendToController(BreachType breachType);
    static void sendToEmail(BreachType breachType);

    // Static method to initialize the cooling limits
    static const CoolingLimits* getCoolingLimits();
};
