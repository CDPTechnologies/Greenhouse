/**
 * (c)2017 CDP Technologies AS
 */

#ifndef GREENHOUSELIB_SIMULATIONENVIRONMENT_H
#define GREENHOUSELIB_SIMULATIONENVIRONMENT_H

#include <CDPSystem/Base/CDPComponent.h>
#include <Signal/CDPSignal.h>
#include <CDPParameter/CDPParameter.h>
#include <CDPAlarm/CDPAlarm.h>
#include <CDPSystem/Base/CDPProperty.h>

namespace GreenhouseLib {

class SimulationEnvironment : public CDPComponent
{
public:
    SimulationEnvironment();

    void Create(const char* fullName) override;
    void CreateModel() override;
    void Configure(const char* componentXML) override;
    void ProcessNull() override;

private:
    double SimulateOutsideTemp(double time) const;
    double SimulateInsideTemp(double previousTime, double time);
    double SimulateSoilMoisture(double previousTime, double time);
    double VentilationTempDecreaseInTimeunit(double previousTime, double time);
    double ApproximateInsideTempChangeInTimeunit(double previousTime, double time) const;
    double CheckSoilMoistureBounds(double SoilMoisture) const;
    void UpdateSimTime();

    CDPSignal<bool> Enable;
    CDPSignal<double> CurrInsideTemp;
    CDPSignal<bool> Irrigating;
    CDPSignal<bool> Ventilation;
    CDPSignal<double> CurrOutsideTemp;
    CDPSignal<double> CurrSoilMoisture;
    CDPSignal<double> CurrSimUTCTime;
    CDPProperty<int> SimSpeedMultiplier;
    using CDPComponent::requestedState;
    using CDPComponent::ts;
    using CDPComponent::fs;

    int OutTempAmplitudeMultiplier;
    double MonthlyOutsideTempAvg;
    double SimSpeedModifier;
    double InsideTemp;
    double VentilationTempDecrease;
    double PreviousTime;
    double IrrigationDuration;
    double IrrigationDelay;
    bool StartSimIrrigationCycle;
    double TimeDiff;
};

}

#endif
