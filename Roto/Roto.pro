#TARGET = Roto

QT = core gui widgets

HEADERS = \
    $$PWD/brush.h \
    $$PWD/brushhandler.h \
    $$PWD/dataIOHandler.h \
    $$PWD/graphics.h \
    $$PWD/layer.h \
    $$PWD/mainwindow.h \
    $$PWD/opencv_handler.h \
    $$PWD/radialprofiler.h \
    $$PWD/screenrender.h \
    $$PWD/splinevector.h \
    $$PWD/stdfuncs.h \
    $$PWD/resizewindow.h \
    $$PWD/triangle.h \
    $$PWD/ui_mainwindow.h \
    $$PWD/ui_radialprofiler.h \
    $$PWD/ui_resizewindow.h \
<<<<<<< Updated upstream
    $$PWD/undoredo.h \
    $$PWD/viewscroller.h
=======
    $$PWD/viewscroller.h\
    $$PWD/undoRedo.h\
    $$PWD/ui_brushShape.h\
    $$PWD/ui_patternprofiler.h\
    $$PWD/brushShape.h\
    $$PWD/patternprofiler.h\
>>>>>>> Stashed changes

SOURCES = \
    $$PWD/brush.cpp \
    $$PWD/brushhandler.cpp \
    $$PWD/dataIOHandler.cpp \
    $$PWD/graphics.cpp \
    $$PWD/layer.cpp \
    $$PWD/main.cpp \
    $$PWD/mainwindow.cpp \
    $$PWD/mainwindow.ui \
    $$PWD/opencv_handler.cpp \
    $$PWD/radialprofiler.cpp \
    $$PWD/resizewindow.cpp \
    $$PWD/screenrender.cpp \
    $$PWD/splinevector.cpp \
    $$PWD/triangle.cpp \
<<<<<<< Updated upstream
    $$PWD/undoredo.cpp \
    $$PWD/viewscroller.cpp
=======
    $$PWD/viewscroller.cpp\
    $$PWD/undoRedo.cpp\
    $$PWD/brushShape.cpp\
    $$PWD/patternprofiler.cpp\
>>>>>>> Stashed changes

LIBS += -LC:\opencv_build\install\x64\vc16\lib -lopencv_core450 -lopencv_imgproc450 -lopencv_highgui450 -lopencv_imgcodecs450 -lopencv_videoio450 -lopencv_video450 -lopencv_calib3d450 -lopencv_photo450 -lopencv_features2d450
INCLUDEPATH += C:\opencv_build\install\include
DEPENDPATH += C:\opencv_build\install\include

# Default rules for deployment
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
