QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    brush.cpp \
    brushHandler.cpp \
    dataIO.cpp \
    graphics.cpp \
    main.cpp \
    mainwindow.cpp \
    opencv_handler.cpp

HEADERS += \
    brush.h \
    brushHandler.h \
    dataIO.h \
    graphics.h \
    mainwindow.h \
    opencv_handler.h \
    stdfuncs.h

FORMS += \
    mainwindow.ui


# The following are placeholder stubs for OpenCV imports
# They will be implemented later. For now they specify how the team should
# set up their opencv directory

#INCLUDEPATH += D:\OpenCV\build\include

#LIBS += C:/OpenCV/build/lib/opencv_core440.lib
#LIBS += C:/OpenCV/build/lib/opencv_highgui440.lib
#LIBS += C:/OpenCV/build/lib/opencv_imgcodecs440.lib
#LIBS += C:/OpenCV/build/lib/opencv_imgproc440.lib
#LIBS += C:/OpenCV/build/lib/opencv_features2d440.lib
#LIBS += C:/OpenCV/build/lib/opencv_calib3d440.lib
#LIBS += C:/OpenCV/build/lib/opencv_videoio440.lib


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
