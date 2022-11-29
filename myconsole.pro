TEMPLATE = app
CONFIG += console
CONFIG += warn_off
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        example.cpp \
        myconsole.cpp

HEADERS += \
    myconsole.h
win32{
LIBS += -lwsock32   \
        -lws2_32
}
MOC_DIR = temp/moc
RCC_DIR = temp/rcc
UI_DIR = temp/ui
OBJECTS_DIR = temp/obj
#指定编译生成的可执行文件到bin目录
DESTDIR     = $$PWD/../bin
