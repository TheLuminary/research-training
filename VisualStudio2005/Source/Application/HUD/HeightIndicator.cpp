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
			//进行柯里化
			this->metersPerDiv = metersPerDiv;
			this->numberOfDivisionToDisplay = numberOfDivisionToDisplay;
			this->displayCenter = displayCenter;
			this->displayWidth = displayWidth;
			this->displayHeight = displayHeight;

			//但是突然想到…………生成文字的时候，好像要考虑更多了
			//displayHeight / numberOfDivisionToDisplay得到每个分隔占的像素，然后选择一定的占空比生成文字
		}

		void HeightIndicator::setMeters(float meters)
		{
			this->meters = meters;
		}
		void HeightIndicator::render(IDirect3DDevice9 *device)
		{
			//如果meter正好在一个分隔上的话…………
			struct {double lower,upper;} metersBound;
			metersBound.lower = this->meters - ((double)(this->numberOfDivisionToDisplay * this->metersPerDiv)*0.501);
			metersBound.upper = this->meters + ((double)(this->numberOfDivisionToDisplay * this->metersPerDiv)*0.501);
			//如果正好等于边界那就一起画，这个和AttitudeIndicator不同  并不需要想太多

			struct {INT16 lower,upper;} divIndexBound;
			divIndexBound.lower = (INT16)(metersBound.lower / this->metersPerDiv);
			divIndexBound.upper = (INT16)(metersBound.upper / this->metersPerDiv);
			if(metersBound.lower > 0) divIndexBound.lower++;
			if(metersBound.upper < 0) divIndexBound.upper--;
			//这样处理是因为C/C++语言中没有MATLAB的ceil()和floor()，又真的不想抽象出一个Util的库干这事

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
			//固定不动的刻度线
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
				//然后怎么确定各条线的坐标呢……
				//把高度meters转换成div为单位
				float y = 0-(this->displayHeight/numberOfDivisionToDisplay)*(this->meters/this->metersPerDiv - (float)currentDivIndex);
				//这里用负的想一想就能明白为什么了
				//y -= this->displayCenter.y; //暂时不加变换看看效果

				//0.刻度数字
				sprintf_s(buffer,"%d",currentDivIndex*this->metersPerDiv);
				std::string currentDivisionText("");
				currentDivisionText.append(buffer);

				Direct3D::v9::resource::mesh::text::TextMesh currentText = 
					Direct3D::v9::resource::mesh::MeshFactory::createTextMesh(device,currentDivisionText,std::string("Microsoft YaHei"));
				currentText.setTranslation(D3DXVECTOR3(this->displayCenter.x+0.35f*displayWidth,y,this->displayCenter.z));
				currentText.setSideLength(D3DXVECTOR3(this->displayWidth,0.15f*(this->displayHeight/numberOfDivisionToDisplay),0.001f));
				//currentText.setScale(D3DXVECTOR3(60,60,0.000001f));
				currentText.render();


				//1.刻度
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

			//渲染这些横线
			Direct3D::v9::resource::primitive::LineList<struct Direct3D::v9::resource::primitive::vertex::VertexWithColor> 
				horizontalLineList(device,vertexList);

			horizontalLineList.render();
		}

	}
}