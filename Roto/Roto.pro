#TARGET = Roto

QT = core gui widgets

RC_ICONS = execIco.ico

HEADERS = \
    $$PWD/brush.h \
    $$PWD/brushhandler.h \
    $$PWD/brushshape.h \
    $$PWD/dataIOHandler.h \
    $$PWD/graphics.h \
    $$PWD/layer.h \
    $$PWD/mainwindow.h \
    $$PWD/patternprofiler.h \
    $$PWD/radialprofiler.h \
    $$PWD/screenrender.h \
    $$PWD/splinevector.h \
    $$PWD/stdfuncs.h \
    $$PWD/resizewindow.h \
    $$PWD/triangle.h \
    $$PWD/ui_mainwindow.h \
    $$PWD/ui_radialprofiler.h \
    $$PWD/ui_resizewindow.h \
    $$PWD/viewscroller.h\
    $$PWD/ui_brushShape.h\
    $$PWD/ui_patternprofiler.h\
    $$PWD/brushShape.h\
    $$PWD/patternprofiler.h

SOURCES = \
    $$PWD/brush.cpp \
    $$PWD/brushhandler.cpp \
    $$PWD/brushshape.cpp \
    $$PWD/dataIOHandler.cpp \
    $$PWD/graphics.cpp \
    $$PWD/layer.cpp \
    $$PWD/main.cpp \
    $$PWD/mainwindow.cpp \
    $$PWD/mainwindow.ui \
    $$PWD/patternprofiler.cpp \
    $$PWD/radialprofiler.cpp \
    $$PWD/resizewindow.cpp \
    $$PWD/screenrender.cpp \
    $$PWD/splinevector.cpp \
    $$PWD/triangle.cpp \
    $$PWD/viewscroller.cpp\
    $$PWD/brushShape.cpp\
    $$PWD/patternprofiler.cpp

LIBS += -LC:\opencv_build\install\x64\vc16\lib -lopencv_core451 -lopencv_imgproc451 -lopencv_highgui451 -lopencv_imgcodecs451 -lopencv_videoio451 -lopencv_video451 -lopencv_calib3d451 -lopencv_photo451 -lopencv_features2d451
INCLUDEPATH += C:\opencv_build\install\include
DEPENDPATH += C:\opencv_build\install\include

# Default rules for deployment
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
