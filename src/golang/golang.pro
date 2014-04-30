include(../plugin.pri)

DEFINES += GO_LIBRARY

# Go files

SOURCES += \
    golangplugin.cpp \
    goproject.cpp \
    goprojectfile.cpp \
    goprojectmanager.cpp \
    goprojectnode.cpp \
    filefilteritems.cpp \
    goprojectfileformat.cpp \
    goprojectitem.cpp \
    toolchain.cpp \
    toolchainmanager.cpp \
    toolchainoptionspage.cpp \
    toolchainconfigwidget.cpp \
    gotoolchain.cpp \
    gobuildconfiguration.cpp \
    gokitinformation.cpp

HEADERS += \
    golangplugin.h \
    golangconstants.h \
    golang_global.h \
    goproject.h \
    goprojectfile.h \
    goprojectmanager.h \
    goprojectnode.h \
    filefilteritems.h \
    goprojectfileformat.h \
    goprojectitem.h \
    toolchain.h \
    toolchainmanager.h \
    toolchainoptionspage.h \
    toolchainconfigwidget.h \
    gotoolchain.h \
    gobuildconfiguration.h \
    gokitinformation.h

OTHER_FILES +=
    GoLang.pluginspec.in

RESOURCES += \
    golangplugin.qrc
