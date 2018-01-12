#include "SimulationEnvironment.h"
#include <cmath>
#include <ctime>

using namespace GreenhouseLib;

const int SEC_IN_AN_HOUR = 60 * 60;
const int SEC_IN_A_DAY = 24 * SEC_IN_AN_HOUR;
const int MAX_SIM_SPEED_MULTIPLIER = 5000;
const double VENTILATION_TEMP_DECREASE_IN_SEC = 0.0012;
const double SIN_FREQUENCY = 2 * M_PI / SEC_IN_A_DAY;
const double SOIL_MOISTURE_DECREASE_IN_SEC = 0.00083;
const double SOIL_MOISTURE_INCREASE_IN_SEC = 0.031;
const short int SOIL_ABSORPTION_DELAY_IN_SEC = 10;
const double INSIDE_TEMP_CHANGE_MULTIPLIER = 1.8;

SimulationEnvironment::SimulationEnvironment()
    : OutTempAmplitudeMultiplier(10)
    , MonthlyOutsideTempAvg(16.5)
    , InsideTemp(SimulateOutsideTemp(0))
    , VentilationTempDecrease(0)
    , PreviousTime(0)
    , IrrigationDuration(0)
    , IrrigationDelay(0)
    , StartSimIrrigationCycle(false)
{
    std::time_t time = std::time(nullptr);
    int UTCHour = std::gmtime(&time)->tm_hour;
    int localHour = std::localtime(&time)->tm_hour;
    TimeDiff = (localHour - UTCHour) * SEC_IN_AN_HOUR;
}

void SimulationEnvironment::Create(const char* fullName)
{
    CDPComponent::Create(fullName);
    Enable.Create("Enable",this);
    CurrInsideTemp.Create("CurrInsideTemp",this);
    Irrigating.Create("Irrigating",this);
    Ventilation.Create("Ventilation", this);
    CurrOutsideTemp.Create("CurrOutsideTemp",this);
    CurrSoilMoisture.Create("CurrSoilMoisture",this);
    CurrSimUTCTime.Create("CurrSimUTCTime",this);
    SimSpeedMultiplier.Create("SimSpeedMultiplier",this,CDPPropertyBase::e_Element,(CDPOBJECT_SETPROPERTY_HANDLER)nullptr,(CDPOBJECT_VALIDATEPROPERTY_HANDLER)nullptr);
    SimSpeedMultiplier.SetPropertyChangeHandler([&] (CDPPropertyBase*) {
        if (SimSpeedMultiplier > MAX_SIM_SPEED_MULTIPLIER) {
            CDPMessage("Sim speed multiplier cannot be greater than %d\n", MAX_SIM_SPEED_MULTIPLIER);
            SimSpeedMultiplier = MAX_SIM_SPEED_MULTIPLIER;
        }
    });
}

void SimulationEnvironment::CreateModel()
{
    CDPComponent::CreateModel();
    RegisterStateProcess("Null",(CDPCOMPONENT_STATEPROCESS)&SimulationEnvironment::ProcessNull,"Initial Null state");
}

void SimulationEnvironment::Configure(const char* componentXML)
{
    CDPComponent::Configure(componentXML);
    CurrSimUTCTime = PreviousTime;
}

void SimulationEnvironment::ProcessNull()
{
    double previousLocalTime = PreviousTime + TimeDiff;
    double currLocalTime = CurrSimUTCTime + TimeDiff;

    CurrOutsideTemp = SimulateOutsideTemp(currLocalTime);
    CurrInsideTemp = SimulateInsideTemp(previousLocalTime, currLocalTime);
    CurrSoilMoisture = SimulateSoilMoisture(previousLocalTime, currLocalTime);
    UpdateSimTime();
}

double SimulationEnvironment::SimulateOutsideTemp(double time) const
{
    return OutTempAmplitudeMultiplier * sin(SIN_FREQUENCY * time) + MonthlyOutsideTempAvg;
}

double SimulationEnvironment::SimulateInsideTemp(double previousTime, double time)
{
    InsideTemp += ApproximateInsideTempChangeInTimeunit(previousTime, time) - VentilationTempDecreaseInTimeunit(previousTime, time);
    InsideTemp = InsideTemp < CurrOutsideTemp ? static_cast<double>(CurrOutsideTemp) : InsideTemp;

    return InsideTemp;
}

double SimulationEnvironment::ApproximateInsideTempChangeInTimeunit(double previousTime, double time) const
{
    return INSIDE_TEMP_CHANGE_MULTIPLIER * (SimulateOutsideTemp(time) - SimulateOutsideTemp(previousTime));
}

double SimulationEnvironment::VentilationTempDecreaseInTimeunit(double previousTime, double time)
{
    double vent = 0;

    if (Ventilation)
        vent = VENTILATION_TEMP_DECREASE_IN_SEC * (time - previousTime);

    return vent;
}

double SimulationEnvironment::SimulateSoilMoisture(double previousTime, double time)
{
    double soilMoisture = CurrSoilMoisture;
    double timeDiff = time - previousTime;

    soilMoisture -= SOIL_MOISTURE_DECREASE_IN_SEC * timeDiff;

    if (Irrigating)
    {
        IrrigationDuration += 1 * timeDiff;
        if (!StartSimIrrigationCycle)
        {
            IrrigationDelay = SOIL_ABSORPTION_DELAY_IN_SEC * timeDiff;
            StartSimIrrigationCycle = true;
        }
    }

    if (StartSimIrrigationCycle)
    {
        IrrigationDelay -= 1 * timeDiff;
        if (IrrigationDelay <= 0)
        {
             IrrigationDelay = 0;
             IrrigationDuration -= 1 * timeDiff;
             soilMoisture += SOIL_MOISTURE_INCREASE_IN_SEC * timeDiff;
         }

        if (IrrigationDuration <= 0)
        {
            IrrigationDuration = 0;
            StartSimIrrigationCycle = false;
        }
    }
    soilMoisture = CheckSoilMoistureBounds(soilMoisture);

    return soilMoisture;
}

double SimulationEnvironment::CheckSoilMoistureBounds(double SoilMoisture) const
{
    if (SoilMoisture < 0)
        SoilMoisture = 0;
    else if (SoilMoisture > 100)
        SoilMoisture = 100;

    return SoilMoisture;
}

void SimulationEnvironment::UpdateSimTime()
{
    double time = PreviousTime = CurrSimUTCTime;

    time += (1 / GetFrequency()) * SimSpeedMultiplier;

    CurrSimUTCTime = time;
}

