#include "HeadingIndicator.h"

namespace Application
{
	namespace HUD
	{
		void HeadingIndicator::render(IDirect3DDevice9 *device)
		{
			//
			using Direct3D::v9::resource::primitive::vertex::VertexWithColor;

			struct {float lower,upper;} degreesBound;

			degreesBound.lower = this->degrees - 
				(float)(this->numberOfDivisionToDisplay*this->degreesPerDiv)*0.501f;
			degreesBound.upper = this->degrees +
				(float)(this->numberOfDivisionToDisplay*this->degreesPerDiv)*0.501f;

			struct {INT8 lower,upper;} divIndexBound;
			divIndexBound.lower = (INT8)(degreesBound.lower / this->degreesPerDiv);
			divIndexBound.upper = (INT8)(degreesBound.upper / this->degreesPerDiv);
			if(degreesBound.lower > 0) divIndexBound.lower++;
			if(degreesBound.upper < 0) divIndexBound.upper--;
			//

			char buffer[64] = {0};
			sprintf_s(buffer,"%.0f",this->degrees);
			std::string currentDegreesText("");
			currentDegreesText.append(buffer);

			/*
			Direct3D::v9::resource::mesh::text::TextMesh currentText = 
				Direct3D::v9::resource::mesh::MeshFactory::createTextMesh(device,currentDegreesText,std::string("Microsoft YaHei"));

			currentText.setTranslation(D3DXVECTOR3(
				this->displayCenter.x-(float)0.3*displayWidth,
				this->displayCenter.y+(this->displayHeight/numberOfDivisionToDisplay)*0.15f,
				this->displayCenter.z));
			//�������Ҫ��һ�����䣬�����Ժ�����
			currentText.setSideLength(D3DXVECTOR3(this->displayWidth*0.4f,(this->displayHeight/numberOfDivisionToDisplay)*0.2f,0.001f));
			//currentText.setScale(D3DXVECTOR3(60,60,0.000001f));
			currentText.render();
			*/


			std::list<
				std::pair<struct VertexWithColor,struct VertexWithColor>
			> vertexList;

			struct VertexWithColor tempVertexArray[2];
			//�̶������Ŀ̶���
			tempVertexArray[0].x = this->displayCenter.x;
			tempVertexArray[1].x = this->displayCenter.x;

			tempVertexArray[0].y = this->displayCenter.y - (float)0.5*this->displayHeight;
			tempVertexArray[1].y = this->displayCenter.y - (float)0.1*this->displayHeight;

			tempVertexArray[0].z = this->displayCenter.z;
			tempVertexArray[1].z = this->displayCenter.z;

			tempVertexArray[0].color = D3DCOLOR_XRGB(0,0xff,0);
			tempVertexArray[1].color = D3DCOLOR_XRGB(0,0xff,0);

			vertexList.push_back(
				std::pair<struct VertexWithColor,struct VertexWithColor
				>(tempVertexArray[0],tempVertexArray[1])
				);

			for(INT16 currentDivIndex = divIndexBound.lower;
				currentDivIndex <= divIndexBound.upper;
				currentDivIndex ++)
			{
				float x = currentDivIndex*this->degreesPerDiv - this->degrees;
				//������һ���õ���x���������ƫ�ƵĶ�������Ҫ����ÿһ������Ӧ�Ŀռ䳤��
				x *= this->displayWidth/(this->numberOfDivisionToDisplay*this->degreesPerDiv);
				//����õ���x���ǵ�ǰ�̶�����Ӧ��x

				//0.��ʾ�̶���
				sprintf_s(buffer,"%d",currentDivIndex*this->degreesPerDiv);
				std::string currentDivisionText("");
				currentDivisionText.append(buffer);

				Direct3D::v9::resource::mesh::text::TextMesh currentText = 
					Direct3D::v9::resource::mesh::MeshFactory::createTextMesh(device,currentDivisionText,std::string("Microsoft YaHei"));
				//currentText.setTranslation(D3DXVECTOR3(this->displayCenter.x+0.35f*displayWidth,y,this->displayCenter.z));
				//currentText.setSideLength(D3DXVECTOR3(this->displayWidth,0.15f*(this->displayHeight/numberOfDivisionToDisplay),0.001f));
				//currentText.setScale(D3DXVECTOR3(60,60,0.000001f));

				currentText.setTranslation(
					D3DXVECTOR3(this->displayCenter.x+x,this->displayCenter.y+0.35f*this->displayHeight,this->displayCenter.z)
					);
				currentText.setSideLength(D3DXVECTOR3(
					0.8f*(this->displayWidth/this->numberOfDivisionToDisplay),
					(this->displayHeight/numberOfDivisionToDisplay),
					0.001f)
					);

				currentText.render();

				//1.�̶�
				tempVertexArray[0].x = this->displayCenter.x+x;
				tempVertexArray[1].x = this->displayCenter.x+x;

				tempVertexArray[0].y = this->displayCenter.y - 0.05f*this->displayHeight;
				tempVertexArray[1].y = this->displayCenter.y + 0.1f*this->displayHeight;

				tempVertexArray[0].z = this->displayCenter.z;
				tempVertexArray[1].z = this->displayCenter.z;

				tempVertexArray[0].color = D3DCOLOR_XRGB(0,0xff,0);
				tempVertexArray[1].color = D3DCOLOR_XRGB(0,0xff,0);

				vertexList.push_back(
					std::pair<struct VertexWithColor,struct VertexWithColor
					>(tempVertexArray[0],tempVertexArray[1])
					);
			}
			//��Ⱦ��Щ����
			Direct3D::v9::resource::primitive::LineList<struct VertexWithColor>(device,vertexList).render();


			//����
		}
		//��������������ֻ�Ǵ����setter
		////////////////////////////////////////////////////////
		HeadingIndicator::HeadingIndicator(
			UINT8 degreesPerDiv,UINT8 numberOfDivisionToDisplay, 
			D3DXVECTOR3 displayCenter,float displayWidth,float displayHeight)
		{
			this->degreesPerDiv = degreesPerDiv;
			this->numberOfDivisionToDisplay = numberOfDivisionToDisplay;
			this->displayCenter = displayCenter;
			this->displayWidth = displayWidth;
			this->displayHeight = displayHeight;
			//
		}

		void HeadingIndicator::setDegrees(float degrees)
		{
			this->degrees = degrees;
		}
	}
}