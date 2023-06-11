QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    animatedlabel.cpp \
    changlabel.cpp \
    cyclebox.cpp \
    dataoper.cpp \
    getfile.cpp \
    main.cpp \
    mainwindow.cpp \
    managewidget.cpp \
    messagewindows.cpp \
    newinput.cpp \
    rollwidget.cpp \
    signalanimalabel.cpp \
    userdata.cpp \
    userdataparser.cpp

HEADERS += \
    animatedlabel.h \
    changlabel.h \
    cyclebox.h \
    data.h \
    dataoper.h \
    getfile.h \
    mainwindow.h \
    managewidget.h \
    messagewindows.h \
    newinput.h \
    rollwidget.h \
    signalanimalabel.h \
    userdata.h \
    userdataparser.h

FORMS += \
    mainwindow.ui \
    managewidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    source/source.qrc

RC_ICONS += ./logo.ico
