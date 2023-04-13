QT += quick

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        ZyHtmlUtil.cpp \
        main.cpp \
        mylineedit.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    ZyHtmlUtil.h \
    mylineedit.h

# 处理  initial memory too small 的问题
#值为64KB的倍数
wasm:QMAKE_LFLAGS += -s \"TOTAL_MEMORY=33554432\"


# https://www.jianshu.com/p/488b93e9b3e8
# https://blog.csdn.net/qq_38882906/article/details/80436866
# 导出后，可以在js中使用c的函数
#wasm:QMAKE_CXXFLAGS += -s EXTRA_EXPORTED_RUNTIME_METHODS=[\"ccall\"]
wasm:QMAKE_LFLAGS += -s EXTRA_EXPORTED_RUNTIME_METHODS=[\"UTF16ToString\",\"stringToUTF16\",\"ccall\"]
