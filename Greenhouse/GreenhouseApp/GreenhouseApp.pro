TYPE = application
PROJECTNAME = GreenhouseApp

DEPS += cdpeventmanager greenhouselib i2cio automation gpiopinio cdplogger

HEADERS += Libraries.h
SOURCES += CDPMain.cpp

DISTFILES += \
    $$files(*.xml, true) \
    $$files(*.lic, true) \
    $$files(Application/www/*.*, true)

load(cdp)
