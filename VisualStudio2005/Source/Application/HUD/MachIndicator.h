#pragma once
#include "../../Direct3D9/Direct3D9.h"

#include <string>

namespace Application
{
	namespace HUD
	{
		class MachIndicator
		{
		public:
			MachIndicator(D3DXVECTOR3 displayCenter);

			void display(IDirect3DDevice9 *device,float machNumber);
		private:
			D3DXVECTOR3 displayCenter;
			//将马赫数转换成用于显示的格式（在前面加M）
			std::string convertMachNumberToString(float machNumber);
		};
	}
}