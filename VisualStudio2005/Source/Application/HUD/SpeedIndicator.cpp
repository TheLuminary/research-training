#include "SpeedIndicator.h"

namespace Application
{
	namespace HUD
	{
		void SpeedIndicator::render(IDirect3DDevice9 *device)
		{
			struct {INT32 lower,upper;} speedBound;
			speedBound.lower = this->speed - ((this->numberOfDivisionToDisplay * this->speedPerDiv)/2)-1;
			speedBound.upper = this->speed + ((this->numberOfDivisionToDisplay * this->speedPerDiv)/2)+1;
			//如果正好等于边界那就一起画，这个和AttitudeIndicator不同  并不需要想太多

			struct {INT16 lower,upper;} divIndexBound;
			divIndexBound.lower = (INT16)(speedBound.lower / this->speedPerDiv);
			divIndexBound.upper = (INT16)(speedBound.upper / this->speedPerDiv);
			if(speedBound.lower > 0) divIndexBound.lower++;
			if(speedBound.upper < 0) divIndexBound.upper--;

			//这一句是为了修正…………你什么时候见过速度为负值
			divIndexBound.lower = divIndexBound.lower < 0?0:divIndexBound.lower;

			char buffer[64] = {0};
			sprintf_s(buffer,"%hu",this->speed);
			std::string currentHeightText("");
			currentHeightText.append(buffer);

			Direct3D::v9::resource::mesh::text::TextMesh currentText = 
				Direct3D::v9::resource::mesh::MeshFactory::createTextMesh(device,currentHeightText,std::string("Microsoft YaHei"));
			/*
			currentText.setTranslation(D3DXVECTOR3(
				this->displayCenter.x-(float)0.3*displayWidth,
				this->displayCenter.y+(this->displayHeight/numberOfDivisionToDisplay)*0.15f,
				this->displayCenter.z));
			*/
			currentText.setTranslation(D3DXVECTOR3(
				this->displayCenter.x+0.3f*this->displayWidth,
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
			//固定不动的刻度线
			//tempVertexArray[0].x = this->displayCenter.x - (float)0.5*this->displayWidth;
			//tempVertexArray[1].x = this->displayCenter.x - (float)0.1*this->displayWidth;
			tempVertexArray[0].x = this->displayCenter.x + (float)0.5*this->displayWidth;
			tempVertexArray[1].x = this->displayCenter.x + (float)0.1*this->displayWidth;

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
						//然后怎么确定各条线的坐标呢……
						//把高度meters转换成div为单位
						//float y = 0-(this->displayHeight/numberOfDivisionToDisplay)*(this->meters/this->metersPerDiv - (float)currentDivIndex);
						//这里用负的想一想就能明白为什么了
						//y -= this->displayCenter.y; //暂时不加变换看看效果
						float y = 0-(this->displayHeight/numberOfDivisionToDisplay)*((float)this->speed/this->speedPerDiv - (float)currentDivIndex);

						//0.刻度数字
						sprintf_s(buffer,"%d",currentDivIndex*this->speedPerDiv);
						std::string currentDivisionText("");
						currentDivisionText.append(buffer);

						Direct3D::v9::resource::mesh::text::TextMesh currentText = 
							Direct3D::v9::resource::mesh::MeshFactory::createTextMesh(device,currentDivisionText,std::string("Microsoft YaHei"));
						//currentText.setTranslation(D3DXVECTOR3(this->displayCenter.x+0.35f*displayWidth,y,this->displayCenter.z));
						currentText.setTranslation(D3DXVECTOR3(this->displayCenter.x-0.35f*displayWidth,y,this->displayCenter.z));
						currentText.setSideLength(D3DXVECTOR3(this->displayWidth,0.15f*(this->displayHeight/numberOfDivisionToDisplay),0.001f));
						//currentText.setScale(D3DXVECTOR3(60,60,0.000001f));
						currentText.render();


						//1.刻度
						//tempVertexArray[0].x = this->displayCenter.x - 0.05f*this->displayWidth;
						//tempVertexArray[1].x = this->displayCenter.x + 0.1f*this->displayWidth;
						tempVertexArray[0].x = this->displayCenter.x + 0.05f*this->displayWidth;
						tempVertexArray[1].x = this->displayCenter.x - 0.1f*this->displayWidth;

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

					//渲染这些横线
					Direct3D::v9::resource::primitive::LineList<struct Direct3D::v9::resource::primitive::vertex::VertexWithColor> 
						horizontalLineList(device,vertexList);

					horizontalLineList.render();
		}
		//下面这两个方法都不可能出错
		SpeedIndicator::SpeedIndicator(
			UINT16 speedPerDiv,UINT8 numberOfDivisionToDisplay, 
			D3DXVECTOR3 displayCenter,float displayWidth,float displayHeight)
		{
			this->speedPerDiv = speedPerDiv;
			this->numberOfDivisionToDisplay = numberOfDivisionToDisplay;
			this->displayCenter = displayCenter;
			this->displayWidth = displayWidth;
			this->displayHeight = displayHeight;

			//displayHeight / numberOfDivisionToDisplay得到每个分隔占的像素，然后选择一定的占空比生成文字
		}

		void SpeedIndicator::setSpeed(unsigned short int speed)
		{
			this->speed = speed;
		}
	}
}