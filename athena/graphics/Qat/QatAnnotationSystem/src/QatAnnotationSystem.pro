######################################################################
# Automatically generated by qmake (2.01a) Mon May 2 09:49:50 2011
######################################################################

TEMPLATE = lib plugin
TARGET = 
DEPENDPATH += .
INCLUDEPATH += . .. ../../QatPlotWidgets ../../QatSystemInterfaces ../../QatPlotting ../../QatDataModelling ../../QatDataAnalysis ../../QatPresenter
DESTDIR=../../../plugins
CONFIG += plugin debug
LIBS += -L../../../lib -L/usr/local/lib -lQatPresenter -lQatSystemInterfaces -lQatPlotWidgets -lQatPlotting -l QatDataModeling -lQatDataAnalysis -lCLHEP -lgfortran
FORMS=AnnotationSystemMainWidget.ui AnnotationSystemMenu.ui TextEditor.ui

SOURCES += AnnotationSystem.cpp AnnotationSystemMainWidget.cpp AnnotationSystemMenu.cpp TextEditor.cpp
HEADERS += ../QatAnnotationSystem/AnnotationSystem.h    ../QatAnnotationSystem/AnnotationSystemMainWidget.h   ../QatAnnotationSystem/AnnotationSystemMenu.h ../QatAnnotationSystem/TextEditor.h

INSTALLS += target
target.path=/usr/local/share/qatplugins


