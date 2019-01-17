#include "./AttitudeIndicator.h"

#include <list>
#include <cstdlib>
#include <string>

#include <cmath>

namespace Application
{
	namespace HUD
	{
		void AttitudeIndicator::render(IDirect3DDevice9 *device)
		{
			//渲染
			//0. 生成水平线
			//每一度在空间中的长度
			double lengthPerDegree = (double)this->R/(double)this->fieldAngle;
			double pitch = this->pitch*lengthPerDegree;//获得俯仰角在空间中的长度

			struct {INT16 upper,lower;} divBound;

			divBound.lower = pitch-this->R<0
				?(INT16)((pitch-(double)this->R)/(lengthPerDegree*(double)this->degreePerDivision))
				:(INT16)((pitch-(double)this->R)/(lengthPerDegree*(double)this->degreePerDivision)+1);
			divBound.upper = pitch+this->R>0
				?(INT16)((pitch+(double)this->R)/(lengthPerDegree*(double)this->degreePerDivision))
				:(INT16)((pitch+(double)this->R)/(lengthPerDegree*(double)this->degreePerDivision)-1);

			//处理边界情况…………暂时先这样处理吧
			if(((pitch-(double)this->R)/(lengthPerDegree*(double)this->degreePerDivision)) == 
				(INT16)((pitch-(double)this->R)/(lengthPerDegree*(double)this->degreePerDivision))
				)
			{
				divBound.lower++;
			}
			if(((pitch+(double)this->R)/(lengthPerDegree*(double)this->degreePerDivision)) == 
				(INT16)((pitch+(double)this->R)/(lengthPerDegree*(double)this->degreePerDivision))
				)
			{
				divBound.upper--;
			}

			using Direct3D::v9::resource::primitive::vertex::VertexWithColor;


			std::list<std::pair<struct VertexWithColor,struct VertexWithColor>> horizontalLineVertexList;
			for(INT16 divIndex = divBound.lower;divIndex <= divBound.upper;divIndex++)
			{
				//遍历生成各条水平线
				double halfLength = sqrt(
					(double)this->R*(double)this->R-
					pow((pitch - (double)divIndex*(double)(lengthPerDegree*this->degreePerDivision)),2)
					);

				//地平线要长一些,半长度占总半径的60%
				if(divIndex == 0) halfLength = halfLength < 0.6*this->R?halfLength:0.6*this->R;
				else halfLength = halfLength < 0.4*this->R?halfLength:0.4*this->R;

				/*
				struct VertexWithColor left,right;
				left.color = D3DCOLOR_XRGB(0,0xff,0);
				left.x = (float)(-halfLength);
				left.y = (float)(-pitch + divIndex*(double)(lengthPerDegree*this->degreePerDivision));
				left.z = (float)this->Z;

				right.color = D3DCOLOR_XRGB(0,0xff,0);
				right.x = (float)halfLength;
				right.y = left.y;
				right.z = (float)this->Z;
				*/
				struct VertexWithColor vertexArray[4];//这是在栈上开辟的，自动管理，所以不用释放

				vertexArray[0].x = (float)(-halfLength);
				vertexArray[1].x = (float)(-0.2)*(float)this->R;
				vertexArray[2].x = (float)0.2*(float)this->R;
				vertexArray[3].x = (float)(halfLength);

				//我只是真的不想命名了，仅此而已
				for(UINT8 aaaaaaaaa = 0;aaaaaaaaa <4;aaaaaaaaa++)
				{
					vertexArray[aaaaaaaaa].color = D3DCOLOR_XRGB(0,0xff,0);
					vertexArray[aaaaaaaaa].y = (float)(-pitch + divIndex*(double)(lengthPerDegree*this->degreePerDivision));
					vertexArray[aaaaaaaaa].z = (float)this->Z;
				}

				horizontalLineVertexList.push_back(std::pair<struct VertexWithColor,struct VertexWithColor>(vertexArray[0],vertexArray[1]));
				horizontalLineVertexList.push_back(std::pair<struct VertexWithColor,struct VertexWithColor>(vertexArray[2],vertexArray[3]));

				///////////////////////////////////////////////
				//the following part is aimed at generating and rendering Text Mesh

				//首先生成文字;

				std::string currentDegreeText("");
				
				char charArray[64];
				sprintf_s(charArray,"%d",(int)(divIndex * this->degreePerDivision));
				currentDegreeText.append(charArray);



				Direct3D::v9::resource::mesh::text::TextMesh currentDegreeTextMesh = 
					Direct3D::v9::resource::mesh::MeshFactory::createTextMesh(device,currentDegreeText,std::string("Microsoft YaHei"));
				
				//因为TextMesh没有针对姿态显示仪的单独优化，所以必须单独设定Translation   其实也不难
				D3DXVECTOR3 translationWithoutRotation;
				translationWithoutRotation.x = (float)(-0.1*(float)this->R);
				translationWithoutRotation.y = (float)(-pitch + divIndex*(float)(lengthPerDegree*this->degreePerDivision));
				translationWithoutRotation.z = (float)this->Z;

				currentDegreeTextMesh.setTranslation(Direct3D::v9::resource::vector::rotate(
					translationWithoutRotation, D3DXVECTOR3(0.0f,0.0f,((float)this->roll/180)*D3DX_PI)));
				currentDegreeTextMesh.setScale(D3DXVECTOR3(80,80,0.001f));
				currentDegreeTextMesh.setRotation(D3DXVECTOR3(0.0f,0.0f,((float)this->roll/180)*D3DX_PI));
				currentDegreeTextMesh.render();
			}

			//1.用于指示滚转的小三角形
			struct VertexWithColor tempVertexArray[4];
			tempVertexArray[0].y = 0-(float)this->R;
			tempVertexArray[1].y = 0-(float)this->R;
			tempVertexArray[2].y = (float)-1.1*(float)this->R;
			tempVertexArray[0].x = (float)-0.05*(float)this->R;
			tempVertexArray[1].x = (float)0.05*(float)this->R;
			tempVertexArray[2].x = 0;
			tempVertexArray[0].z = (float)this->Z;
			tempVertexArray[1].z = (float)this->Z;
			tempVertexArray[2].z = (float)this->Z;

			horizontalLineVertexList.push_back(
				std::pair<struct VertexWithColor,struct VertexWithColor>(tempVertexArray[0],tempVertexArray[1])
				);
			horizontalLineVertexList.push_back(
				std::pair<struct VertexWithColor,struct VertexWithColor>(tempVertexArray[1],tempVertexArray[2])
				);
			horizontalLineVertexList.push_back(
				std::pair<struct VertexWithColor,struct VertexWithColor>(tempVertexArray[2],tempVertexArray[0])
				);



			Direct3D::v9::resource::primitive::LineList<struct VertexWithColor> horizontalLineList = 
				Direct3D::v9::resource::primitive::LineList<struct VertexWithColor>(device,horizontalLineVertexList);
			horizontalLineList.setRotation(D3DXVECTOR3(0,0,((float)this->roll/180)*D3DX_PI));
			horizontalLineList.render();

			//2.中间的+要单独生成并渲染，否则…………会跟随着滚转一起旋转
			tempVertexArray[0].color = D3DCOLOR_XRGB(0,0xff,0);
			tempVertexArray[1].color = D3DCOLOR_XRGB(0,0xff,0);
			tempVertexArray[2].color = D3DCOLOR_XRGB(0,0xff,0);
			tempVertexArray[3].color = D3DCOLOR_XRGB(0,0xff,0);

			tempVertexArray[0].x = (float)(-0.05*(float)this->R);
			tempVertexArray[1].x = (float)(0.05*(float)this->R);
			tempVertexArray[2].y = (float)(-0.05*(float)this->R);
			tempVertexArray[3].y = (float)(0.05*(float)this->R);

			tempVertexArray[0].y = 0;
			tempVertexArray[1].y = 0;
			tempVertexArray[2].x = 0;
			tempVertexArray[3].x = 0;

			//我只是真的不想命名了，仅此而已
			for(UINT8 aaaaaaaaa = 0;aaaaaaaaa <4;aaaaaaaaa++)
			{
				tempVertexArray[aaaaaaaaa].z = (float)this->Z;
				tempVertexArray[aaaaaaaaa].color = D3DCOLOR_XRGB(0,0xff,0);
			}

			std::list<std::pair<struct VertexWithColor,struct VertexWithColor>> aaaaaaaaaaaaaa;
			//没错，我又懒得命名了
			aaaaaaaaaaaaaa.clear();
			aaaaaaaaaaaaaa.push_back(std::pair<struct VertexWithColor,struct VertexWithColor>(tempVertexArray[0],tempVertexArray[1]));
			aaaaaaaaaaaaaa.push_back(std::pair<struct VertexWithColor,struct VertexWithColor>(tempVertexArray[2],tempVertexArray[3]));

			//3.下面的圆弧
			for(UINT i = 0;180+45+0.2*(float)i<270+45;i++)
			{
				//绘制从i到(i+1)的线段
				tempVertexArray[0].x = (float)1.1*(float)this->R*cos((float)((180+45+0.2*i)*D3DX_PI)/180);
				tempVertexArray[0].y = (float)1.1*(float)this->R*sin((float)((180+45+0.2*i)*D3DX_PI)/180);
				tempVertexArray[0].z = (float)this->Z;

				tempVertexArray[1].x = (float)1.1*(float)this->R*cos((float)((180+45+0.2*(i+1))*D3DX_PI)/180);
				tempVertexArray[1].y = (float)1.1*(float)this->R*sin((float)((180+45+0.2*(i+1))*D3DX_PI)/180);
				tempVertexArray[1].z = (float)this->Z;

				aaaaaaaaaaaaaa.push_back(
					std::pair<struct VertexWithColor,struct VertexWithColor>(tempVertexArray[0],tempVertexArray[1])
					);
			}
			for(UINT i = 180+45;i <= 270+45;i += 5)
			{
				tempVertexArray[0].x = (float)1.1*(float)this->R*cos(((float)i*D3DX_PI)/180);
				tempVertexArray[0].y = (float)1.1*(float)this->R*sin(((float)i*D3DX_PI)/180);
				tempVertexArray[0].z = (float)this->Z;

				tempVertexArray[1].x = (float)1.15*(float)this->R*cos(((float)i*D3DX_PI)/180);
				tempVertexArray[1].y = (float)1.15*(float)this->R*sin(((float)i*D3DX_PI)/180);
				tempVertexArray[1].z = (float)this->Z;

				aaaaaaaaaaaaaa.push_back(
					std::pair<struct VertexWithColor,struct VertexWithColor>(tempVertexArray[0],tempVertexArray[1])
					);
			}

			Direct3D::v9::resource::primitive::LineList<struct VertexWithColor>(device,aaaaaaaaaaaaaa).render();

		}
		//the following methods are setters
		AttitudeIndicator::AttitudeIndicator(UINT8 fieldAngle, UINT8 degreePerDivision, UINT R /* = 500 */,UINT Z /* = 1000 */)
		{
			this->fieldAngle = fieldAngle;
			this->degreePerDivision = degreePerDivision;
			this->R = R;
			this->Z = Z;

			this->pitch = 0;
			this->roll = 0;
		}
		void AttitudeIndicator::setPitch(double pitch)
		{
			this->pitch = pitch;
		}
		void AttitudeIndicator::setRoll(double roll)
		{
			this->roll = roll;
		}
	}
}