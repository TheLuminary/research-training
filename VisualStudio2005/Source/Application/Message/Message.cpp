#include "message.h"

namespace Application
{
	namespace Message
	{
		namespace Util
		{
			bool convertMessageTo32bitDataList(struct Application::Message::Message sourceMessage,std::list<UINT32> *destination)
			{
				destination->clear();//清空
				if(sourceMessage.isUnsignedInteger>0)
				{
					//说明信息是无符号整型
					switch(sourceMessage.isUnsignedInteger)
					{
					case 1:
						//8位无符号整型
						{
							UINT32 data = 0;
							//设定type
							data |= ((UINT32)sourceMessage.type)<<24;
							//设定20到23位代表数据类型
							data |= ((UINT32)0x0a)<<20;
							//设定数据
							data |= (UINT32)sourceMessage.data.uint8;
							destination->push_back(data);
							break;
						}
					case 2:
						//16位无符号整型
						{
							UINT32 data = 0;
							//设定type
							data |= ((UINT32)sourceMessage.type)<<24;
							//设定20到23位代表数据类型
							data |= ((UINT32)0x0b)<<20;
							//设定数据
							data |= (UINT32)sourceMessage.data.uint16;
							destination->push_back(data);
							break;
						}
					case 4:
						//32位无符号整型
						{
							UINT32 data = 0;
							//设定type
							data |= ((UINT32)sourceMessage.type)<<24;
							//设定20到23位代表数据类型
							data |= ((UINT32)0x0c)<<20;
							//设定数据
							//低位在前，低位先入
							data |= (UINT32)(sourceMessage.data.uint32&0x0000ffff);
							destination->push_back(data);

							data = 0;
							//设定type
							data |= ((UINT32)sourceMessage.type)<<24;
							//设定20到23位代表数据类型
							data |= ((UINT32)0x0c)<<20;
							//高位在后，高位后入
							data |= (UINT32)((sourceMessage.data.uint32&0xffff0000)>>16);
							destination->push_back(data);
							break;
						}
					case 8:
						//64位无符号整型
						{
							UINT32 data = 0;
							//设定type
							data |= ((UINT32)sourceMessage.type)<<24;
							//设定20到23位代表数据类型
							data |= ((UINT32)0x0e)<<20;
							//设定数据
							//低位在前，低位先入
							data |= (UINT32)(sourceMessage.data.uint64&0x000000000000ffff);
							destination->push_back(data);

							data &= 0xffff0000;//清空低位数据
							data |= (UINT32)((sourceMessage.data.uint64&0x00000000ffff0000)>>16);
							destination->push_back(data);

							data &= 0xffff0000;//清空低位数据
							data |= (UINT32)((sourceMessage.data.uint64&0x0000ffff00000000)>>32);
							destination->push_back(data);

							data &= 0xffff0000;//清空低位数据
							data |= (UINT32)((sourceMessage.data.uint64&0xffff000000000000)>>48);
							destination->push_back(data);
							break;
						}
					default:
						return false;
					}
				}
				//////////////////////////////////////////////////////////////////////////
				//带符号整型
				if(sourceMessage.isSignedInteger>0)
				{
					//说明信息是有符号整型
					switch(sourceMessage.isSignedInteger)
					{
					case 1:
						//8位有符号整型
						{
							UINT32 data = 0;
							//设定type
							data |= ((UINT32)sourceMessage.type)<<24;
							//设定20到23位代表数据类型
							data |= ((UINT32)0x00)<<20;
							//设定数据
							data |= (UINT32)sourceMessage.data.int8;
							destination->push_back(data);
							break;
						}
					case 2:
						//16位有符号整型
						{
							UINT32 data = 0;
							//设定type
							data |= ((UINT32)sourceMessage.type)<<24;
							//设定20到23位代表数据类型
							data |= ((UINT32)0x01)<<20;
							//设定数据
							data |= (UINT32)sourceMessage.data.int16;
							destination->push_back(data);
							break;
						}
					case 4:
						//32位无符号整型
						{
							UINT32 data = 0;
							//设定type
							data |= ((UINT32)sourceMessage.type)<<24;
							//设定20到23位代表数据类型
							data |= ((UINT32)0x02)<<20;
							//设定数据
							//低位在前，低位先入
							data |= (UINT32)(sourceMessage.data.int32&0x0000ffff);
							destination->push_back(data);

							data = 0;
							//设定type
							data |= ((UINT32)sourceMessage.type)<<24;
							//设定20到23位代表数据类型
							data |= ((UINT32)0x02)<<20;
							//高位在后，高位后入
							data |= (UINT32)((sourceMessage.data.int32&0xffff0000)>>16);
							destination->push_back(data);
							break;
						}
					case 8:
						//64位无符号整型
						{
							UINT32 data = 0;
							//设定type
							data |= ((UINT32)sourceMessage.type)<<24;
							//设定20到23位代表数据类型
							data |= ((UINT32)0x04)<<20;
							//设定数据
							//低位在前，低位先入
							data |= (UINT32)(sourceMessage.data.int64&0x000000000000ffff);
							destination->push_back(data);

							data &= 0xffff0000;//清空低位数据
							data |= (UINT32)((sourceMessage.data.int64&0x00000000ffff0000)>>16);
							destination->push_back(data);

							data &= 0xffff0000;//清空低位数据
							data |= (UINT32)((sourceMessage.data.int64&0x0000ffff00000000)>>32);
							destination->push_back(data);

							data &= 0xffff0000;//清空低位数据
							data |= (UINT32)((sourceMessage.data.int64&0xffff000000000000)>>48);
							destination->push_back(data);
							break;
						}
					default:
						return false;
					}
				}
				////////////////////////////////////
				if(sourceMessage.isFloat)
				{
					//是32位浮点数
					UINT32 *tempPointer;
					//我对编程本身没什么兴趣，但是喜欢解决一些实际问题
					tempPointer = (UINT32 *)&sourceMessage.data.float32;

					UINT32 data = 0;
					//设定type
					data |= ((UINT32)sourceMessage.type)<<24;
					//设定20到23位代表数据类型
					data |= ((UINT32)0x06)<<20;
					//设定数据
					//低位在前，低位先入
					data |= (*tempPointer)&0x0000ffff;
					destination->push_back(data);

					data = 0;
					//设定type
					data |= ((UINT32)sourceMessage.type)<<24;
					//设定20到23位代表数据类型
					data |= ((UINT32)0x06)<<20;
					//高位在后，高位后入
					data |= (UINT32)(((*tempPointer)&0xffff0000)>>16);
					destination->push_back(data);
				}
				if(sourceMessage.isDouble)
				{
					UINT64 *tempPointer;
					//我对编程本身没什么兴趣，但是喜欢解决一些实际问题
					tempPointer = (UINT64 *)&sourceMessage.data.float64;

					UINT32 data = 0;
					//设定type
					data |= ((UINT32)sourceMessage.type)<<24;
					//设定20到23位代表数据类型
					data |= ((UINT32)0x08)<<20;
					//设定数据
					//低位在前，低位先入
					data |= (UINT32)((*tempPointer)&0x000000000000ffff);
					destination->push_back(data);

					data &= 0xffff0000;//清空低位数据
					data |= (UINT32)(((*tempPointer)&0x00000000ffff0000)>>16);
					destination->push_back(data);

					data &= 0xffff0000;//清空低位数据
					data |= (UINT32)(((*tempPointer)&0x0000ffff00000000)>>32);
					destination->push_back(data);

					data &= 0xffff0000;//清空低位数据
					data |= (UINT32)(((*tempPointer)&0xffff000000000000)>>48);
					destination->push_back(data);
				}

				return true;
			}
			bool convert32bitDataListToMessage(std::list<UINT32> sourceList,struct Application::Message::Message *destination)
			{
				//
				UINT32 *dataArray = new UINT32[sourceList.size()];
				UINT16 index = 0;
				for(std::list<UINT32>::iterator sourceListIterator = sourceList.begin();
					sourceListIterator != sourceList.end();sourceListIterator++
					)
				{
					*(dataArray + index) = *sourceListIterator;
					index++;
				}

				bool flag = convert32bitDataArrayToMessage(dataArray,(UINT8)sourceList.size(),destination);
				delete[] dataArray;
				return flag;
			}
			bool convert32bitDataArrayToMessage(UINT32 *sourceData,UINT8 sourceLength,struct Application::Message::Message *destination)
			{
				//设定第20到23位代表数据类型
				UINT8 dataType = (sourceData[0]&0x00f00000)>>20;

				//todo:设定第16到19位代表这是整个数据的第几部分，有效位为16-17位，0-3代表第1到第4个16位部分
				switch(sourceLength)
				{
				case 1:
					//长度为1，data可能为8位或16位整型
					switch(dataType)
					{
					case 0x0a:	//0x0a = 10,定为无符号1字节整型，即INT8
						destination->isUnsignedInteger = 1;
						destination->isSignedInteger = 0;
						destination->isFloat = false;
						destination->isDouble = false;

						destination->data.uint8 = (UINT8)(sourceData[0]&0x000000ff);
						break;
					case 0x0b:	//0x0b = 11,定为无符号2字节整型，即INT16
						destination->isUnsignedInteger = 2;
						destination->isSignedInteger = 0;
						destination->isFloat = false;
						destination->isDouble = false;

						destination->data.uint16 = (UINT16)(sourceData[0]&0x0000ffff);
						break;
					case 0x00:	//定为1字节有符号整型
						destination->isUnsignedInteger = 0;
						destination->isSignedInteger = 1;
						destination->isFloat = false;
						destination->isDouble = false;

						destination->data.int8 = (INT8)(sourceData[0]&0x000000ff);
						break;
					case 0x01:	//定为2字节有符号整型
						destination->isUnsignedInteger = 0;
						destination->isSignedInteger = 1;
						destination->isFloat = false;
						destination->isDouble = false;

						destination->data.int16 = (INT16)(sourceData[0]&0x0000ffff);
						break;
					default:
						return false;
					}
					break;

				case 2:
					//长度为2，data可能为32位整型或float
					switch(dataType)
					{
					case 0x0c:	//0x0c = 12,定为无符号4字节整型，即INT32
						destination->isUnsignedInteger = 4;
						destination->isSignedInteger = 0;
						destination->isFloat = false;
						destination->isDouble = false;
						//
						destination->data.uint32 = ((sourceData[1]&0x0000ffff)<<16)|(sourceData[0]&0x0000ffff);
						break;
					case 0x02:	//0x02定为有符号4字节整型
						destination->isUnsignedInteger = 0;
						destination->isSignedInteger = 4;
						destination->isFloat = false;
						destination->isDouble = false;
						//
						destination->data.int32 = ((sourceData[1]&0x0000ffff)<<16)|(sourceData[0]&0x0000ffff);
						break;
					case 0x06:	//0x06定为float
						destination->isUnsignedInteger = 0;
						destination->isSignedInteger = 0;
						destination->isFloat = true;
						destination->isDouble = false;

						{
							UINT32 temp = ((sourceData[1]&0x0000ffff)<<16)|(sourceData[0]&0x0000ffff);

							float *pFloat;
							pFloat = (float *)&temp;
							destination->data.float32 = *pFloat;
						}

						break;
					default:
						return false;
					}
					break;

				case 4:
					//长度为4,data可能为64位整型或double
					switch(dataType)
					{
					case 14:
						//64位无符号整型
						destination->isUnsignedInteger = 8;
						destination->isSignedInteger = 0;
						destination->isFloat = false;
						destination->isDouble = false;
						{

							UINT64 temp = 0;
							temp |= ((UINT64)(sourceData[0]&0x0000ffff));
							temp |= ((UINT64)(sourceData[1]&0x0000ffff))<<16;
							temp |= ((UINT64)(sourceData[2]&0x0000ffff))<<32;
							temp |= ((UINT64)(sourceData[3]&0x0000ffff))<<48;
							destination->data.uint64 = temp;
						}



						break;
					case 0x04:
						//64位有符号整型
						destination->isUnsignedInteger = 0;
						destination->isSignedInteger = 8;
						destination->isFloat = false;
						destination->isDouble = false;
						{
							INT64 temp = 0;
							temp |= ((INT64)(sourceData[0]&0x0000ffff));
							temp |= ((INT64)(sourceData[1]&0x0000ffff))<<16;
							temp |= ((INT64)(sourceData[2]&0x0000ffff))<<32;
							temp |= ((INT64)(sourceData[3]&0x0000ffff))<<48;

							destination->data.int64 = temp;
						}
						break;
					case 0x08:
						//double
						destination->isUnsignedInteger = 0;
						destination->isSignedInteger = 0;
						destination->isFloat = false;
						destination->isDouble = true;
						{
							UINT64 temp = (
								((UINT64)((UINT16)(sourceData[0]&0x0000ffff)))
								|(((UINT64)((UINT16)(sourceData[1]&0x0000ffff)))<<16)
								|(((UINT64)((UINT16)(sourceData[2]&0x0000ffff)))<<32)
								|(((UINT64)((UINT16)(sourceData[3]&0x0000ffff)))<<48)
								);
							double *pDouble;
							pDouble = (double *)&temp;
							destination->data.float64 = *pDouble;
						}
						break;
					default:
						return false;
					}
					break;
				default:
					return false;
				}
				destination->type = (sourceData[0]&0xff000000)>>24;

				return true;
			}
			/////////////////////////////////////////////////////////////////
			std::list<UINT32> charArrayTo32bitDataList(const char charArray[],UINT16 charArrayLength)
			{
				std::list<UINT32> dataList;
				if(charArrayLength%4 != 0) return dataList;
				dataList.clear();
				UINT16 index = 0;

				UINT32 currentData = 0;

				UINT8 *puint8;
				puint8 = (UINT8 *)charArray;
				UINT32 temp1;
				for(index = 0;index<charArrayLength;index++)
				{
					switch(index % 4)
					{
					case 0:
						//最低位
						//currentData = 0;//清空
						currentData = (UINT32)*(puint8+index);
						break;
					case 1:
						temp1 = (UINT32)*(puint8+index);
						currentData |= (temp1<<8)&0x0000ff00;
						break;
					case 2:
						temp1 = (UINT32)*(puint8+index);
						currentData |= (temp1<<16)&0x00ff0000;
						break;
					case 3:
						temp1 = (UINT32)*(puint8+index);
						currentData |= (temp1<<24)&0xff000000;
						//push
						dataList.push_back(currentData);
						currentData = 0;//清空
						break;
					}
				}
				return dataList;
			}
            std::list<UINT8> split32bitDataListTo8bitDataList(std::list<UINT32> dataList)
			{
                std::list<UINT8> result;
				result.clear();
				
				//牢记：低位在前，高位在后
				for(std::list<UINT32>::iterator dataListIterator = dataList.begin();
					dataListIterator != dataList.end();dataListIterator++
					)
				{
					//唉…………
                    UINT8 temp = (UINT8)((*dataListIterator)&0x000000ff);
                    result.push_back(temp);
                    temp = (UINT8)(((*dataListIterator)&0x0000ff00)>>8);
                    result.push_back(temp);
                    temp = (UINT8)(((*dataListIterator)&0x00ff0000)>>16);
                    result.push_back(temp);
                    temp = (UINT8)(((*dataListIterator)&0xff000000)>>24);
                    result.push_back(temp);
				}
				return result;
			}
			//////////////////////////////////////////////////////////////////////////////
			std::list<UINT16> convertFloatTo16bitDataList(float data)
			{
				UINT32 *p;
				p = (UINT32 *)&data;
				std::list<UINT16> tempList;

				UINT16 temp1;
				temp1 = (UINT16)(*p & 0x0000ffff);
				tempList.push_back(temp1);
				temp1 = (UINT16)((*p & 0xffff0000)>>16);
				tempList.push_back(temp1);
				return tempList;
			}
			std::list<UINT16> convertDoubleTo16bitDataList(double data)
			{
				UINT64 *p;
				p = (UINT64 *)&data;
				std::list<UINT16> tempList;
				UINT16 temp1 = (UINT16)(*p & 0x000000000000ffff);
				tempList.push_back(temp1);
				temp1 = (UINT16)((*p & 0x00000000ffff0000)>>16);
				tempList.push_back(temp1);
				temp1 = (UINT16)((*p & 0x0000ffff00000000)>>32);
				tempList.push_back(temp1);
				temp1 = (UINT16)((*p & 0xffff000000000000)>>48);
				tempList.push_back(temp1);
				return tempList;
			}

			bool convert16bitDataListToFloat(std::list<UINT16> dataList,float *pData)
			{
				if(dataList.size()!=2||pData == NULL) return false;
				UINT16 dataArray[2];
				UINT8 index = 0;
				for(std::list<UINT16>::iterator dataListIterator = dataList.begin();
					dataListIterator != dataList.end();dataListIterator ++)
				{
					//
					dataArray[index] = *dataListIterator;
					index++;
				}
				UINT32 temp = (((UINT32)dataArray[1])<<16)|(UINT32)dataArray[0];
				float *tempPointer = (float *)&temp;
				*pData = *tempPointer;
				return true;
			}
			bool convert16bitDataListToDouble(std::list<UINT16> dataList,double *pData)
			{
				if(dataList.size()!=4||pData == NULL) return false;
				UINT16 dataArray[4];
				UINT8 index = 0;
				for(std::list<UINT16>::iterator dataListIterator = dataList.begin();
					dataListIterator != dataList.end();dataListIterator ++)
				{
					//
					dataArray[index] = *dataListIterator;
					index++;
				}
				UINT64 temp = (((UINT64)dataArray[3])<<48)|(((UINT64)dataArray[2])<<32)|(((UINT64)dataArray[1])<<16)|(UINT64)dataArray[0];
				double *tempPointer = (double *)&temp;
				*pData = *tempPointer;
				return true;
			}
		}
	}
}
