#pragma once
#include "d3d9.h"
#include "d3dx9.h"

#include <list>
#include <map>
#include <string>

namespace Direct3D
{
	namespace v9
	{
		namespace initialization
		{
			//if the master GPU support,return D3DCREATE_HARDWARE_VERTEXPROCESSING
			//otherwise return D3DCREATE_SOFTWARE_VERTEXPROCESSING
			UINT32 renderByHardwareOrSoftware(IDirect3D9 *direct3D9, D3DDEVTYPE deviceType);

			bool createDevice(IDirect3D9 *direct3D9, D3DDEVTYPE deviceType, HWND hwnd, UINT32 byHardwareOrSoftware,
				D3DPRESENT_PARAMETERS presentParameters, IDirect3DDevice9 **device
				);

			D3DPRESENT_PARAMETERS createPresentParameters(HWND hwnd, UINT16 width, UINT16 height, bool isFullScreen);

			HWND createWindow(HINSTANCE hInstance, WNDPROC wndProc, UINT16 width, UINT16 height, bool isFullScreen);

			LRESULT CALLBACK defaultWindowProc(
				HWND hwnd,
				UINT uMessage,
				WPARAM wParam,
				LPARAM lParam
				);

			bool initialize(HINSTANCE hInstance, WNDPROC wndProc, UINT16 width, UINT16 height, bool isFullScreen, D3DDEVTYPE deviceType, IDirect3DDevice9 **device,HWND *windowHandle = NULL);
		}

		void enterMessageLoop(
			bool (*displayFunction)(double totalTime,double deltaTime,IDirect3DDevice9 *device,HWND windowHandle),
			IDirect3DDevice9 *device,HWND windowHandle
			);

		namespace configuration
		{
			void setStaticCamera(
				IDirect3DDevice9 *device,
				D3DXVECTOR3 position,D3DXVECTOR3 target,
				float angle,UINT16 viewPortWidth,UINT16 viewPortHeight,
				float zMinToRender,float zMaxToRender
				);
		}

		namespace resource
		{
			class OneFrameLifecycleResource
			{
				//单例！
			private:
				OneFrameLifecycleResource();
				~OneFrameLifecycleResource();
				static OneFrameLifecycleResource *instance;
			public:
				static OneFrameLifecycleResource *getInstance();
				//以上的部分用于实现单例模式

				void clear(IDirect3DDevice9 *device);//用于在一帧结束后，清空

				//返回新的大小
				UINT64 addResource(IDirect3DDevice9 *device,ID3DXMesh *mesh);
				//返回新的大小
				UINT64 addResource(IDirect3DDevice9 *device,IDirect3DVertexBuffer9 *vertexBuffer);

			private:
				std::map<IDirect3DDevice9 *,std::list<ID3DXMesh *>> mesh;
				std::map<IDirect3DDevice9 *,std::list<IDirect3DVertexBuffer9 *>> vertexBuffer;
			};

			namespace vector
			{
				D3DXVECTOR3 rotate(D3DXVECTOR3 vector,D3DXVECTOR3 rotation);
			}

			namespace primitive
			{
				namespace vertex
				{
					struct VertexWithColor
					{
						float x,y,z;
						D3DCOLOR color;

						static const DWORD flexibleVectorFormat;
					};
				}

				class Primitive;//所有Primitive必须继承的基类  用于提供统一的void render()方法

				template<typename Vertex>
				class LineList
				{
				private:
					UINT vertexSize;
					IDirect3DDevice9 *device;
					IDirect3DVertexBuffer9 *d3dVertexBuffer;

					D3DXVECTOR3 center;
					D3DXVECTOR3 translation;
					D3DXVECTOR3 rotation;
					D3DXVECTOR3 scale;
				public:
					LineList(IDirect3DDevice9 *device,
						std::list<std::pair<Vertex,Vertex>> lineList,
						DWORD usage = D3DUSAGE_WRITEONLY,
						D3DPOOL pool = D3DPOOL_MANAGED
						)
					{
						this->center = D3DXVECTOR3(0,0,0);
						this->translation = D3DXVECTOR3(0,0,0);
						this->rotation = D3DXVECTOR3(0,0,0);
						this->scale = D3DXVECTOR3(1.0f,1.0f,1.0f);

						this->device = device;

						this->vertexSize = 2*(UINT)lineList.size();
						//×2是因为这个list是元素为std::pair<Vertex,Vertex>的list
						
						device->CreateVertexBuffer(
							(this->vertexSize)*sizeof(Vertex),
							usage,Vertex::flexibleVectorFormat,
							pool,&(this->d3dVertexBuffer),0);
						
						//向资源管理中添加这个资源，以在帧结束时自动释放
						Direct3D::v9::resource::OneFrameLifecycleResource::getInstance()->addResource(device,this->d3dVertexBuffer);

						Vertex *vertexPointer = NULL;
						this->d3dVertexBuffer->Lock(0,0,(void **)&vertexPointer,0);

						UINT index = 0;
						for(std::list<std::pair<Vertex,Vertex>>::iterator lineListIterator = lineList.begin();
							lineListIterator != lineList.end();lineListIterator ++
							)
						{
							*(vertexPointer + index) = lineListIterator->first;
							++index;
							*(vertexPointer + index) = lineListIterator->second;
							++index;
						}
						this->d3dVertexBuffer->Unlock();
					}

					void render()
					{
						//apply transform
						//0.apply Translation
						Vertex *vertexPointer = NULL;
						this->d3dVertexBuffer->Lock(0,0,(void **)&vertexPointer,0);

						for(UINT index = 0;index < this->vertexSize;index ++)
						{
							vertexPointer[index].x += this->translation.x;
							vertexPointer[index].y += this->translation.y;
							vertexPointer[index].z += this->translation.z;
						}
						this->center += this->translation;//平移中心

						//1.apply rotation
						for(UINT index = 0;index < this->vertexSize;index ++)
						{
							D3DXVECTOR3 relative;//相对于旋转和缩放中心的相对矢量
							relative.x = vertexPointer[index].x - this->center.x;
							relative.y = vertexPointer[index].y - this->center.y;
							relative.z = vertexPointer[index].z - this->center.z;
							//对相对矢量进行旋转
							D3DXVECTOR3 rotated = Direct3D::v9::resource::vector::rotate(relative,this->rotation);

							vertexPointer[index].x = this->center.x + rotated.x;
							vertexPointer[index].y = this->center.y + rotated.y;
							vertexPointer[index].z = this->center.z + rotated.z;
						}

						//2.apply Scale
						for(UINT index = 0;index < this->vertexSize;index ++)
						{
							D3DXVECTOR3 relative;//相对于旋转和缩放中心的相对矢量
							relative.x = vertexPointer[index].x - this->center.x;
							relative.y = vertexPointer[index].y - this->center.y;
							relative.z = vertexPointer[index].z - this->center.z;

							vertexPointer[index].x = this->center.x + relative.x*this->scale.x;
							vertexPointer[index].y = this->center.y + relative.y*this->scale.y;
							vertexPointer[index].z = this->center.z + relative.z*this->scale.z;
						}
						this->d3dVertexBuffer->Unlock();

						//3.render
						this->device->SetStreamSource(0,this->d3dVertexBuffer,0,sizeof(Vertex));
						this->device->SetFVF(Vertex::flexibleVectorFormat);
						this->device->DrawPrimitive(D3DPT_LINELIST,0,this->vertexSize/2);
					}

					void setCenter(D3DXVECTOR3 center)
					{
						this->center = center;
					}
					void setTranslation(D3DXVECTOR3 translation)
					{
						this->translation = translation;
					}
					void setRotation(D3DXVECTOR3 rotation)
					{
						this->rotation = rotation;
					}
					void setScale(D3DXVECTOR3 scale)
					{
						this->scale = scale;
					}
				};

				//在写TriangleList之前，先定义Triangle
				template<typename Vertex>
				class Triangle
				{
				private:
					std::list<Vertex> vertexList;
				public:
					//重载构造函数
					Triangle(Vertex A,Vertex B,Vertex C)
					{
						vertexList.clear();
						vertexList.push_back(A);
						vertexList.push_back(B);
						vertexList.push_back(C);
					}
					//复制构造函数
					Triangle(const Triangle<Vertex> &triangle)
					{
						this->vertexList = triangle.getVertexList();
					}

					std::list<Vertex> getVertexList()
					{
						return this->vertexList;
					}
				};

				template<typename Vertex>
				class TriangleList
				{
				private:
					UINT vertexSize;
					IDirect3DDevice9 *device;
					IDirect3DVertexBuffer9 *d3dVertexBuffer;

					D3DXVECTOR3 center;
					D3DXVECTOR3 translation;
					D3DXVECTOR3 rotation;
					D3DXVECTOR3 scale;
				public:
					TriangleList(IDirect3DDevice9 *device,
						std::list<Triangle<Vertex>> triangleList,
						DWORD usage = D3DUSAGE_WRITEONLY,
						D3DPOOL pool = D3DPOOL_MANAGED
						)
					{
						this->center = D3DXVECTOR3(0,0,0);
						this->translation = D3DXVECTOR3(0,0,0);
						this->rotation = D3DXVECTOR3(0,0,0);
						this->scale = D3DXVECTOR3(1.0f,1.0f,1.0f);

						this->device = device;

						this->vertexSize = 3*(UINT)triangleList.size();
						//×3是因为这个list是元素为Triangle的list

						device->CreateVertexBuffer(
							(this->vertexSize)*sizeof(Vertex),
							usage,Vertex::flexibleVectorFormat,
							pool,&(this->d3dVertexBuffer),0);

						//向资源管理中添加这个资源，以在帧结束时自动释放
						Direct3D::v9::resource::OneFrameLifecycleResource::getInstance()->addResource(device,this->d3dVertexBuffer);

						Vertex *vertexPointer = NULL;
						this->d3dVertexBuffer->Lock(0,0,(void **)&vertexPointer,0);

						UINT index = 0;
						for(std::list<Triangle<Vertex>>::iterator triangleListIterator = triangleList.begin();
							triangleListIterator != triangleList.end();triangleListIterator ++
							)
						{
							std::list<Vertex> currentTriangleVertexList = triangleListIterator->getVertexList();

							for(std::list<Vertex>::iterator vertexListIterator = currentTriangleVertexList.begin();
								vertexListIterator != currentTriangleVertexList.end();vertexListIterator++
								)
							{
								*(vertexPointer + index) = *vertexListIterator;
								++index;
							}
						}
						this->d3dVertexBuffer->Unlock();
					}

					void render()
					{
						//apply transform
						//0.apply Translation
						Vertex *vertexPointer = NULL;
						this->d3dVertexBuffer->Lock(0,0,(void **)&vertexPointer,0);

						for(UINT index = 0;index < this->vertexSize;index ++)
						{
							vertexPointer[index].x += this->translation.x;
							vertexPointer[index].y += this->translation.y;
							vertexPointer[index].z += this->translation.z;
						}
						this->center += this->translation;//平移中心

						//1.apply rotation
						for(UINT index = 0;index < this->vertexSize;index ++)
						{
							D3DXVECTOR3 relative;//相对于旋转和缩放中心的相对矢量
							relative.x = vertexPointer[index].x - this->center.x;
							relative.y = vertexPointer[index].y - this->center.y;
							relative.z = vertexPointer[index].z - this->center.z;
							//对相对矢量进行旋转
							D3DXVECTOR3 rotated = Direct3D::v9::resource::vector::rotate(relative,this->rotation);

							vertexPointer[index].x = this->center.x + rotated.x;
							vertexPointer[index].y = this->center.y + rotated.y;
							vertexPointer[index].z = this->center.z + rotated.z;
						}

						//2.apply Scale
						for(UINT index = 0;index < this->vertexSize;index ++)
						{
							D3DXVECTOR3 relative;//相对于旋转和缩放中心的相对矢量
							relative.x = vertexPointer[index].x - this->center.x;
							relative.y = vertexPointer[index].y - this->center.y;
							relative.z = vertexPointer[index].z - this->center.z;

							vertexPointer[index].x = this->center.x + relative.x*this->scale.x;
							vertexPointer[index].y = this->center.y + relative.y*this->scale.y;
							vertexPointer[index].z = this->center.z + relative.z*this->scale.z;
						}
						this->d3dVertexBuffer->Unlock();

						//3.render
						this->device->SetStreamSource(0,this->d3dVertexBuffer,0,sizeof(Vertex));
						this->device->SetFVF(Vertex::flexibleVectorFormat);
						//按TriangleList绘制，所以图元数量当然是Vertex数量的三分之一
						this->device->DrawPrimitive(D3DPT_TRIANGLELIST,0,this->vertexSize/3);
					}

					void setCenter(D3DXVECTOR3 center)
					{
						this->center = center;
					}
					void setTranslation(D3DXVECTOR3 translation)
					{
						this->translation = translation;
					}
					void setRotation(D3DXVECTOR3 rotation)
					{
						this->rotation = rotation;
					}
					void setScale(D3DXVECTOR3 scale)
					{
						this->scale = scale;
					}
				};
			}

			


			namespace mesh
			{
				//声明 
				namespace text
				{
					class TextMesh;
				}
				class MeshFactory;

				class Mesh
				{
				public:
					Mesh();//在Visual Studio 2005中不支持C++ 11中的=delete定义

					//获得拷贝的Mesh,FVF直接按照原FVF
					ID3DXMesh *getClonedMesh();
					//不直接绘制子集
					//virtual HRESULT DrawSubset(DWORD attributeId = 0);

				private:
					//使用Direct 3D内置的Mesh指针创建Mesh
					//只能由友元类访问
					Mesh(ID3DXMesh *d3dxMesh,DWORD FVF,IDirect3DDevice9 *device);

					//MeshFactory must be the friend class of this class
					//so that the FACTORY can generate product
					friend class MeshFactory;

					DWORD flexibleVertexFormat;
					ID3DXMesh *d3dxMesh;
					IDirect3DDevice9 *device;
				};

				class MeshFactory
				{
				public:
					//Mesh的工厂类，可以创建Mesh
					//注意，只有MeshFactory应该被声明为“原材料”和“产品”的友元类
					//拥有完全控制权才能生产“产品”
					//不应该在Mesh中声明任何一个产品为友元类
					//否则产品整个类就可以访问
					static text::TextMesh createTextMesh(IDirect3DDevice9 *device,std::string content,std::string font,int weight = 200);
				};


				namespace text
				{
					struct TextVertex
					{
						float x,y,z;
						float nx,ny,nz;
						//DWORD Vertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL对应的顶点格式
						//之所以知道从Text中获取的是这样的顶点格式，
						//就是调试输出了18也就是0x12，所以就是D3DFVF_XYZ | D3DFVF_NORMAL
					};

					class TextMesh
					{
					private:
						//指定MeshFactory为工厂类，使得MeshFactory可以创建TextMesh
						friend class Direct3D::v9::resource::mesh::MeshFactory;

						Direct3D::v9::resource::mesh::Mesh mesh;

						//经过反复考虑，center是不需要定义的，因为render的函数是Pure的
						//D3DXVECTOR3 center,translation,rotation;
						D3DXVECTOR3 translation,rotation;
						//需要定义一个单独的Cube类来标识吗？
						//不，应该将scale和边长组合成一个内部类，统一处理
						class Scale
						{
						private:
							D3DXVECTOR3 scale,sideLength;
						public:
							//Scale() = delete;
							Scale();//Visual Studio 2005不支持C++ 11特性，所以不能用 = delete
							Scale(D3DXVECTOR3 sideLength,D3DXVECTOR3 scale = D3DXVECTOR3(1.0f,1.0f,1.0f));
							D3DXVECTOR3 setByScale(D3DXVECTOR3 scale);
							D3DXVECTOR3 setBySideLength(D3DXVECTOR3 sideLength);
							D3DXVECTOR3 getScale();
							//根据输入的边长求出对应的scale;
							D3DXVECTOR3 getScale(D3DXVECTOR3 sideLength);
							D3DXVECTOR3 getSideLength();
							//根据输入的scale求出对应的边长
							D3DXVECTOR3 getSideLength(D3DXVECTOR3 scale);
						} scale;
						TextMesh(Direct3D::v9::resource::mesh::Mesh mesh,D3DXVECTOR3 sideLength);
					public:
						~TextMesh();

						static int convertStringToWideChar(std::string toConvert,LPWSTR *result);
						static int convertMultiByteToWideChar(const char *toConvert,LPWSTR *result);

						void setTranslation(D3DXVECTOR3 translation);
						void setRotation(D3DXVECTOR3 rotation);
						void setScale(D3DXVECTOR3 scale);
						void setSideLength(D3DXVECTOR3 sideLength,bool keepAspectRatio = true);
						//之所以要传递device，是因为要复制原有的Mesh，不在原始生成（并平移到原点）的Mesh上直接Draw
						void render();//渲染
					};
				}
			}
		}
	}
}