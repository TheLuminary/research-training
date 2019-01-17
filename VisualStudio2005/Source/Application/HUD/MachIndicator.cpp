#include "MachIndicator.h"

#include <string>

namespace Application
{
	namespace HUD
	{
		void MachIndicator::display(IDirect3DDevice9 *device,float machNumber)
		{
			Direct3D::v9::resource::mesh::text::TextMesh machTextMesh = 
				Direct3D::v9::resource::mesh::MeshFactory::createTextMesh(
				device,this->convertMachNumberToString(machNumber),std::string("Microsoft YaHei")
				);
			machTextMesh.setTranslation(this->displayCenter);
			machTextMesh.setRotation(D3DXVECTOR3(0,0,0));
			machTextMesh.setScale(D3DXVECTOR3(80,80,0.001f));
			machTextMesh.render();
		}


		MachIndicator::MachIndicator(D3DXVECTOR3 displayCenter)
		{
			this->displayCenter = displayCenter;
		}

		std::string MachIndicator::convertMachNumberToString(float machNumber)
		{
			//将float转换成std::string并在前面加前缀
			char tempCharArray[16];
			sprintf_s(tempCharArray,"%.2f",machNumber);

			return std::string("M ").append(tempCharArray);
		}
	}
}