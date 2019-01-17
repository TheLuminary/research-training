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
			//显示宽度和高度	使用float是因为空间中的坐标就是float类型
			D3DXVECTOR3 displayCenter;//显示中心在空间中的位置
			UINT16 speedPerDiv,numberOfDivisionToDisplay;
			//每分隔显示多少米 和在HUD上要显示多少个分隔
			//选择UINT为了防止显示的文字出现小数

			UINT16 speed;
		public:
			SpeedIndicator(UINT16 speedPerDiv,UINT8 numberOfDivisionToDisplay,
				D3DXVECTOR3 displayCenter,float displayWidth,float displayHeight);
			//构造函数体现了一些柯里化的想法，确定了除高度以外的参数

			void setSpeed(unsigned short int speed);

			void render(IDirect3DDevice9 *device);
		};
	}
}