#pragma once

//导入MIL-STD-1553B PCI MCH的库
#include "./MIL-STD-1553B/Proto_mch.h"
#pragma comment(lib,"Excalibur1553B.lib")
//因为学院楼该死的MIL-STD-1553B需要Visual C++ 6.0才能编译，所以只能该死地采用了动态链接库
//我有一万个想骂人

#include "./Message/message.h"
#include "./HUD/ParameterStore.h"
#include "./HUD/Thread/threads.h"

#include "./HUD/AltitudeIndicator.h"//无线电高度指示器
#include "./HUD/AttitudeIndicator.h"//姿态显示
#include "./HUD/HeadingIndicator.h"//朝向指示器
#include "./HUD/HeightIndicator.h"//高度指示器
#include "./HUD/SpeedIndicator.h"//速度指示器
#include "./HUD/MachIndicator.h"//马赫数显示

//俯仰、滚转和偏航应该是INS给出的
//速度和高度是大气数据计算机给出的
//无线电高度是ALT给出的