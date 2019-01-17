#pragma once
#include "../../Direct3D9/Direct3D9.h"

namespace Application
{
	namespace HUD
	{
		class AttitudeIndicator
		{
		public:
			//以角度值位单位的视野角度，民航一般为20
			//以角度值为单位的每分隔的角度，一般为5或10
			AttitudeIndicator(
				UINT8 fieldAngle,
				UINT8 anglePerDivision,
				UINT R = 500,UINT Z = 1000);

			//以角度为单位设置俯仰角，会被统一到±90之内
			void setPitch(double pitch);
			//以角度为单位设置滚转角，虽然不需要设置边界但是最好还是应该统一在±Π范围
			void setRoll(double roll);

			void render(IDirect3DDevice9 *device);

		private:
			UINT8 fieldAngle,degreePerDivision;
			UINT R,Z;
			double pitch,roll;
		};
	}
}