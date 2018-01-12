/**
 * (c)2017 CDP Technologies AS
 */

#ifndef GREENHOUSELIB_GREENHOUSECONTROLLER_H
#define GREENHOUSELIB_GREENHOUSECONTROLLER_H

#include <CDPSystem/Base/CDPComponent.h>
#include <Signal/CDPSignal.h>
#include <CDPParameter/CDPParameter.h>
#include <CDPAlarm/CDPAlarm.h>
#include <CDPSystem/Base/CDPProperty.h>

namespace GreenhouseLib {

class GreenhouseController : public CDPComponent
{
public:
    GreenhouseController();

    void Create(const char* fullName) override;
    void CreateModel() override;
    void Configure(const char* componentXML) override;
    void ProcessNull() override;

private:
    void MonitorInsideTemp();
    void MonitorSoilMoisture();
    bool IsMaxInsideTempGreaterThanMin();
    void RegulateIrrigation(double currLocalTime);
    bool CanIrrigateToMax(int startTime, int endTime, double currTime) const;
    void Irrigate(double currLocalTime);

    CDPSignal<double> CurrInsideTemp;
    CDPSignal<bool> Ventilating;
    CDPSignal<double> CurrOutsideTemp;
    CDPSignal<double> CurrSoilMoisture;
    CDPSignal<bool> Irrigating;
    CDPSignal<double> CurrTime;
    CDPSignal<double> AllowedInsideTempRange;
    CDPSignal<double> AllowedInsideTempRangeCenter;
    CDPSignal<double> AllowedMoistureRange;
    CDPSignal<double> AllowedMoistureRangeCenter;
    CDPProperty<double> MinAllowedInsideTemp;
    CDPProperty<double> MaxAllowedInsideTemp;
    CDPProperty<double> MaxAllowedSoilMoisture;
    CDPProperty<double> MinAllowedSoilMoisture;
    CDPProperty<int> IrrigationStartHour;
    CDPProperty<int> IrrigationEndHour;
    CDPProperty<int> CycleDuration;
    CDPProperty<int> WaterAbsorptionTime;
    CDPSignal<bool> MaxIrrigationIsAllowed;
    using CDPComponent::requestedState;
    using CDPComponent::ts;
    using CDPComponent::fs;

    double IrrigationStartTime;
    double TimeDiff;
};

}

#endif
