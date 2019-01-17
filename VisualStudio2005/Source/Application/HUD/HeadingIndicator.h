#pragma once
#include "../../Direct3D9/Direct3D9.h"

namespace Application
{
	namespace HUD
	{
		class HeadingIndicator
		{
		private:
			float displayWidth,displayHeight;
			//��ʾ��Ⱥ͸߶�	ʹ��float����Ϊ�ռ��е��������float����
			D3DXVECTOR3 displayCenter;//��ʾ�����ڿռ��е�λ��
			UINT8 degreesPerDiv,numberOfDivisionToDisplay;

			float degrees;
		public:
			HeadingIndicator(
				UINT8 degreesPerDiv,UINT8 numberOfDivisionToDisplay,
				D3DXVECTOR3 displayCenter,float displayWidth,float displayHeight);
			
			void setDegrees(float degrees);

			void render(IDirect3DDevice9 *device);
		};
	}
}