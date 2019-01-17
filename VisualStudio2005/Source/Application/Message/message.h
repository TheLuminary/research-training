#pragma once
//#include <BaseTsd.h>
#include <Windows.h>

#include <list>
#include <string>

namespace Application
{
	namespace Message
	{
		typedef struct Message
		{
			UINT8 type;//消息类型  用于区分不同种类的消息
			//判断是否是整型
			//>0 :sizeof the data
			//==0:NOT a Unsigned Integer
			UINT8 isUnsignedInteger,isSignedInteger;
			bool isFloat,isDouble;
			union
			{
				UINT8 uint8;
				UINT16 uint16;
				UINT32 uint32;
				UINT64 uint64;

				INT8 int8;
				INT16 int16;
				INT32 int32;
				INT64 int64;

				float float32;
				double float64;
			} data;
		} Message_t;

		namespace Util
		{
			bool convertMessageTo32bitDataList(struct Application::Message::Message sourceMessage,std::list<UINT32> *destination);
			bool convert32bitDataListToMessage(std::list<UINT32> sourceList,struct Application::Message::Message *destination);
			bool convert32bitDataArrayToMessage(UINT32 *sourceData,UINT8 sourceLength,struct Application::Message::Message *destination);

            std::list<UINT32> charArrayTo32bitDataList(const char charArray[],UINT16 charArrayLength);
            std::list<UINT8> split32bitDataListTo8bitDataList(std::list<UINT32> dataList);

			std::list<UINT16> convertFloatTo16bitDataList(float data);
			std::list<UINT16> convertDoubleTo16bitDataList(double data);

			bool convert16bitDataListToFloat(std::list<UINT16> dataList,float *pData);
			bool convert16bitDataListToDouble(std::list<UINT16> dataList,double *pData);
		}
	}
}
