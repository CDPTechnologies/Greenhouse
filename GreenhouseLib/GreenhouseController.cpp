#include "GreenhouseController.h"
#include <OSAPI/Timer/CDPTime.h>
#include <cmath>
#include <ctime>

using namespace GreenhouseLib;

const unsigned int SEC_IN_AN_HOUR = 60 * 60;
const size_t SEC_IN_A_DAY = 24 * SEC_IN_AN_HOUR;

GreenhouseController::GreenhouseController()
    : IrrigationStartTime(0)
{
    std::time_t time = std::time(nullptr);
    int UTCHour = std::gmtime(&time)->tm_hour;
    int localHour = std::localtime(&time)->tm_hour;
    TimeDiff = (localHour - UTCHour) * SEC_IN_AN_HOUR;
}

void GreenhouseController::Create(const char* fullName)
{
    CDPComponent::Create(fullName);
    CurrInsideTemp.Create("CurrInsideTemp",this);
    CurrOutsideTemp.Create("CurrOutsideTemp",this);
    CurrSoilMoisture.Create("CurrSoilMoisture",this);
    CurrTime.Create("CurrTime",this);
    Ventilating.Create("Ventilating",this);
    Irrigating.Create("Irrigating",this);
    AllowedInsideTempRange.Create("AllowedInsideTempRange",this);
    AllowedInsideTempRangeCenter.Create("AllowedInsideTempRangeCenter",this);
    AllowedMoistureRange.Create("AllowedMoistureRange",this);
    AllowedMoistureRangeCenter.Create("AllowedMoistureRangeCenter",this);
    MinAllowedInsideTemp.Create("MinAllowedInsideTemp",this,CDPPropertyBase::e_Element,(CDPOBJECT_SETPROPERTY_HANDLER)nullptr,(CDPOBJECT_VALIDATEPROPERTY_HANDLER)nullptr);
    MinAllowedInsideTemp.SetPropertyChangeHandler([&] (CDPPropertyBase*) {
        if (MinAllowedInsideTemp > static_cast<double>(MaxAllowedInsideTemp))
            MaxAllowedInsideTemp = MinAllowedInsideTemp;
    });
    MaxAllowedInsideTemp.Create("MaxAllowedInsideTemp",this,CDPPropertyBase::e_Element,(CDPOBJECT_SETPROPERTY_HANDLER)nullptr,(CDPOBJECT_VALIDATEPROPERTY_HANDLER)nullptr);
    MaxAllowedInsideTemp.SetPropertyChangeHandler([&] (CDPPropertyBase*) {
        if (MaxAllowedInsideTemp < static_cast<double>(MinAllowedInsideTemp))
            MinAllowedInsideTemp = MaxAllowedInsideTemp;
    });
    MaxAllowedSoilMoisture.Create("MaxAllowedSoilMoisture",this,CDPPropertyBase::e_Element,(CDPOBJECT_SETPROPERTY_HANDLER)nullptr,(CDPOBJECT_VALIDATEPROPERTY_HANDLER)nullptr);
    MaxAllowedSoilMoisture.SetPropertyChangeHandler([&] (CDPPropertyBase*) {
        if (MaxAllowedSoilMoisture < static_cast<double>(MinAllowedSoilMoisture))
            MinAllowedSoilMoisture = MaxAllowedSoilMoisture;
    });
    MinAllowedSoilMoisture.Create("MinAllowedSoilMoisture",this,CDPPropertyBase::e_Element,(CDPOBJECT_SETPROPERTY_HANDLER)nullptr,(CDPOBJECT_VALIDATEPROPERTY_HANDLER)nullptr);
    MinAllowedSoilMoisture.SetPropertyChangeHandler([&] (CDPPropertyBase*) {
        if (MinAllowedSoilMoisture > static_cast<double>(MaxAllowedSoilMoisture))
            MaxAllowedSoilMoisture = MinAllowedSoilMoisture;
    });
    IrrigationStartHour.Create("IrrigationStartHour",this,CDPPropertyBase::e_Element,(CDPOBJECT_SETPROPERTY_HANDLER)nullptr,(CDPOBJECT_VALIDATEPROPERTY_HANDLER)nullptr);
    IrrigationEndHour.Create("IrrigationEndHour",this,CDPPropertyBase::e_Element,(CDPOBJECT_SETPROPERTY_HANDLER)nullptr,(CDPOBJECT_VALIDATEPROPERTY_HANDLER)nullptr);
    CycleDuration.Create("CycleDuration",this,CDPPropertyBase::e_Element,(CDPOBJECT_SETPROPERTY_HANDLER)nullptr,(CDPOBJECT_VALIDATEPROPERTY_HANDLER)nullptr);
    WaterAbsorptionTime.Create("WaterAbsorptionTime",this,CDPPropertyBase::e_Element,(CDPOBJECT_SETPROPERTY_HANDLER)nullptr,(CDPOBJECT_VALIDATEPROPERTY_HANDLER)nullptr);
    MaxIrrigationIsAllowed.Create("MaxIrrigationIsAllowed",this);
}

void GreenhouseController::CreateModel()
{
    CDPComponent::CreateModel();

    RegisterStateProcess("Null", (CDPCOMPONENT_STATEPROCESS)&GreenhouseController::ProcessNull, "Initial Null state");
}

void GreenhouseController::Configure(const char* componentXML)
{
    CDPComponent::Configure(componentXML);
}

void GreenhouseController::ProcessNull()
{
    MonitorInsideTemp();
    MonitorSoilMoisture();
}

void GreenhouseController::MonitorInsideTemp()
{
    if (CurrInsideTemp > MaxAllowedInsideTemp)
        Ventilating = 1;
    else if (CurrInsideTemp < MinAllowedInsideTemp)
        Ventilating = 0;

    AllowedInsideTempRange = MaxAllowedInsideTemp - MinAllowedInsideTemp;
    AllowedInsideTempRangeCenter = MinAllowedInsideTemp + (AllowedInsideTempRange/2);
}

void GreenhouseController::MonitorSoilMoisture()
{
    if (CurrSoilMoisture >= MaxAllowedSoilMoisture)
    {
        Irrigating = false;
        IrrigationStartTime = 0;
        MaxIrrigationIsAllowed = false;
    }
    else if (CurrSoilMoisture <= MinAllowedSoilMoisture || MaxIrrigationIsAllowed)
        RegulateIrrigation(CurrTime + TimeDiff);

    AllowedMoistureRange = MaxAllowedSoilMoisture - MinAllowedSoilMoisture;
    AllowedMoistureRangeCenter = MinAllowedSoilMoisture + (AllowedMoistureRange/2);
}

void GreenhouseController::RegulateIrrigation(double currLocalTime)
{
    if (CanIrrigateToMax(IrrigationStartHour, IrrigationEndHour, currLocalTime))
        MaxIrrigationIsAllowed = true;
    else
        MaxIrrigationIsAllowed = false;

    Irrigate(currLocalTime);
}

bool GreenhouseController::CanIrrigateToMax(int startHour, int endHour, double currLocalTime) const
{
    bool start = false;
    double timeInHours = fmod(currLocalTime, SEC_IN_A_DAY) / (SEC_IN_AN_HOUR);

    if (endHour <= startHour)
    {
        if (timeInHours <= endHour && timeInHours <= startHour)
            start = true;
        else if (timeInHours >= endHour && timeInHours >= startHour)
            start = true;
    }
    else if (startHour <= timeInHours && timeInHours < endHour)
        start = true;

    return start;
}

void GreenhouseController::Irrigate(double currLocalTime)
{
    if (!IrrigationStartTime)
        IrrigationStartTime = currLocalTime;

    if (currLocalTime - IrrigationStartTime < CycleDuration)
        Irrigating = true;
    else if (currLocalTime - IrrigationStartTime < CycleDuration + WaterAbsorptionTime)
        Irrigating = false;
    else
        IrrigationStartTime = 0;
}
