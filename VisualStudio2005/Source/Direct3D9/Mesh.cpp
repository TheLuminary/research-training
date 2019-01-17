#pragma once
#include "Direct3D9.h"

namespace Direct3D
{
	namespace v9
	{
		namespace resource
		{
			namespace mesh
			{
				namespace text
				{
					void TextMesh::setTranslation(D3DXVECTOR3 translation)
					{
						this->translation = translation;
					}
					void TextMesh::setRotation(D3DXVECTOR3 rotation)
					{
						this->rotation = rotation;
					}
					void TextMesh::setScale(D3DXVECTOR3 scale)
					{
						this->scale.setByScale(scale);
					}
					void TextMesh::setSideLength(D3DXVECTOR3 sideLength,bool keepAspectRatio /* = true */)
					{
						if(keepAspectRatio)
						{
							//保持文字长宽比,z根据输入的边长确定
							D3DXVECTOR3 temp1 = this->scale.getScale(sideLength);
							float temp2 = temp1.x<temp1.y?temp1.x:temp1.y;
							this->scale.setByScale(D3DXVECTOR3(temp2,temp2,temp1.z));
						}
						else
						{
							this->scale.setBySideLength(sideLength);
						}
					}
					void TextMesh::render()
					{
						//之所以要传递device，是因为要复制原有的Mesh，不在原始生成（并平移到原点）的Mesh上直接Draw
						//以后都是对这个进行操作，不再改变原始的
						ID3DXMesh *meshToRender = this->mesh.getClonedMesh();
						//todo

						//中心根据setTranslation确定
						D3DXVECTOR3 center = this->translation;
						//0.平移
						struct Direct3D::v9::resource::mesh::text::TextVertex *pVertex = NULL;
						meshToRender->LockVertexBuffer(0,(void **)&pVertex);
						for(DWORD i = 0;i < meshToRender->GetNumVertices();i++)
						{
							pVertex[i].x += this->translation.x;
							pVertex[i].y += this->translation.y;
							pVertex[i].z += this->translation.z;
							//暂时不改变顶点法线试试效果
						}

						//1.旋转(todo)
						//旋转的时候要改变法向量，所以就比较复杂，暂时留着
						//emmmmmm……我做了一下抽象，应该没问题了吧
						for(DWORD i = 0;i < meshToRender->GetNumVertices();i++)
						{
							D3DXVECTOR3 relative;//relative to center
							relative.x = pVertex[i].x - center.x;
							relative.y = pVertex[i].y - center.y;
							relative.z = pVertex[i].z - center.z;

							D3DXVECTOR3 rotated = Direct3D::v9::resource::vector::rotate(relative,this->rotation);
							pVertex[i].x = center.x + rotated.x;
							pVertex[i].y = center.y + rotated.y;
							pVertex[i].z = center.z + rotated.z;

							//下面要旋转法向量
							D3DXVECTOR3 n = D3DXVECTOR3(pVertex[i].nx,pVertex[i].ny,pVertex[i].nz);
							rotated = Direct3D::v9::resource::vector::rotate(n,this->rotation);
							pVertex[i].nx = rotated.x;
							pVertex[i].ny = rotated.y;
							pVertex[i].nz = rotated.z;
						}
						//2.缩放，缩放不改变中心
						for(DWORD i = 0;i < meshToRender->GetNumVertices();i++)
						{
							D3DXVECTOR3 relative;
							relative.x = pVertex[i].x - center.x;
							relative.y = pVertex[i].y - center.y;
							relative.z = pVertex[i].z - center.z;
							pVertex[i].x = center.x + relative.x*this->scale.getScale().x;
							pVertex[i].y = center.y + relative.y*this->scale.getScale().y;
							pVertex[i].z = center.z + relative.z*this->scale.getScale().z;
							//暂时不改变顶点法线
							//因为真的不知道顶点法线会影响什么
						}
						meshToRender->UnlockVertexBuffer();
						meshToRender->DrawSubset(0);
					}

					//下面的部分都是不会出什么错的
					////////////////////////////////////////////////////////////////////////////////
					TextMesh::Scale::Scale()
					{
						//empty default Constructor
					}
					TextMesh::Scale::Scale(D3DXVECTOR3 sideLength,D3DXVECTOR3 scale /* = D3DXVECTOR3(1.0f,1.0f,1.0f) */)
					{
						this->sideLength = sideLength;
						this->scale = scale;
					}
					D3DXVECTOR3 TextMesh::Scale::setByScale(D3DXVECTOR3 scale)
					{
						this->sideLength.x *= scale.x;
						this->sideLength.y *= scale.y;
						this->sideLength.z *= scale.z;

						this->scale = scale;
						return this->sideLength;
					}
					D3DXVECTOR3 TextMesh::Scale::setBySideLength(D3DXVECTOR3 sideLength)
					{
						//以下三个式子等号右边的最后那个括号相当于取消原来的scale，将长度还原到一倍长度
						this->scale.x = sideLength.x / (this->sideLength.x / this->scale.x);
						this->scale.y = sideLength.y / (this->sideLength.y / this->scale.y);
						this->scale.z = sideLength.z / (this->sideLength.z / this->scale.z);

						this->sideLength = sideLength;
						return this->scale;
					}
					D3DXVECTOR3 TextMesh::Scale::getScale()
					{
						return this->scale;
					}
					D3DXVECTOR3 TextMesh::Scale::getScale(D3DXVECTOR3 sideLength)
					{
						//根据输入的边长求出对应的scale;
						D3DXVECTOR3 result;
						result.x = 	sideLength.x / (this->sideLength.x/this->scale.x);
						result.y = 	sideLength.y / (this->sideLength.y/this->scale.y);
						result.z = 	sideLength.z / (this->sideLength.z/this->scale.z);

						return result;
					}
					D3DXVECTOR3 TextMesh::Scale::getSideLength()
					{
						return this->sideLength;
					}
					D3DXVECTOR3 TextMesh::Scale::getSideLength(D3DXVECTOR3 scale)
					{
						//根据输入的scale求出对应的边长
						D3DXVECTOR3 result;
						result.x = 	scale.x * (this->sideLength.x/this->scale.x);
						result.y = 	scale.y * (this->sideLength.y/this->scale.y);
						result.z =	scale.z * (this->sideLength.z/this->scale.z);
						return result;
					}
					//辅助转换函数
					int TextMesh::convertStringToWideChar(std::string toConvert,LPWSTR *result)
					{
						return convertMultiByteToWideChar(toConvert.c_str(),result);
					}
					int TextMesh::convertMultiByteToWideChar(const char *toConvert,LPWSTR *result)
					{
						int resultLength = MultiByteToWideChar(CP_ACP,0,toConvert,-1,NULL,0);
						*result = (wchar_t*)malloc( resultLength*sizeof(wchar_t) );
						MultiByteToWideChar(CP_ACP,0, toConvert, -1, *result, resultLength);
						return resultLength;
					}

					TextMesh::TextMesh(Direct3D::v9::resource::mesh::Mesh mesh,D3DXVECTOR3 sideLength)
					{
						this->mesh = mesh;
						this->scale = Scale(sideLength);

						this->rotation = D3DXVECTOR3(0.0f,0.0f,0.0f);
						this->translation = D3DXVECTOR3(0.0f,0.0f,0.0f);
					}
					TextMesh::~TextMesh()
					{
						//
					}
				}

				text::TextMesh MeshFactory::createTextMesh(IDirect3DDevice9 *device,std::string content,std::string font,int weight /* = 200 */)
				{
					ID3DXMesh *d3dxMesh = NULL;

					//0.创建文本网格
					LPWSTR fontName;
					text::TextMesh::convertStringToWideChar(font,&fontName);
					HFONT hFont = CreateFont(10,10,0,0,weight,0,0,0,DEFAULT_CHARSET,0,0,0,0,(LPCWSTR)fontName);
					delete[] fontName;

					LPWSTR tempContent;
					text::TextMesh::convertStringToWideChar(content,&tempContent);
					//我还是想爬得更高去看看更大的舞台
					//我想飞的更高


					//创建字体
					HDC hdc = CreateCompatibleDC(0);//创建环境
					HFONT originalFont = (HFONT)SelectObject(hdc,hFont);//将字体选入设备环境,返回上一个字体，以备重新选择
					D3DXCreateText(device,hdc,(LPCWSTR)tempContent,0.001f,//这是最小值了  再小就出Bug了
						(0.001f*0.001f*0.001f*0.001f*0.001f*0.001f*0.001f*0.001f),
						&d3dxMesh,NULL,NULL);//创建文本网格
					SelectObject(hdc,originalFont);//选择原来的字体
					DeleteObject(hFont);
					DeleteDC(hdc);

					delete[] tempContent;

					//1.进行预变换
					//将文字网格外接长方体的体积中心平移到原点
					struct {D3DXVECTOR3 leftBottom,rightTop;} bound;
					bound.rightTop.x = -(1024.0f*1024.0f*1024.0f);
					bound.rightTop.y = -(1024.0f*1024.0f*1024.0f);
					bound.rightTop.z = -(1024.0f*1024.0f*1024.0f);
					bound.leftBottom.x = 1024.0f*1024.0f*1024.0f;
					bound.leftBottom.y = 1024.0f*1024.0f*1024.0f;
					bound.leftBottom.z = 1024.0f*1024.0f*1024.0f;

					struct Direct3D::v9::resource::mesh::text::TextVertex *pVertex = NULL;
					d3dxMesh->LockVertexBuffer(0,(void **)&pVertex);
					for(DWORD i = 0;i < d3dxMesh->GetNumVertices();i++)
					{
						bound.leftBottom.x = pVertex[i].x<bound.leftBottom.x?pVertex[i].x:bound.leftBottom.x;
						bound.leftBottom.y = pVertex[i].y<bound.leftBottom.y?pVertex[i].y:bound.leftBottom.y;
						bound.leftBottom.z = pVertex[i].z<bound.leftBottom.z?pVertex[i].z:bound.leftBottom.z;

						bound.rightTop.x = pVertex[i].x>bound.rightTop.x?pVertex[i].x:bound.rightTop.x;
						bound.rightTop.y = pVertex[i].y>bound.rightTop.y?pVertex[i].y:bound.rightTop.y;
						bound.rightTop.z = pVertex[i].z>bound.rightTop.z?pVertex[i].z:bound.rightTop.z;
					}
					//获得上下限之后，应用变换
					D3DXVECTOR3 initialCenter = D3DXVECTOR3(
						(bound.rightTop.x + bound.leftBottom.x)/2,
						(bound.rightTop.y + bound.leftBottom.y)/2,
						(bound.rightTop.z + bound.leftBottom.z)/2
						);//生成文本网格时的中心
					for(DWORD i = 0;i < d3dxMesh->GetNumVertices();i++)
					{
						//进行平移，将文本网格移动到中点
						pVertex[i].x -= initialCenter.x;
						pVertex[i].y -= initialCenter.y;
						pVertex[i].z -= initialCenter.z;
					}
					d3dxMesh->UnlockVertexBuffer();

					D3DXVECTOR3 sideLength = D3DXVECTOR3(
						bound.rightTop.x - bound.leftBottom.x,
						bound.rightTop.y - bound.leftBottom.y,
						bound.rightTop.z - bound.leftBottom.z
						);

					return text::TextMesh(Mesh(d3dxMesh,D3DFVF_XYZ|D3DFVF_NORMAL,device),sideLength);
				}
				//methods in class Mesh are defined as follows
				Mesh::Mesh()
				{
					//这个方法不应该被调用
					//现在由于Visual Studio 2005不支持 = delete……所以暂时想不到什么很好的办法
					this->d3dxMesh = NULL;
					this->flexibleVertexFormat = NULL;
					this->device = NULL;

					//现在我开始理解Google的人为什么觉得C++太复杂于是造了go语言
				}

				Mesh::Mesh(ID3DXMesh *d3dxMesh,DWORD FVF,IDirect3DDevice9 *device)
				{
					this->d3dxMesh = d3dxMesh;
					//向资源管理去添加这个……mesh，以在一帧结束时释放
					Direct3D::v9::resource::OneFrameLifecycleResource::getInstance()->addResource(device,d3dxMesh);
					this->flexibleVertexFormat = FVF;
					this->device = device;
				}

				ID3DXMesh *Mesh::getClonedMesh()
				{
					ID3DXMesh *clonedMesh = NULL;
					this->d3dxMesh->CloneMeshFVF(this->d3dxMesh->GetOptions(),this->flexibleVertexFormat,this->device,&clonedMesh);
					//向资源管理去添加这个……mesh，以在一帧结束时释放
					Direct3D::v9::resource::OneFrameLifecycleResource::getInstance()->addResource(device,clonedMesh);
					return clonedMesh;
				}
			}
		}
	}
}