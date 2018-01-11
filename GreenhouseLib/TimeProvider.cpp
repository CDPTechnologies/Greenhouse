#include "TimeProvider.h"
#include <OSAPI/Timer/CDPTime.h>

using namespace GreenhouseLib;

TimeProvider::TimeProvider()
{
}

void TimeProvider::Create(const char* fullName)
{
    CDPComponent::Create(fullName);
    CurrTime.Create("CurrTime",this);
}

void TimeProvider::CreateModel()
{
    CDPComponent::CreateModel();

    RegisterStateProcess("Null", (CDPCOMPONENT_STATEPROCESS)&TimeProvider::ProcessNull, "Initial Null state");
}

void TimeProvider::Configure(const char* componentXML)
{
    CDPComponent::Configure(componentXML);
}

void TimeProvider::ProcessNull()
{
     CurrTime = CDPTime::GlobalClock();
}
