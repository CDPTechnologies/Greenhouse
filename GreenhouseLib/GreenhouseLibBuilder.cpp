/**
GreenhouseLibBuilder implementation.
*/

#include "GreenhouseController.h"
#include "SimulationEnvironment.h"
#include "TimeProvider.h"
#include "TimeProvider.h"
#include "GreenhouseLibBuilder.h"

using namespace GreenhouseLib;

GreenhouseLibBuilder::GreenhouseLibBuilder(const char* libName, const char* timeStamp)
    : CDPBuilder(libName, timeStamp)
{
}

CDPComponent* GreenhouseLibBuilder::CreateNewComponent(const std::string& type)
{
    if (type=="GreenhouseLib.GreenhouseController")
        return new GreenhouseController;

    if (type=="GreenhouseLib.SimulationEnvironment")
        return new SimulationEnvironment;
    
    if (type=="GreenhouseLib.TimeProvider")
        return new TimeProvider;
    
    return CDPBuilder::CreateNewComponent(type);
}

CDPBaseObject* GreenhouseLibBuilder::CreateNewCDPOperator(const std::string& modelName, const std::string& type, const CDPPropertyBase* inputProperty)
{
    return CDPBuilder::CreateNewCDPOperator(modelName, type, inputProperty);
}
