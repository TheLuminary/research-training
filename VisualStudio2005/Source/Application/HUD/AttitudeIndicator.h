#pragma once
#include "../../Direct3D9/Direct3D9.h"

namespace Application
{
	namespace HUD
	{
		class AttitudeIndicator
		{
		public:
			//�ԽǶ�ֵλ��λ����Ұ�Ƕȣ���һ��Ϊ20
			//�ԽǶ�ֵΪ��λ��ÿ�ָ��ĽǶȣ�һ��Ϊ5��10
			AttitudeIndicator(
				UINT8 fieldAngle,
				UINT8 anglePerDivision,
				UINT R = 500,UINT Z = 1000);

			//�ԽǶ�Ϊ��λ���ø����ǣ��ᱻͳһ����90֮��
			void setPitch(double pitch);
			//�ԽǶ�Ϊ��λ���ù�ת�ǣ���Ȼ����Ҫ���ñ߽絫����û���Ӧ��ͳһ�ڡ�����Χ
			void setRoll(double roll);

			void render(IDirect3DDevice9 *device);

		private:
			UINT8 fieldAngle,degreePerDivision;
			UINT R,Z;
			double pitch,roll;
		};
	}
}