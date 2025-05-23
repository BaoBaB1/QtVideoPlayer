TEMPLATE = app
TARGET = VideoPlayer

QT = core gui multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += main.cpp \
    MainWindow.cpp \
    VideoPlayer.cpp

HEADERS += \
    MainWindow.h \
    VideoPlayer.h
