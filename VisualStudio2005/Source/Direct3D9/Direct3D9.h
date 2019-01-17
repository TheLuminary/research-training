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
				//������
			private:
				OneFrameLifecycleResource();
				~OneFrameLifecycleResource();
				static OneFrameLifecycleResource *instance;
			public:
				static OneFrameLifecycleResource *getInstance();
				//���ϵĲ�������ʵ�ֵ���ģʽ

				void clear(IDirect3DDevice9 *device);//������һ֡���������

				//�����µĴ�С
				UINT64 addResource(IDirect3DDevice9 *device,ID3DXMesh *mesh);
				//�����µĴ�С
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

				class Primitive;//����Primitive����̳еĻ���  �����ṩͳһ��void render()����

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
						//��2����Ϊ���list��Ԫ��Ϊstd::pair<Vertex,Vertex>��list
						
						device->CreateVertexBuffer(
							(this->vertexSize)*sizeof(Vertex),
							usage,Vertex::flexibleVectorFormat,
							pool,&(this->d3dVertexBuffer),0);
						
						//����Դ��������������Դ������֡����ʱ�Զ��ͷ�
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
						this->center += this->translation;//ƽ������

						//1.apply rotation
						for(UINT index = 0;index < this->vertexSize;index ++)
						{
							D3DXVECTOR3 relative;//�������ת���������ĵ����ʸ��
							relative.x = vertexPointer[index].x - this->center.x;
							relative.y = vertexPointer[index].y - this->center.y;
							relative.z = vertexPointer[index].z - this->center.z;
							//�����ʸ��������ת
							D3DXVECTOR3 rotated = Direct3D::v9::resource::vector::rotate(relative,this->rotation);

							vertexPointer[index].x = this->center.x + rotated.x;
							vertexPointer[index].y = this->center.y + rotated.y;
							vertexPointer[index].z = this->center.z + rotated.z;
						}

						//2.apply Scale
						for(UINT index = 0;index < this->vertexSize;index ++)
						{
							D3DXVECTOR3 relative;//�������ת���������ĵ����ʸ��
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

				//��дTriangleList֮ǰ���ȶ���Triangle
				template<typename Vertex>
				class Triangle
				{
				private:
					std::list<Vertex> vertexList;
				public:
					//���ع��캯��
					Triangle(Vertex A,Vertex B,Vertex C)
					{
						vertexList.clear();
						vertexList.push_back(A);
						vertexList.push_back(B);
						vertexList.push_back(C);
					}
					//���ƹ��캯��
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
						//��3����Ϊ���list��Ԫ��ΪTriangle��list

						device->CreateVertexBuffer(
							(this->vertexSize)*sizeof(Vertex),
							usage,Vertex::flexibleVectorFormat,
							pool,&(this->d3dVertexBuffer),0);

						//����Դ��������������Դ������֡����ʱ�Զ��ͷ�
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
						this->center += this->translation;//ƽ������

						//1.apply rotation
						for(UINT index = 0;index < this->vertexSize;index ++)
						{
							D3DXVECTOR3 relative;//�������ת���������ĵ����ʸ��
							relative.x = vertexPointer[index].x - this->center.x;
							relative.y = vertexPointer[index].y - this->center.y;
							relative.z = vertexPointer[index].z - this->center.z;
							//�����ʸ��������ת
							D3DXVECTOR3 rotated = Direct3D::v9::resource::vector::rotate(relative,this->rotation);

							vertexPointer[index].x = this->center.x + rotated.x;
							vertexPointer[index].y = this->center.y + rotated.y;
							vertexPointer[index].z = this->center.z + rotated.z;
						}

						//2.apply Scale
						for(UINT index = 0;index < this->vertexSize;index ++)
						{
							D3DXVECTOR3 relative;//�������ת���������ĵ����ʸ��
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
						//��TriangleList���ƣ�����ͼԪ������Ȼ��Vertex����������֮һ
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
				//���� 
				namespace text
				{
					class TextMesh;
				}
				class MeshFactory;

				class Mesh
				{
				public:
					Mesh();//��Visual Studio 2005�в�֧��C++ 11�е�=delete����

					//��ÿ�����Mesh,FVFֱ�Ӱ���ԭFVF
					ID3DXMesh *getClonedMesh();
					//��ֱ�ӻ����Ӽ�
					//virtual HRESULT DrawSubset(DWORD attributeId = 0);

				private:
					//ʹ��Direct 3D���õ�Meshָ�봴��Mesh
					//ֻ������Ԫ�����
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
					//Mesh�Ĺ����࣬���Դ���Mesh
					//ע�⣬ֻ��MeshFactoryӦ�ñ�����Ϊ��ԭ���ϡ��͡���Ʒ������Ԫ��
					//ӵ����ȫ����Ȩ������������Ʒ��
					//��Ӧ����Mesh�������κ�һ����ƷΪ��Ԫ��
					//�����Ʒ������Ϳ��Է���
					static text::TextMesh createTextMesh(IDirect3DDevice9 *device,std::string content,std::string font,int weight = 200);
				};


				namespace text
				{
					struct TextVertex
					{
						float x,y,z;
						float nx,ny,nz;
						//DWORD Vertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL��Ӧ�Ķ����ʽ
						//֮����֪����Text�л�ȡ���������Ķ����ʽ��
						//���ǵ��������18Ҳ����0x12�����Ծ���D3DFVF_XYZ | D3DFVF_NORMAL
					};

					class TextMesh
					{
					private:
						//ָ��MeshFactoryΪ�����࣬ʹ��MeshFactory���Դ���TextMesh
						friend class Direct3D::v9::resource::mesh::MeshFactory;

						Direct3D::v9::resource::mesh::Mesh mesh;

						//�����������ǣ�center�ǲ���Ҫ����ģ���Ϊrender�ĺ�����Pure��
						//D3DXVECTOR3 center,translation,rotation;
						D3DXVECTOR3 translation,rotation;
						//��Ҫ����һ��������Cube������ʶ��
						//����Ӧ�ý�scale�ͱ߳���ϳ�һ���ڲ��࣬ͳһ����
						class Scale
						{
						private:
							D3DXVECTOR3 scale,sideLength;
						public:
							//Scale() = delete;
							Scale();//Visual Studio 2005��֧��C++ 11���ԣ����Բ����� = delete
							Scale(D3DXVECTOR3 sideLength,D3DXVECTOR3 scale = D3DXVECTOR3(1.0f,1.0f,1.0f));
							D3DXVECTOR3 setByScale(D3DXVECTOR3 scale);
							D3DXVECTOR3 setBySideLength(D3DXVECTOR3 sideLength);
							D3DXVECTOR3 getScale();
							//��������ı߳������Ӧ��scale;
							D3DXVECTOR3 getScale(D3DXVECTOR3 sideLength);
							D3DXVECTOR3 getSideLength();
							//���������scale�����Ӧ�ı߳�
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
						//֮����Ҫ����device������ΪҪ����ԭ�е�Mesh������ԭʼ���ɣ���ƽ�Ƶ�ԭ�㣩��Mesh��ֱ��Draw
						void render();//��Ⱦ
					};
				}
			}
		}
	}
}