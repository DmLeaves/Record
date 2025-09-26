QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += \
    $$PWD/src \
    $$PWD/src/core \
    $$PWD/src/ui \
    $$PWD/src/ui/widgets

SOURCES += \
    src/app/main.cpp \
    src/core/cyclebox.cpp \
    src/core/dataoper.cpp \
    src/core/userdata.cpp \
    src/core/userdataparser.cpp \
    src/ui/getfile.cpp \
    src/ui/mainwindow.cpp \
    src/ui/managewidget.cpp \
    src/ui/messagewindows.cpp \
    src/ui/newinput.cpp \
    src/ui/rollwidget.cpp \
    src/ui/widgets/animatedlabel.cpp \
    src/ui/widgets/changlabel.cpp \
    src/ui/widgets/signalanimalabel.cpp

HEADERS += \
    src/core/cyclebox.h \
    src/core/data.h \
    src/core/dataoper.h \
    src/core/userdata.h \
    src/core/userdataparser.h \
    src/ui/getfile.h \
    src/ui/mainwindow.h \
    src/ui/managewidget.h \
    src/ui/messagewindows.h \
    src/ui/newinput.h \
    src/ui/rollwidget.h \
    src/ui/widgets/animatedlabel.h \
    src/ui/widgets/changlabel.h \
    src/ui/widgets/signalanimalabel.h

FORMS += \
    forms/mainwindow.ui \
    forms/managewidget.ui \
    forms/SampleInput.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    source/source.qrc

RC_ICONS += ./logo.ico
