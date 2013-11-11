#-------------------------------------------------
#
# Project created by QtCreator 2013-07-27T15:30:15
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = J_Imgproc
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    utilfuncs.cpp \
    imagestorage.cpp \
    vernier.cpp

HEADERS  += mainwindow.h \
    utilfuncs.h \
    imagestorage.h \
    vernier.h

FORMS    += mainwindow.ui

INCLUDEPATH += C:\opencv2.4.5\opencv\build\include
INCLUDEPATH += C:\JsImageProc\Jethro_Improc_GUI\dbgsupport
INCLUDEPATH += C:\JsImageProc\Jethro_Improc_GUI\mods_and_ctrls
INCLUDEPATH += C:\JsImageProc\Jethro_Improc_GUI\ocv_SISR

LIBS += C:\opencv2.4.5\opencv\build\x86\vc11\lib\opencv_core245.lib
LIBS += C:\opencv2.4.5\opencv\build\x86\vc11\lib\opencv_highgui245.lib
LIBS += C:\opencv2.4.5\opencv\build\x86\vc11\lib\opencv_imgproc245.lib
LIBS += C:\opencv2.4.5\opencv\build\x86\vc11\lib\opencv_photo245.lib

LIBS += C:\JsImageProc\Jethro_Improc_GUI\Release\mods_and_ctrls.lib
LIBS += C:\JsImageProc\Jethro_Improc_GUI\Release\dbgsupport.lib
