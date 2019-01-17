#include "Direct3D9.h"

namespace Direct3D
{
	namespace v9
	{
		void enterMessageLoop(
			bool (*displayFunction)(double totalTime,double deltaTime,IDirect3DDevice9 *device,HWND windowHandle),
			IDirect3DDevice9 *device,HWND windowHandle
			)
		{
			MSG message;
			ZeroMemory(&message,sizeof(MSG));

			double appStartTime  = (double)timeGetTime();
			double lastDisplayFunctionEnterTime = appStartTime;

			Direct3D::v9::resource::OneFrameLifecycleResource::getInstance()->clear(device);

			while(message.message!=WM_QUIT)
			{
				if(PeekMessage(&message,0,0,0,PM_REMOVE))
				{
					TranslateMessage(&message);
					DispatchMessage(&message);
				}
				else
				{
					double currentDisplayFunctionEnterTime = (double)timeGetTime();
					displayFunction(
						currentDisplayFunctionEnterTime - appStartTime,
						currentDisplayFunctionEnterTime - lastDisplayFunctionEnterTime,
						device,windowHandle);
					lastDisplayFunctionEnterTime = currentDisplayFunctionEnterTime;

					//释放生命周期为一帧的资源
					Direct3D::v9::resource::OneFrameLifecycleResource::getInstance()->clear(device);
				}
			}
		}
	}
}