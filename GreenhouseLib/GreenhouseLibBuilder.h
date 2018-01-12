/**
 * (c)2017 CDP Technologies AS
 */

#ifndef GREENHOUSELIB_GREENHOUSELIBBUILDER_H
#define GREENHOUSELIB_GREENHOUSELIBBUILDER_H

#include <CDPSystem/Application/CDPBuilder.h>

namespace GreenhouseLib {

class GreenhouseLibBuilder : public CDPBuilder
{
public:
    GreenhouseLibBuilder(const char* libName,const char* timeStamp);
    CDPComponent* CreateNewComponent(const std::string& type) override;
    CDPBaseObject* CreateNewCDPOperator(const std::string& modelName,const std::string& type,const CDPPropertyBase* inputProperty) override;
};

}

#endif
