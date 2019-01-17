//����socket����Ҫ��ͷ�ļ�
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")
//#include "Device.h"
//��Device.h�а�����windows.h
//����ĳЩ�ɰ汾��Visual Studio��Windows.h���Զ������ɰ��winsock.h
//��������Ӧ����������winsock2.h
//����winsock2.h�еĺ�ͻ���Ԥ����׶η�winsock.h�������
//��֮winsock2.h������Ȱ�����


#include "threads.h"

#include "../../Message/message.h"
#include "../ParameterStore.h"

#include <map>
#include <queue>
#include <list>

namespace Application
{
	namespace HUD
	{
		namespace Thread
		{
			SOCKET openServerSocket(unsigned short int port)
			{
				WORD sockVersion = MAKEWORD(2, 2);//������BYTEƴ�ӳ�һ��WORD�ĺ�

				//һ���ṹ��
				WSADATA wsaData;

				if (WSAStartup(sockVersion, &wsaData) != 0)
				{
					printf("����\n");
					return NULL;
				}
				//�����׽���  
				SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				if (serverSocket == INVALID_SOCKET)
				{
					printf("socket error!\n");
					return NULL;
				}
				//ָ��server��ַ
				sockaddr_in serverSocketAddress;
				serverSocketAddress.sin_family = AF_INET;
				//#define AF_INET 2	// internetwork: UDP, TCP, etc.
				serverSocketAddress.sin_addr.s_addr = inet_addr("0.0.0.0");  //�����IP��ַ
				//��������Դ��ַ
				//serverSocketAddress.sin_port = htons(940);
				serverSocketAddress.sin_port = htons(port);
				//htons()��һ��16λ���������ֽ�˳��ת���������ֽ�˳��
				//�򵥵�˵,htons()���ǽ�һ�����ĸߵ�λ����
				//��:12 34 --> 34 12
				//����Ȼ��������Լ�д

				if (bind(serverSocket, (LPSOCKADDR)(&serverSocketAddress), sizeof(serverSocketAddress)) == SOCKET_ERROR)
				{
					printf("bind error !");
					return NULL;
				}
				//��ʼ����  
				if (listen(serverSocket, 0xf) == SOCKET_ERROR)
				{
					printf("listen error !");
					return NULL;
				}
				//int listen(SOCKET s, int backlog);
				//backlog���ȴ����Ӷ��е���󳤶�
				//listen()��������֧�����ӵ��׽ӿڣ���SOCK_STREAM����
				//SOCKET s����һ�֡��䶯��ģʽ������������������ȷ�ϣ����Ŷӵȴ������ܡ�
				//��������ر�������ͬʱ�ж����������ķ�������
				//�����һ������������ʱ��������������ô�ͻ����յ�һ��WSAECONNREFUSED����

				return serverSocket;
			}
			DWORD WINAPI receiveMessageBySocket(LPVOID lpParam)
			{
				//��һ��Map��Ϊ�������ݲ���
				std::map<std::string, void *> parameters = *((std::map<std::string, void *> *)lpParam);
				//��parameters map�л�ò���
				/*
				for (std::map<std::string, void *>::iterator iterator = parameters.begin();
				iterator != parameters.end();iterator++
				){}
				*/

				SOCKET serverSocket = openServerSocket(*((unsigned short *)parameters.find(std::string("ServerPort"))->second));
				//std::queue<std::list<UINT32>> *messageQueue = (std::queue<std::list<UINT32>> *)(parameters.find(std::string("MessageQueue"))->second);

				while (true)
				{
					sockaddr_in remoteAddress;
					int remoteAddressLength = sizeof(remoteAddress);
					//ͬ������ģʽ	�ȴ�����
					SOCKET clientSocket = accept(serverSocket, (SOCKADDR *)&remoteAddress, &remoteAddressLength);

					if (clientSocket == INVALID_SOCKET)
					{
						printf("accept error !");
						continue;
					}
					else
					{
						printf("new Control Panel Socket connection from ip = %s:%hu\n",
							inet_ntoa(remoteAddress.sin_addr),
							htons(remoteAddress.sin_port)
							);
						//send(clientSocket, "hello i am server \n", strlen("hello i am server \n"), 0);
						while (true)
						{

							char receivedData[0xff + 1];//����ط�һ��Ҫ�������ô�С���������ÿ���ܻ�ȡ��������
							int  receptionFlag;//recv�����ķ���ֵ ������״̬ÿ��״̬�ĺ������·��н���

							//�������� ����ͻ��˵���Ϣ
							receptionFlag = recv(clientSocket, receivedData, 0xff, 0);
							/************************************************************************
							recv���� ��ʵ�ʾ��Ǵ�socket�Ļ�������copy������
							����ֵ���ǿ������ֽ����Ĵ�С��
							���涨������壨receivedData����С��255������recv�ĵ������������ֻ������Ϊ255��
							�������Ϊ����255����ֵ����ִ��recv����ʱǡ�û����������ݴ���255��
							�ͻᵼ���ڴ�й©������receptionStateֵС���㣬�������״̬�����������ý�����������
							����ΪreceivedData�Ĵ�С����ô���������ڵ�����С��255��ʱ��
							ֻ��Ҫִ��һ��recv�Ϳ��Խ������������ݶ��������������������������ݴ�
							��255��ʱ�򣬾�Ҫִ�ж��recv����������������û�����ݵ�ʱ�򣬻ᴦ������
							״̬�����while������ͣ�����ֱ���µ����ݽ������߳����쳣��
							************************************************************************/

							if (receptionFlag > 0)
							{
								if(receptionFlag%4 == 0)
								{
									std::list<UINT32> current32bitDataList = 
										Application::Message::Util::charArrayTo32bitDataList(receivedData,receptionFlag);

									Application::HUD::ParameterStore::getInstance()->update(current32bitDataList);

									/*
									for(std::list<UINT32>::iterator dataListIterator = current32bitDataList.begin();
										dataListIterator != current32bitDataList.end();dataListIterator++
										)
									{
										//��Ĳ�����C++�ˣ�̫�鷳��
										std::stringstream sStream;
										sStream<<*dataListIterator;

										std::string debugMessage = sStream.str();

										MessageBoxA(0,debugMessage.c_str(),0,0);
									}
									*/
								}
							}
							if (receptionFlag == 0)
							{
								//��receptionState == 0 ˵���ͻ����ѶϿ����ӡ���������ֱ������ѭ��ȥ�ȴ��´����Ӽ��ɡ�
								printf("lost connection , ip = %s:%hu\n",
									inet_ntoa(remoteAddress.sin_addr),
									htons(remoteAddress.sin_port)
									);
								break;
							}
							if (receptionFlag < 0)
							{
								//��receptionState < 0 ˵���������쳣 ��������״̬��������߶�ȡ����ʱ����ָ�����ȡ�
								//������������Ҫ�����Ͽ��Ϳͻ��˵����ӣ�Ȼ������ѭ��ȥ�ȴ��µ����ӡ�
								printf("something wrong of %s\n", inet_ntoa(remoteAddress.sin_addr));
								closesocket(clientSocket);
								break;
							}
						}
					}
				}
			}
		}
	}
}