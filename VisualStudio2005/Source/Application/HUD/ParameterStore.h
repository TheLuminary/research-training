#pragma once

#include <list>
#include "../Message/message.h"

namespace Application
{
	namespace HUD
	{
		class ParameterStore
		{
		private:
			static ParameterStore *instance;
			ParameterStore(void);
		public:
			static ParameterStore *getInstance();
			//�����ǵ���ģʽʵ��

			//���ݴ�socket��ARINC 429���߽��յ�����Ϣ�����²���
			bool update(std::list<UINT32> newMessage);

			double getPitch();//41
			double getRoll();//42
			double getYaw();//43

			double getHeight();//44
			double getSpeed();//45
			double getMach();//46

			double getAltitude();


			//�����⼸��setter method�����ǲ���ӵ�
			//����Ϊ��1553Bֻ����Ϊ���ѡ�������
			void setPitch(double pitch);
			void setRoll(double roll);
			void setYaw(double yaw);
			void setHeight(double height);
			void setSpeed(double speed);
			void setMach(double mach);
			void setMach(UINT16 mach);
			//��ϡ�������ʱѧ��1m/s = 3.6km/s

			void setAltitude(double altitude);
			

		private:
			double pitch,roll,yaw;
			double speed,height,mach;
			//�����������Ҫռ�������֣�һ��32λ��������
			//����˵������С������UINT16�͹���

			double altitude;

			//�ںڰ�������ǰ��
		};
	}
}