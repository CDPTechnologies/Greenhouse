CDPVERSION = 4.4
TYPE = library
PROJECTNAME = GreenhouseLib

DEPS += \

HEADERS += \
    greenhouselib.h \
    GreenhouseLibBuilder.h \
    GreenhouseController.h \
    SimulationEnvironment.h \
    TimeProvider.h

SOURCES += \
    GreenhouseLibBuilder.cpp \
    GreenhouseController.cpp \
    SimulationEnvironment.cpp \
    TimeProvider.cpp

DISTFILES += $$files(*.xml, true) \
    Templates/Models/GreenhouseLib.GreenhouseController.xml \
    Templates/Models/GreenhouseLib.SimulationEnvironment.xml \
    Templates/Models/GreenhouseLib.TimeProvider.xml

load(cdp)
