#include "./ParameterStore.h"

namespace Application
{
	namespace HUD
	{
		ParameterStore::ParameterStore(void)
		{
			this->pitch = 0;
			this->roll = 0;
			this->yaw = 0;

			this->height = 0;
			this->speed = 0;
			this->mach = 0;

			this->altitude = 0;
		}
		ParameterStore *ParameterStore::instance = NULL;
		ParameterStore *ParameterStore::getInstance()
		{
			while(instance == NULL) instance = new ParameterStore();
			return instance;
		}
		//�����ǵ���ģʽʵ��
		//������������ĸ��ݡ����յ�����Ϣ�������ݵĺ���
		bool ParameterStore::update(std::list<UINT32> newMessage)
		{
			struct Application::Message::Message message;
			if(Application::Message::Util::convert32bitDataListToMessage(newMessage,&message))
			{
				//ת���ɹ�
				switch(message.type)
				{
				case 41:
					//pitch
					if(message.isDouble)
					{
						this->pitch = message.data.float64;
						return true;
					}
					if(message.isFloat)
					{
						this->pitch = message.data.float32;
						return true;
					}
					return false;
					break;
				case 42:
					//roll
					if(message.isDouble)
					{
						this->roll = message.data.float64;
						return true;
					}
					if(message.isFloat)
					{
						this->roll = message.data.float32;
						return true;
					}
					return false;
					break;
				case 43:
					//yaw
					if(message.isDouble)
					{
						this->yaw = message.data.float64;
						return true;
					}
					if(message.isFloat)
					{
						this->yaw = message.data.float32;
						return true;
					}
					return false;
					break;
				default:
					return false;
				}
			}
			else return false;
		}

		double ParameterStore::getPitch()
		{
			return this->pitch;
		}
		double ParameterStore::getRoll()
		{
			return this->roll;
		}
		double ParameterStore::getYaw()
		{
			return this->yaw;
		}
		double ParameterStore::getHeight()
		{
			return this->height;
		}
		double ParameterStore::getSpeed()
		{
			return this->speed;
		}
		double ParameterStore::getMach()
		{
			return this->mach;
		}

		double ParameterStore::getAltitude()
		{
			return this->altitude;
		}

		//setter methods are listed as follows
		void ParameterStore::setPitch(double pitch)
		{
			this->pitch = pitch;
		}
		void ParameterStore::setRoll(double roll)
		{
			this->roll = roll;
		}
		void ParameterStore::setYaw(double yaw)
		{
			this->yaw = yaw;
		}
		void ParameterStore::setHeight(double height)
		{
			this->height = height;
		}
		void ParameterStore::setSpeed(double speed)
		{
			this->speed = speed;
		}
		void ParameterStore::setMach(double mach)
		{
			this->mach = mach;
		}
		void ParameterStore::setMach(UINT16 mach)
		{
			this->mach = ((double)mach)/100;//��λ�Ķ���С��
		}

		/////
		void ParameterStore::setAltitude(double altitude)
		{
			this->altitude = altitude;
		}
	}
}