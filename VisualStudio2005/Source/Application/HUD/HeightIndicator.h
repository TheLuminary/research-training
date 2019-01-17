#pragma once
#include "../../Direct3D9/Direct3D9.h"

namespace Application
{
	namespace HUD
	{
		class HeightIndicator
		{
		private:
			float displayWidth,displayHeight;
			//��ʾ��Ⱥ͸߶�	ʹ��float����Ϊ�ռ��е��������float����
			D3DXVECTOR3 displayCenter;//��ʾ�����ڿռ��е�λ��
			UINT16 metersPerDiv,numberOfDivisionToDisplay;
			//ÿ�ָ���ʾ������ ����HUD��Ҫ��ʾ���ٸ��ָ�
			//ѡ��UINTΪ�˷�ֹ��ʾ�����ֳ���С��

			float meters;
		public:
			HeightIndicator(UINT16 metersPerDiv,UINT8 numberOfDivisionToDisplay,
				D3DXVECTOR3 displayCenter,float displayWidth,float displayHeight);
			//���캯��������һЩ���ﻯ���뷨��ȷ���˳��߶�����Ĳ���

			void setMeters(float meters);

			void render(IDirect3DDevice9 *device);
		};
	}
}