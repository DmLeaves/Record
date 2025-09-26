QT       += core gui widgets sql

CONFIG += c++17

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x050F00

SOURCES += \
    src/app/main.cpp \
    src/core/database_manager.cpp \
    src/core/record_repository.cpp \
    src/core/master_password_service.cpp \
    src/core/import_service.cpp \
    src/ui/main_window.cpp \
    src/ui/unlock_widget.cpp \
    src/ui/record_page.cpp \
    src/ui/record_list_model.cpp \
    src/ui/record_editor_dialog.cpp \
    src/ui/import_dialog.cpp \
    src/ui/tray_manager.cpp \
    src/ui/notifications.cpp

HEADERS += \
    src/core/database_manager.h \
    src/core/record.h \
    src/core/record_repository.h \
    src/core/master_password_service.h \
    src/core/import_service.h \
    src/ui/main_window.h \
    src/ui/unlock_widget.h \
    src/ui/record_page.h \
    src/ui/record_list_model.h \
    src/ui/record_editor_dialog.h \
    src/ui/import_dialog.h \
    src/ui/tray_manager.h \
    src/ui/notifications.h

DISTFILES += README.md LICENSE

RESOURCES += source/source.qrc

RC_ICONS += ./logo.ico

# Deployment rules
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
