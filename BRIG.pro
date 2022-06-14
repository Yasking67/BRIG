#-------------------------------------------------
#
# Project created by Nikita Vernigor 2021-04-12T15:49:31
#
#-------------------------------------------------

QT       += core gui
QT       += network
QT       += widgets serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Brig_VideoRecord
TEMPLATE = app

CONFIG += c++17

SOURCES += main.cpp\
            MainWindow.cpp \
            Converts.cpp \
            DataProcessing.cpp \
            Form_VideoFrame.cpp \
            ImageProcessing.cpp \
            Tracker.cpp \
            UDP_Receiver.cpp \
            UDP_Sender.cpp \
            Uart.cpp

HEADERS  += MainWindow.h \
            Converts.h \
            DataProcessing.h \
            Form_VideoFrame.h \
            ImageProcessing.h \
            RunYOLO.h \
            Tracker.h \
            UDP_Receiver.h \
            UDP_Sender.h \
            Uart.h

FORMS    += MainWindow.ui \
            Form_VideoFrame.ui \
            UART.ui

INCLUDEPATH += "D:/Qt/WpdPack/Include/"
INCLUDEPATH += "D:/Qt/WpdPack/Lib/x64"
LIBS += -L"D:/Qt/WpdPack/Lib/x64" -lwpcap

INCLUDEPATH += C:/opencv/build/install/include
LIBS += -L"C:/opencv/build/install/x64/vc16/lib" -lopencv_core451
LIBS += -L"C:/opencv/build/install/x64/vc16/lib" -lopencv_highgui451
LIBS += -L"C:/opencv/build/install/x64/vc16/lib" -lopencv_imgproc451
LIBS += -L"C:/opencv/build/install/x64/vc16/lib" -lopencv_features2d451
LIBS += -L"C:/opencv/build/install/x64/vc16/lib" -lopencv_calib3d451
LIBS += -L"C:/opencv/build/install/x64/vc16/lib" -lopencv_video451
LIBS += -L"C:/opencv/build/install/x64/vc16/lib" -lopencv_videoio451
LIBS += -L"C:/opencv/build/install/x64/vc16/lib" -lopencv_imgcodecs451
LIBS += -L"C:/opencv/build/install/x64/vc16/lib" -lopencv_dnn451
LIBS += -L"C:/opencv/build/install/x64/vc16/lib" -lopencv_tracking451


win32:LIBS += -lIphlpapi

