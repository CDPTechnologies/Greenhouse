#ifndef GREENHOUSELIB_TIMEPROVIDER_H
#define GREENHOUSELIB_TIMEPROVIDER_H

#include <CDPSystem/Base/CDPComponent.h>
#include <Signal/CDPSignal.h>
#include <CDPParameter/CDPParameter.h>
#include <CDPAlarm/CDPAlarm.h>

namespace GreenhouseLib {

class TimeProvider : public CDPComponent
{
public:
    TimeProvider();

    void Create(const char* fullName) override;
    void CreateModel() override;
    void Configure(const char* componentXML) override;
    void ProcessNull() override;

private:
    CDPSignal<double> CurrTime;
    using CDPComponent::requestedState;
    using CDPComponent::ts;
    using CDPComponent::fs;

private:

};

}

#endif
