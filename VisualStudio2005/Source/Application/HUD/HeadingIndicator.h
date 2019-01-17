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
			//显示宽度和高度	使用float是因为空间中的坐标就是float类型
			D3DXVECTOR3 displayCenter;//显示中心在空间中的位置
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