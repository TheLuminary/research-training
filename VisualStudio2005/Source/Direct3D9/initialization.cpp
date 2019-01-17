#include "Direct3D9.h"

namespace Direct3D
{
	namespace v9
	{
		namespace initialization
		{
			UINT32 renderByHardwareOrSoftware(IDirect3D9 *direct3D9, D3DDEVTYPE deviceType)
			{
				D3DCAPS9 caps;
				direct3D9->GetDeviceCaps(D3DADAPTER_DEFAULT, deviceType, &caps);
				return (caps.DevCaps&D3DDEVCAPS_HWTRANSFORMANDLIGHT)?D3DCREATE_HARDWARE_VERTEXPROCESSING:D3DCREATE_SOFTWARE_VERTEXPROCESSING;
			}

			bool createDevice(IDirect3D9 *direct3D9, D3DDEVTYPE deviceType, HWND hwnd, UINT32 byHardwareOrSoftware,
				D3DPRESENT_PARAMETERS presentParameters, IDirect3DDevice9 **device
				)
			{
				//long
				HRESULT hResult = direct3D9->CreateDevice(D3DADAPTER_DEFAULT, deviceType, hwnd, byHardwareOrSoftware, &presentParameters, device);
				if (FAILED(hResult))
				{
					presentParameters.MultiSampleType = D3DMULTISAMPLE_8_SAMPLES;
					hResult = direct3D9->CreateDevice(D3DADAPTER_DEFAULT, deviceType, hwnd, byHardwareOrSoftware, &presentParameters, device);
					if(FAILED(hResult))
					{
						presentParameters.AutoDepthStencilFormat = D3DFMT_D16;
						presentParameters.MultiSampleType = D3DMULTISAMPLE_4_SAMPLES;
						hResult = direct3D9->CreateDevice(D3DADAPTER_DEFAULT, deviceType, hwnd, byHardwareOrSoftware, &presentParameters, device);
						if (FAILED(hResult))
						{
							presentParameters.MultiSampleType = D3DMULTISAMPLE_NONE;
							hResult = direct3D9->CreateDevice(D3DADAPTER_DEFAULT, deviceType, hwnd, byHardwareOrSoftware, &presentParameters, device);
							if (FAILED(hResult))
							{
								MessageBoxA(0, "Direct3D9CreateDeviceFailed", 0, 0);
								//释放资源
								direct3D9->Release();
								return false;
							}
						}
					}
				}
				direct3D9->Release();
				return true;
			}

			HWND createWindow(HINSTANCE hInstance, WNDPROC wndProc, UINT16 width, UINT16 height, bool isFullScreen)
			{
				//设计窗口类
				WNDCLASS windowClass;
				windowClass.style = CS_HREDRAW | CS_VREDRAW;
				windowClass.lpfnWndProc = (WNDPROC)wndProc;
				windowClass.cbClsExtra = 0;
				windowClass.cbWndExtra = 0;
				windowClass.hInstance = hInstance;
				windowClass.hIcon = LoadIcon(0,IDI_APPLICATION);
				windowClass.hCursor = LoadCursor(0, IDC_ARROW);
				windowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
				windowClass.lpszMenuName = 0;
				windowClass.lpszClassName = TEXT("Direct3D Window");
				//注册窗口类
				if(!RegisterClass(&windowClass))
				{
					MessageBoxA(0,"RegisterClass Failed",0,0);
					return 0;
				}
				//创建窗口
				//这里的第一个参数必须是上面创建的类名
				//HWND hwnd = CreateWindow(TEXT("Direct3D window"),TEXT("Direct3D project"),WS_EX_TOPMOST|WS_OVERLAPPEDWINDOW,0,0,width,height,0,0,hInstance,0);
				//这里的改变是为了禁止窗口改变大小
				HWND hwnd = CreateWindow(TEXT("Direct3D window"),TEXT("Direct3D project"),WS_EX_TOPMOST|(WS_OVERLAPPEDWINDOW^WS_THICKFRAME),0,0,width,height,0,0,hInstance,0);
				//显示及更新窗口
				if(!hwnd)
				{
					MessageBoxA(0,"CreateWindow Failed.",0,0);
					return 0;
				}

				ShowWindow(hwnd,SW_SHOW);
				UpdateWindow(hwnd);
				return hwnd;
			}


			LRESULT CALLBACK defaultWindowProc(
				HWND hwnd,
				UINT uMessage,
				WPARAM wParam,
				LPARAM lParam
				)
			{
				switch(uMessage)
				{
				case WM_DESTROY:
					::PostQuitMessage(0);
					break;
				case WM_KEYDOWN:
					if(wParam == VK_ESCAPE)
						::DestroyWindow(hwnd);
					break;
				}
				return DefWindowProc(hwnd,uMessage,wParam,lParam);
			}



			D3DPRESENT_PARAMETERS createPresentParameters(HWND hwnd, UINT16 width, UINT16 height, bool isFullScreen)
			{
				D3DPRESENT_PARAMETERS presentParameters;
				presentParameters.BackBufferWidth = width;
				presentParameters.BackBufferHeight = height;
				presentParameters.BackBufferFormat = D3DFMT_A8R8G8B8;
				presentParameters.BackBufferCount = 1;
				//presentParameters.MultiSampleType = D3DMULTISAMPLE_NONE;
				presentParameters.MultiSampleType = D3DMULTISAMPLE_16_SAMPLES;;
				presentParameters.MultiSampleQuality = 0;
				presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
				presentParameters.hDeviceWindow = hwnd;
				presentParameters.Windowed = !isFullScreen;
				presentParameters.EnableAutoDepthStencil = true;
				presentParameters.AutoDepthStencilFormat = D3DFMT_D24S8;
				presentParameters.Flags = 0;
				presentParameters.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
				presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

				return presentParameters;
			}

			bool initialize(HINSTANCE hInstance, WNDPROC wndProc, UINT16 width, UINT16 height, bool isFullScreen, D3DDEVTYPE deviceType, IDirect3DDevice9 **device,HWND *windowHandle)
			{


				//创建窗口
				HWND hwnd = createWindow(hInstance,wndProc,width,height,isFullScreen);
				if(!hwnd)
				{
					MessageBoxA(0,"Direct3D CreateWindow Failed",0,0);
					return false;
				}

				//创建Direct3D 9接口
				//这是最原始的防止污染全局命名空间的方法
				IDirect3D9 *direct3D9 = Direct3DCreate9(D3D_SDK_VERSION);
				if(!direct3D9)
				{
					MessageBoxA(0,"Direct3DCreate9() Failed",0,0);
					return false;
				}
				UINT32 byHardwareOrSoftware = renderByHardwareOrSoftware(direct3D9,deviceType);

				//创建参数
				D3DPRESENT_PARAMETERS presentParameters = createPresentParameters(hwnd,width,height,isFullScreen);
				//创建设备
				if (createDevice(direct3D9,deviceType,hwnd,byHardwareOrSoftware,presentParameters,device))
				{
					if(windowHandle!=NULL) *windowHandle = hwnd;
					return true;
				}
				else return false;
			}
		}
	}
}