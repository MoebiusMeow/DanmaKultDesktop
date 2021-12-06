QT       += core gui
QT       += opengl openglwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ScreenOverlay/danmakutestset.cpp \
    ScreenOverlay/danmakutext.cpp \
    ScreenOverlay/danmakutextbottom.cpp \
    ScreenOverlay/danmakutextbottomset.cpp \
    ScreenOverlay/danmakutextbuttomset.cpp \
    ScreenOverlay/danmakutextfloat.cpp \
    ScreenOverlay/danmakutextfloatset.cpp \
    ScreenOverlay/danmakutextset.cpp \
    ScreenOverlay/danmakutexttop.cpp \
    ScreenOverlay/danmakutexttopset.cpp \
    ScreenOverlay/danmakuwidget.cpp \
    View/DynamicVBoxLayout.cpp \
    View/KultLoginBox.cpp \
    View/KultMessageBox.cpp \
    View/MainWindow.cpp \
    View/WaveWidget.cpp \
    main.cpp \

HEADERS += \
    ScreenOverlay/danmakutestset.h \
    ScreenOverlay/danmakutext.h \
    ScreenOverlay/danmakutextbottom.h \
    ScreenOverlay/danmakutextbottomset.h \
    ScreenOverlay/danmakutextbuttomset.h \
    ScreenOverlay/danmakutextfloat.h \
    ScreenOverlay/danmakutextfloatset.h \
    ScreenOverlay/danmakutextset.h \
    ScreenOverlay/danmakutexttop.h \
    ScreenOverlay/danmakutexttopset.h \
    ScreenOverlay/danmakuwidget.h \
    View/DynamicVBoxLayout.h \
    View/KultLoginBox.h \
    View/KultMessageBox.h \
    View/MainWindow.h \
    View/WaveWidget.h \

TRANSLATIONS += \
    DanmaKuItDesktop_en_US.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

DISTFILES += \
    Assets/Shaders/plainUV.vert \
    Assets/Shaders/waveShader.frag \
    Assets/Styles/mainUI.qss