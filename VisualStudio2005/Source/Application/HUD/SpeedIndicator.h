#pragma once
#include "../../Direct3D9/Direct3D9.h"

namespace Application
{
	namespace HUD
	{
		class SpeedIndicator
		{
		private:
			float displayWidth,displayHeight;
			//��ʾ��Ⱥ͸߶�	ʹ��float����Ϊ�ռ��е��������float����
			D3DXVECTOR3 displayCenter;//��ʾ�����ڿռ��е�λ��
			UINT16 speedPerDiv,numberOfDivisionToDisplay;
			//ÿ�ָ���ʾ������ ����HUD��Ҫ��ʾ���ٸ��ָ�
			//ѡ��UINTΪ�˷�ֹ��ʾ�����ֳ���С��

			UINT16 speed;
		public:
			SpeedIndicator(UINT16 speedPerDiv,UINT8 numberOfDivisionToDisplay,
				D3DXVECTOR3 displayCenter,float displayWidth,float displayHeight);
			//���캯��������һЩ���ﻯ���뷨��ȷ���˳��߶�����Ĳ���

			void setSpeed(unsigned short int speed);

			void render(IDirect3DDevice9 *device);
		};
	}
}