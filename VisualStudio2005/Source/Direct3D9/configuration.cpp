#include "Direct3D9.h"

namespace Direct3D
{
	namespace v9
	{
		namespace configuration
		{
			void setStaticCamera(
				IDirect3DDevice9 *device,D3DXVECTOR3 position,D3DXVECTOR3 target,
				float angle,UINT16 viewPortWidth,UINT16 viewPortHeight,
				float zMinToRender,float zMaxToRender
				)
			{
				D3DXMATRIX transformMatrix;
				D3DXMatrixLookAtLH(&transformMatrix,&position,&target,&D3DXVECTOR3(0.0f,1.0f,0.0f));
				device->SetTransform(D3DTS_VIEW,&transformMatrix);

				D3DXMatrixPerspectiveFovLH(&transformMatrix,angle,(float)viewPortWidth/(float)viewPortHeight,zMinToRender,zMaxToRender);
				device->SetTransform(D3DTS_PROJECTION,&transformMatrix);
			}
		}
	}
}