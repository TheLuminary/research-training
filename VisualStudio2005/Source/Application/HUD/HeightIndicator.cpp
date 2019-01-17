#include "HeightIndicator.h"

#include <cstdio>

namespace Application
{
	namespace HUD
	{
		HeightIndicator::HeightIndicator(
			UINT16 metersPerDiv,UINT8 numberOfDivisionToDisplay, 
			D3DXVECTOR3 displayCenter,float displayWidth,float displayHeight)
		{
			//���п��ﻯ
			this->metersPerDiv = metersPerDiv;
			this->numberOfDivisionToDisplay = numberOfDivisionToDisplay;
			this->displayCenter = displayCenter;
			this->displayWidth = displayWidth;
			this->displayHeight = displayHeight;

			//����ͻȻ�뵽���������������ֵ�ʱ�򣬺���Ҫ���Ǹ�����
			//displayHeight / numberOfDivisionToDisplay�õ�ÿ���ָ�ռ�����أ�Ȼ��ѡ��һ����ռ�ձ���������
		}

		void HeightIndicator::setMeters(float meters)
		{
			this->meters = meters;
		}
		void HeightIndicator::render(IDirect3DDevice9 *device)
		{
			//���meter������һ���ָ��ϵĻ���������
			struct {double lower,upper;} metersBound;
			metersBound.lower = this->meters - ((double)(this->numberOfDivisionToDisplay * this->metersPerDiv)*0.501);
			metersBound.upper = this->meters + ((double)(this->numberOfDivisionToDisplay * this->metersPerDiv)*0.501);
			//������õ��ڱ߽��Ǿ�һ�𻭣������AttitudeIndicator��ͬ  ������Ҫ��̫��

			struct {INT16 lower,upper;} divIndexBound;
			divIndexBound.lower = (INT16)(metersBound.lower / this->metersPerDiv);
			divIndexBound.upper = (INT16)(metersBound.upper / this->metersPerDiv);
			if(metersBound.lower > 0) divIndexBound.lower++;
			if(metersBound.upper < 0) divIndexBound.upper--;
			//������������ΪC/C++������û��MATLAB��ceil()��floor()������Ĳ�������һ��Util�Ŀ������

			char buffer[64] = {0};
			sprintf_s(buffer,"%.0f",this->meters);
			std::string currentHeightText("");
			currentHeightText.append(buffer);

			Direct3D::v9::resource::mesh::text::TextMesh currentText = 
				Direct3D::v9::resource::mesh::MeshFactory::createTextMesh(device,currentHeightText,std::string("Microsoft YaHei"));
			
			currentText.setTranslation(D3DXVECTOR3(
				this->displayCenter.x-(float)0.3*displayWidth,
				this->displayCenter.y+(this->displayHeight/numberOfDivisionToDisplay)*0.15f,
				this->displayCenter.z));
			currentText.setSideLength(D3DXVECTOR3(this->displayWidth*0.4f,(this->displayHeight/numberOfDivisionToDisplay)*0.2f,0.001f));
			//currentText.setScale(D3DXVECTOR3(60,60,0.000001f));
			currentText.render();

			

			std::list<std::pair<\
			struct Direct3D::v9::resource::primitive::vertex::VertexWithColor,\
			struct Direct3D::v9::resource::primitive::vertex::VertexWithColor>\
			> vertexList;

			struct Direct3D::v9::resource::primitive::vertex::VertexWithColor tempVertexArray[2];
			//�̶������Ŀ̶���
			tempVertexArray[0].x = this->displayCenter.x - (float)0.5*this->displayWidth;
			tempVertexArray[1].x = this->displayCenter.x - (float)0.1*this->displayWidth;

			tempVertexArray[0].y = this->displayCenter.y;
			tempVertexArray[1].y = this->displayCenter.y;

			tempVertexArray[0].z = this->displayCenter.z;
			tempVertexArray[1].z = this->displayCenter.z;

			tempVertexArray[0].color = D3DCOLOR_XRGB(0,0xff,0);
			tempVertexArray[1].color = D3DCOLOR_XRGB(0,0xff,0);

			vertexList.push_back(
				std::pair<
					struct Direct3D::v9::resource::primitive::vertex::VertexWithColor,
					struct Direct3D::v9::resource::primitive::vertex::VertexWithColor
				>(tempVertexArray[0],tempVertexArray[1])
				);


			for(INT16 currentDivIndex = divIndexBound.lower;
				currentDivIndex <= divIndexBound.upper;
				currentDivIndex ++)
			{
				//Ȼ����ôȷ�������ߵ������ء���
				//�Ѹ߶�metersת����divΪ��λ
				float y = 0-(this->displayHeight/numberOfDivisionToDisplay)*(this->meters/this->metersPerDiv - (float)currentDivIndex);
				//�����ø�����һ���������Ϊʲô��
				//y -= this->displayCenter.y; //��ʱ���ӱ任����Ч��

				//0.�̶�����
				sprintf_s(buffer,"%d",currentDivIndex*this->metersPerDiv);
				std::string currentDivisionText("");
				currentDivisionText.append(buffer);

				Direct3D::v9::resource::mesh::text::TextMesh currentText = 
					Direct3D::v9::resource::mesh::MeshFactory::createTextMesh(device,currentDivisionText,std::string("Microsoft YaHei"));
				currentText.setTranslation(D3DXVECTOR3(this->displayCenter.x+0.35f*displayWidth,y,this->displayCenter.z));
				currentText.setSideLength(D3DXVECTOR3(this->displayWidth,0.15f*(this->displayHeight/numberOfDivisionToDisplay),0.001f));
				//currentText.setScale(D3DXVECTOR3(60,60,0.000001f));
				currentText.render();


				//1.�̶�
				tempVertexArray[0].x = this->displayCenter.x - 0.05f*this->displayWidth;
				tempVertexArray[1].x = this->displayCenter.x + 0.1f*this->displayWidth;

				tempVertexArray[0].y = y;
				tempVertexArray[1].y = y;

				tempVertexArray[0].z = this->displayCenter.z;
				tempVertexArray[1].z = this->displayCenter.z;

				tempVertexArray[0].color = D3DCOLOR_XRGB(0,0xff,0);
				tempVertexArray[1].color = D3DCOLOR_XRGB(0,0xff,0);

				vertexList.push_back(
					std::pair<
						struct Direct3D::v9::resource::primitive::vertex::VertexWithColor,
						struct Direct3D::v9::resource::primitive::vertex::VertexWithColor
					>(tempVertexArray[0],tempVertexArray[1])
					);
			}

			//��Ⱦ��Щ����
			Direct3D::v9::resource::primitive::LineList<struct Direct3D::v9::resource::primitive::vertex::VertexWithColor> 
				horizontalLineList(device,vertexList);

			horizontalLineList.render();
		}

	}
}