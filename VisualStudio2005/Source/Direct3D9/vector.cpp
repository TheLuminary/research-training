#include "Direct3D9.h"
#include <cmath>

namespace Direct3D
{
	namespace v9
	{
		namespace resource
		{
			namespace vector
			{
				D3DXVECTOR3 rotate(D3DXVECTOR3 vector,D3DXVECTOR3 rotation)
				{
					if(rotation.x!=0)
					{
						//绕x轴旋转,x不会变
						float y = vector.y;
						float z = vector.z;
						vector.y = y*cos(rotation.x)-z*sin(rotation.x);
						vector.z = y*sin(rotation.x)+z*cos(rotation.x);
					}
					if(rotation.y!=0)
					{
						//绕y轴旋转,y不会变
						float x = vector.x;
						float z = vector.z;
						vector.x =  x*cos(rotation.y)+z*sin(rotation.y);
						vector.z = -x*sin(rotation.y)+z*cos(rotation.y);
					}
					if(rotation.z!=0)
					{
						//绕z轴旋转,z不会变
						float x = vector.x;
						float y = vector.y;
						vector.x = x*cos(rotation.z)-y*sin(rotation.z);
						vector.y = x*sin(rotation.z)+y*cos(rotation.z);
					}
					return vector;
				}
			}
		}
	}
}