#include "AltitudeIndicator.h"

#include <string>

namespace Application
{
	namespace HUD
	{
		void AltitudeIndicator::display(IDirect3DDevice9 *device,float altitude)
		{
			Direct3D::v9::resource::mesh::text::TextMesh altitudeTextMesh = 
				Direct3D::v9::resource::mesh::MeshFactory::createTextMesh(
				device,this->convertAltitudeNumberToString(altitude),std::string("Microsoft YaHei")
				);
			altitudeTextMesh.setTranslation(this->displayCenter);
			altitudeTextMesh.setRotation(D3DXVECTOR3(0,0,0));
			altitudeTextMesh.setScale(D3DXVECTOR3(80,80,0.001f));
			altitudeTextMesh.render();
		}

		AltitudeIndicator::AltitudeIndicator(D3DXVECTOR3 displayCenter)
		{
			this->displayCenter = displayCenter;
		}

		std::string AltitudeIndicator::convertAltitudeNumberToString(float altitude)
		{
			//将float转换成std::string并在前面加前缀
			char tempCharArray[16];
			sprintf_s(tempCharArray,"%.0f",altitude);

			return std::string("A ").append(tempCharArray);
		}
	}
}