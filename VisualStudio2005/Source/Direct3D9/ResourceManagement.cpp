#include "Direct3D9.h"

namespace Direct3D
{
	namespace v9
	{
		namespace resource
		{
			OneFrameLifecycleResource *OneFrameLifecycleResource::instance = NULL;
			OneFrameLifecycleResource::OneFrameLifecycleResource()
			{
				this->mesh.clear();
			}
			OneFrameLifecycleResource::~OneFrameLifecycleResource()
			{
				if(instance != NULL)
				{
					delete instance;
					instance = NULL;
				}
			}
			OneFrameLifecycleResource *OneFrameLifecycleResource::getInstance()
			{
				if(instance == NULL) instance = new OneFrameLifecycleResource;
				while(instance == NULL);//�ȴ�����
				return instance;
			}
			void OneFrameLifecycleResource::clear(IDirect3DDevice9 *device)
			{
				//release all the mesh
				std::map<IDirect3DDevice9 *,std::list<ID3DXMesh *>>::iterator meshMapIterator = this->mesh.find(device);
				if(meshMapIterator != this->mesh.end())
				{
					//std::list<ID3DXMesh *> meshList = meshMapIterator->second;
					for(std::list<ID3DXMesh *>::iterator meshListIterator = meshMapIterator->second.begin();
						meshListIterator != meshMapIterator->second.end();meshListIterator++)
					{
						//ID3DXMesh *currentMesh = (ID3DXMesh *)*meshListIterator;
						(*meshListIterator)->Release();
					}
					//����б�
					meshMapIterator->second.clear();	
				}
				//release all vertex buffer
				std::map<IDirect3DDevice9 *,std::list<IDirect3DVertexBuffer9 *>>::iterator vertexBufferMapIterator = this->vertexBuffer.find(device);
				if(vertexBufferMapIterator != this->vertexBuffer.end())
				{
					for(std::list<IDirect3DVertexBuffer9 *>::iterator vertexBufferIterator = vertexBufferMapIterator->second.begin();
						vertexBufferIterator != vertexBufferMapIterator->second.end();vertexBufferIterator++)
					{
						//ID3DXMesh *currentMesh = (ID3DXMesh *)*meshListIterator;
						(*vertexBufferIterator)->Release();
					}
					//����б�
					vertexBufferMapIterator->second.clear();	
				}
			}

			UINT64 OneFrameLifecycleResource::addResource(IDirect3DDevice9 *device,ID3DXMesh *mesh)
			{
				
				std::map<IDirect3DDevice9 *,std::list<ID3DXMesh *>>::iterator meshMapIterator = this->mesh.find(device);
				if(meshMapIterator == this->mesh.end())
				{
					//������map����ʱľ�У�����Ҫ����
					std::list<ID3DXMesh *> meshList;
					meshList.push_back(mesh);

					this->mesh[device] = meshList;
					return (UINT64)meshList.size();
				}
				else
				{
					//�б����Ѿ�����
					meshMapIterator->second.push_back(mesh);
					return (UINT64)meshMapIterator->second.size();
				}
			}
			UINT64 OneFrameLifecycleResource::addResource(IDirect3DDevice9 *device,IDirect3DVertexBuffer9 *vertexBuffer)
			{

				std::map<IDirect3DDevice9 *,std::list<IDirect3DVertexBuffer9 *>>::iterator vertexBufferMapIterator = this->vertexBuffer.find(device);
				if(vertexBufferMapIterator == this->vertexBuffer.end())
				{
					//������map����ʱľ�У�����Ҫ����
					std::list<IDirect3DVertexBuffer9 *> vertexBufferList;
					vertexBufferList.push_back(vertexBuffer);

					this->vertexBuffer[device] = vertexBufferList;
					return (UINT64)vertexBufferList.size();
				}
				else
				{
					//�б����Ѿ�����
					vertexBufferMapIterator->second.push_back(vertexBuffer);
					return (UINT64)vertexBufferMapIterator->second.size();
				}
			}
		}
	}
}