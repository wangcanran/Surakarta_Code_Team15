QT       += core gui
#network
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    aithread.cpp \
    chess.cpp \
    choose.cpp \
    client.cpp \
    main.cpp \
    mainwindow.cpp \
    networkdata.cpp \
    networkserver.cpp \
    networksocket.cpp \
    pvpgame.cpp \
    pvrgame.cpp \
    surakarta_agent_mine.cpp \
    surakarta_agent_random.cpp \
    surakarta_game.cpp \
    surakarta_reason.cpp \
    surakarta_rule_manager.cpp

HEADERS += \
    aithread.h \
    chess.h \
    choose.h \
    client.h \
    global_random_generator.h \
    mainwindow.h \
    networkdata.h \
    networkserver.h \
    networksocket.h \
    pvpgame.h \
    pvrgame.h \
    surakarta_agent_base.h \
    surakarta_agent_mine.h \
    surakarta_agent_random.h \
    surakarta_board.h \
    surakarta_common.h \
    surakarta_game.h \
    surakarta_piece.h \
    surakarta_reason.h \
    surakarta_rule_manager.h

FORMS += \
    chess.ui \
    choose.ui \
    client.ui \
    mainwindow.ui \
    pvpgame.ui \
    pvrgame.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    1.qrc \
    3.qrc

SUBDIRS += \
    ../myqt/networkerserver/networkerserver.pro

DISTFILES += \
    ../myqt/networkerserver/.gitignore \
    ../myqt/networkerserver/networkerserver.pro.user
