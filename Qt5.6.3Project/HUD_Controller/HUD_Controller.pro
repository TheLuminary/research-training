#-------------------------------------------------
#
# Project created by QtCreator 2019-01-17T03:33:12
#
#-------------------------------------------------

QT       += core gui

# 添加ARINC429驱动的静态库
LIBS += -L$$PWD/Library/ -lExcalibur1553B
# 采用相对路径

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HUD_Controller
TEMPLATE = app


SOURCES += \
    Source/Application/Thread/DataTransmitter.cpp \
    Source/Application/Widget/PitchSetter.cpp \
    Source/Application/Widget/RollSetter.cpp \
    Source/Application/Widget/Widget.cpp \
    Source/Application/ParameterStore.cpp \
    Source/main.cpp \
    Source/Application/Widget/INS.cpp \
    Source/Application/Widget/Entry.cpp \
    Source/Application/Widget/YawSetter.cpp \
    Source/Application/Widget/HeightSetter.cpp \
    Source/Application/Widget/ADC.cpp \
    Source/Application/Widget/SpeedSetter.cpp \
    Source/Application/Widget/MachSetter.cpp \
    Source/Application/Widget/AltitudeSetter.cpp \
    Source/Application/Widget/ALT.cpp

HEADERS  += \
    Source/Application/MIL-STD-1553B/Deviceio.h \
    Source/Application/MIL-STD-1553B/error_devio.h \
    Source/Application/MIL-STD-1553B/Error_mch.h \
    Source/Application/MIL-STD-1553B/exc4000.h \
    Source/Application/MIL-STD-1553B/excdef.h \
    Source/Application/MIL-STD-1553B/Excsysio.h \
    Source/Application/MIL-STD-1553B/flags_mch.h \
    Source/Application/MIL-STD-1553B/Instance_mch.h \
    Source/Application/MIL-STD-1553B/mchIncl.h \
    Source/Application/MIL-STD-1553B/mem_mch.h \
    Source/Application/MIL-STD-1553B/Proto_mch.h \
    Source/Application/Thread/DataTransmitter.h \
    Source/Application/Widget/PitchSetter.h \
    Source/Application/Widget/RollSetter.h \
    Source/Application/Widget/Widget.h \
    Source/Application/ParameterStore.h \
    Source/Application/Widget/INS.h \
    Source/Application/Widget/Entry.h \
    Source/Application/Widget/YawSetter.h \
    Source/Application/Widget/HeightSetter.h \
    Source/Application/Widget/ADC.h \
    Source/Application/Widget/SpeedSetter.h \
    Source/Application/Widget/MachSetter.h \
    Source/Application/Widget/AltitudeSetter.h \
    Source/Application/Widget/ALT.h
