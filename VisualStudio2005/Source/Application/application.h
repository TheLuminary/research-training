#pragma once

//����MIL-STD-1553B PCI MCH�Ŀ�
#include "./MIL-STD-1553B/Proto_mch.h"
#pragma comment(lib,"Excalibur1553B.lib")
//��ΪѧԺ¥������MIL-STD-1553B��ҪVisual C++ 6.0���ܱ��룬����ֻ�ܸ����ز����˶�̬���ӿ�
//����һ���������

#include "./Message/message.h"
#include "./HUD/ParameterStore.h"
#include "./HUD/Thread/threads.h"

#include "./HUD/AltitudeIndicator.h"//���ߵ�߶�ָʾ��
#include "./HUD/AttitudeIndicator.h"//��̬��ʾ
#include "./HUD/HeadingIndicator.h"//����ָʾ��
#include "./HUD/HeightIndicator.h"//�߶�ָʾ��
#include "./HUD/SpeedIndicator.h"//�ٶ�ָʾ��
#include "./HUD/MachIndicator.h"//�������ʾ

//��������ת��ƫ��Ӧ����INS������
//�ٶȺ͸߶��Ǵ������ݼ����������
//���ߵ�߶���ALT������