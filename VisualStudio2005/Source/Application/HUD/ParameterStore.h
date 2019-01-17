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
			//以上是单例模式实现

			//根据从socket或ARINC 429总线接收到的消息，更新参数
			bool update(std::list<UINT32> newMessage);

			double getPitch();//41
			double getRoll();//42
			double getYaw();//43

			double getHeight();//44
			double getSpeed();//45
			double getMach();//46

			double getAltitude();


			//下面这几个setter method本来是不想加的
			//但是为了1553B只好免为其难…………
			void setPitch(double pitch);
			void setRoll(double roll);
			void setYaw(double yaw);
			void setHeight(double height);
			void setSpeed(double speed);
			void setMach(double mach);
			void setMach(UINT16 mach);
			//依稀想起初中时学过1m/s = 3.6km/s

			void setAltitude(double altitude);
			

		private:
			double pitch,roll,yaw;
			double speed,height,mach;
			//马赫数传输需要占用两个字，一共32位…………
			//还是说传定点小数，用UINT16就够了

			double altitude;

			//在黑暗中砥砺前行
		};
	}
}