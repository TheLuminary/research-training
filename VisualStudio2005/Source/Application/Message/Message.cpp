#include "message.h"

namespace Application
{
	namespace Message
	{
		namespace Util
		{
			bool convertMessageTo32bitDataList(struct Application::Message::Message sourceMessage,std::list<UINT32> *destination)
			{
				destination->clear();//���
				if(sourceMessage.isUnsignedInteger>0)
				{
					//˵����Ϣ���޷�������
					switch(sourceMessage.isUnsignedInteger)
					{
					case 1:
						//8λ�޷�������
						{
							UINT32 data = 0;
							//�趨type
							data |= ((UINT32)sourceMessage.type)<<24;
							//�趨20��23λ������������
							data |= ((UINT32)0x0a)<<20;
							//�趨����
							data |= (UINT32)sourceMessage.data.uint8;
							destination->push_back(data);
							break;
						}
					case 2:
						//16λ�޷�������
						{
							UINT32 data = 0;
							//�趨type
							data |= ((UINT32)sourceMessage.type)<<24;
							//�趨20��23λ������������
							data |= ((UINT32)0x0b)<<20;
							//�趨����
							data |= (UINT32)sourceMessage.data.uint16;
							destination->push_back(data);
							break;
						}
					case 4:
						//32λ�޷�������
						{
							UINT32 data = 0;
							//�趨type
							data |= ((UINT32)sourceMessage.type)<<24;
							//�趨20��23λ������������
							data |= ((UINT32)0x0c)<<20;
							//�趨����
							//��λ��ǰ����λ����
							data |= (UINT32)(sourceMessage.data.uint32&0x0000ffff);
							destination->push_back(data);

							data = 0;
							//�趨type
							data |= ((UINT32)sourceMessage.type)<<24;
							//�趨20��23λ������������
							data |= ((UINT32)0x0c)<<20;
							//��λ�ں󣬸�λ����
							data |= (UINT32)((sourceMessage.data.uint32&0xffff0000)>>16);
							destination->push_back(data);
							break;
						}
					case 8:
						//64λ�޷�������
						{
							UINT32 data = 0;
							//�趨type
							data |= ((UINT32)sourceMessage.type)<<24;
							//�趨20��23λ������������
							data |= ((UINT32)0x0e)<<20;
							//�趨����
							//��λ��ǰ����λ����
							data |= (UINT32)(sourceMessage.data.uint64&0x000000000000ffff);
							destination->push_back(data);

							data &= 0xffff0000;//��յ�λ����
							data |= (UINT32)((sourceMessage.data.uint64&0x00000000ffff0000)>>16);
							destination->push_back(data);

							data &= 0xffff0000;//��յ�λ����
							data |= (UINT32)((sourceMessage.data.uint64&0x0000ffff00000000)>>32);
							destination->push_back(data);

							data &= 0xffff0000;//��յ�λ����
							data |= (UINT32)((sourceMessage.data.uint64&0xffff000000000000)>>48);
							destination->push_back(data);
							break;
						}
					default:
						return false;
					}
				}
				//////////////////////////////////////////////////////////////////////////
				//����������
				if(sourceMessage.isSignedInteger>0)
				{
					//˵����Ϣ���з�������
					switch(sourceMessage.isSignedInteger)
					{
					case 1:
						//8λ�з�������
						{
							UINT32 data = 0;
							//�趨type
							data |= ((UINT32)sourceMessage.type)<<24;
							//�趨20��23λ������������
							data |= ((UINT32)0x00)<<20;
							//�趨����
							data |= (UINT32)sourceMessage.data.int8;
							destination->push_back(data);
							break;
						}
					case 2:
						//16λ�з�������
						{
							UINT32 data = 0;
							//�趨type
							data |= ((UINT32)sourceMessage.type)<<24;
							//�趨20��23λ������������
							data |= ((UINT32)0x01)<<20;
							//�趨����
							data |= (UINT32)sourceMessage.data.int16;
							destination->push_back(data);
							break;
						}
					case 4:
						//32λ�޷�������
						{
							UINT32 data = 0;
							//�趨type
							data |= ((UINT32)sourceMessage.type)<<24;
							//�趨20��23λ������������
							data |= ((UINT32)0x02)<<20;
							//�趨����
							//��λ��ǰ����λ����
							data |= (UINT32)(sourceMessage.data.int32&0x0000ffff);
							destination->push_back(data);

							data = 0;
							//�趨type
							data |= ((UINT32)sourceMessage.type)<<24;
							//�趨20��23λ������������
							data |= ((UINT32)0x02)<<20;
							//��λ�ں󣬸�λ����
							data |= (UINT32)((sourceMessage.data.int32&0xffff0000)>>16);
							destination->push_back(data);
							break;
						}
					case 8:
						//64λ�޷�������
						{
							UINT32 data = 0;
							//�趨type
							data |= ((UINT32)sourceMessage.type)<<24;
							//�趨20��23λ������������
							data |= ((UINT32)0x04)<<20;
							//�趨����
							//��λ��ǰ����λ����
							data |= (UINT32)(sourceMessage.data.int64&0x000000000000ffff);
							destination->push_back(data);

							data &= 0xffff0000;//��յ�λ����
							data |= (UINT32)((sourceMessage.data.int64&0x00000000ffff0000)>>16);
							destination->push_back(data);

							data &= 0xffff0000;//��յ�λ����
							data |= (UINT32)((sourceMessage.data.int64&0x0000ffff00000000)>>32);
							destination->push_back(data);

							data &= 0xffff0000;//��յ�λ����
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
					//��32λ������
					UINT32 *tempPointer;
					//�ҶԱ�̱���ûʲô��Ȥ������ϲ�����һЩʵ������
					tempPointer = (UINT32 *)&sourceMessage.data.float32;

					UINT32 data = 0;
					//�趨type
					data |= ((UINT32)sourceMessage.type)<<24;
					//�趨20��23λ������������
					data |= ((UINT32)0x06)<<20;
					//�趨����
					//��λ��ǰ����λ����
					data |= (*tempPointer)&0x0000ffff;
					destination->push_back(data);

					data = 0;
					//�趨type
					data |= ((UINT32)sourceMessage.type)<<24;
					//�趨20��23λ������������
					data |= ((UINT32)0x06)<<20;
					//��λ�ں󣬸�λ����
					data |= (UINT32)(((*tempPointer)&0xffff0000)>>16);
					destination->push_back(data);
				}
				if(sourceMessage.isDouble)
				{
					UINT64 *tempPointer;
					//�ҶԱ�̱���ûʲô��Ȥ������ϲ�����һЩʵ������
					tempPointer = (UINT64 *)&sourceMessage.data.float64;

					UINT32 data = 0;
					//�趨type
					data |= ((UINT32)sourceMessage.type)<<24;
					//�趨20��23λ������������
					data |= ((UINT32)0x08)<<20;
					//�趨����
					//��λ��ǰ����λ����
					data |= (UINT32)((*tempPointer)&0x000000000000ffff);
					destination->push_back(data);

					data &= 0xffff0000;//��յ�λ����
					data |= (UINT32)(((*tempPointer)&0x00000000ffff0000)>>16);
					destination->push_back(data);

					data &= 0xffff0000;//��յ�λ����
					data |= (UINT32)(((*tempPointer)&0x0000ffff00000000)>>32);
					destination->push_back(data);

					data &= 0xffff0000;//��յ�λ����
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
				//�趨��20��23λ������������
				UINT8 dataType = (sourceData[0]&0x00f00000)>>20;

				//todo:�趨��16��19λ���������������ݵĵڼ����֣���ЧλΪ16-17λ��0-3�����1����4��16λ����
				switch(sourceLength)
				{
				case 1:
					//����Ϊ1��data����Ϊ8λ��16λ����
					switch(dataType)
					{
					case 0x0a:	//0x0a = 10,��Ϊ�޷���1�ֽ����ͣ���INT8
						destination->isUnsignedInteger = 1;
						destination->isSignedInteger = 0;
						destination->isFloat = false;
						destination->isDouble = false;

						destination->data.uint8 = (UINT8)(sourceData[0]&0x000000ff);
						break;
					case 0x0b:	//0x0b = 11,��Ϊ�޷���2�ֽ����ͣ���INT16
						destination->isUnsignedInteger = 2;
						destination->isSignedInteger = 0;
						destination->isFloat = false;
						destination->isDouble = false;

						destination->data.uint16 = (UINT16)(sourceData[0]&0x0000ffff);
						break;
					case 0x00:	//��Ϊ1�ֽ��з�������
						destination->isUnsignedInteger = 0;
						destination->isSignedInteger = 1;
						destination->isFloat = false;
						destination->isDouble = false;

						destination->data.int8 = (INT8)(sourceData[0]&0x000000ff);
						break;
					case 0x01:	//��Ϊ2�ֽ��з�������
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
					//����Ϊ2��data����Ϊ32λ���ͻ�float
					switch(dataType)
					{
					case 0x0c:	//0x0c = 12,��Ϊ�޷���4�ֽ����ͣ���INT32
						destination->isUnsignedInteger = 4;
						destination->isSignedInteger = 0;
						destination->isFloat = false;
						destination->isDouble = false;
						//
						destination->data.uint32 = ((sourceData[1]&0x0000ffff)<<16)|(sourceData[0]&0x0000ffff);
						break;
					case 0x02:	//0x02��Ϊ�з���4�ֽ�����
						destination->isUnsignedInteger = 0;
						destination->isSignedInteger = 4;
						destination->isFloat = false;
						destination->isDouble = false;
						//
						destination->data.int32 = ((sourceData[1]&0x0000ffff)<<16)|(sourceData[0]&0x0000ffff);
						break;
					case 0x06:	//0x06��Ϊfloat
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
					//����Ϊ4,data����Ϊ64λ���ͻ�double
					switch(dataType)
					{
					case 14:
						//64λ�޷�������
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
						//64λ�з�������
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
						//���λ
						//currentData = 0;//���
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
						currentData = 0;//���
						break;
					}
				}
				return dataList;
			}
            std::list<UINT8> split32bitDataListTo8bitDataList(std::list<UINT32> dataList)
			{
                std::list<UINT8> result;
				result.clear();
				
				//�μǣ���λ��ǰ����λ�ں�
				for(std::list<UINT32>::iterator dataListIterator = dataList.begin();
					dataListIterator != dataList.end();dataListIterator++
					)
				{
					//����������
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
