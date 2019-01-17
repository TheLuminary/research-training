//创建socket所需要的头文件
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")
//#include "Device.h"
//在Device.h中包含了windows.h
//而在某些旧版本的Visual Studio中Windows.h会自动包含旧版的winsock.h
//所以我们应该优先引入winsock2.h
//这样winsock2.h中的宏就会在预处理阶段防winsock.h参与编译
//总之winsock2.h最好优先包含。


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
				WORD sockVersion = MAKEWORD(2, 2);//将两个BYTE拼接成一个WORD的宏

				//一个结构体
				WSADATA wsaData;

				if (WSAStartup(sockVersion, &wsaData) != 0)
				{
					printf("错误\n");
					return NULL;
				}
				//创建套接字  
				SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				if (serverSocket == INVALID_SOCKET)
				{
					printf("socket error!\n");
					return NULL;
				}
				//指定server地址
				sockaddr_in serverSocketAddress;
				serverSocketAddress.sin_family = AF_INET;
				//#define AF_INET 2	// internetwork: UDP, TCP, etc.
				serverSocketAddress.sin_addr.s_addr = inet_addr("0.0.0.0");  //具体的IP地址
				//监听所有源地址
				//serverSocketAddress.sin_port = htons(940);
				serverSocketAddress.sin_port = htons(port);
				//htons()将一个16位数从主机字节顺序转换成网络字节顺序
				//简单地说,htons()就是将一个数的高低位互换
				//如:12 34 --> 34 12
				//很显然这个可以自己写

				if (bind(serverSocket, (LPSOCKADDR)(&serverSocketAddress), sizeof(serverSocketAddress)) == SOCKET_ERROR)
				{
					printf("bind error !");
					return NULL;
				}
				//开始监听  
				if (listen(serverSocket, 0xf) == SOCKET_ERROR)
				{
					printf("listen error !");
					return NULL;
				}
				//int listen(SOCKET s, int backlog);
				//backlog：等待连接队列的最大长度
				//listen()仅适用于支持连接的套接口，如SOCK_STREAM类型
				//SOCKET s处于一种“变动”模式，申请进入的连接请求被确认，并排队等待被接受。
				//这个函数特别适用于同时有多个连接请求的服务器；
				//如果当一个连接请求到来时，队列已满，那么客户将收到一个WSAECONNREFUSED错误

				return serverSocket;
			}
			DWORD WINAPI receiveMessageBySocket(LPVOID lpParam)
			{
				//用一个Map作为容器传递参数
				std::map<std::string, void *> parameters = *((std::map<std::string, void *> *)lpParam);
				//从parameters map中获得参数
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
					//同步阻塞模式	等待连接
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

							char receivedData[0xff + 1];//这个地方一定要酌情设置大小，这决定了每次能获取多少数据
							int  receptionFlag;//recv函数的返回值 有三种状态每种状态的含义在下方有解释

							//接收来自 这个客户端的消息
							receptionFlag = recv(clientSocket, receivedData, 0xff, 0);
							/************************************************************************
							recv函数 的实质就是从socket的缓冲区里copy出数据
							返回值就是拷贝出字节数的大小。
							上面定义的载体（receivedData）大小是255，所以recv的第三个参数最大只能设置为255，
							如果设置为大于255的数值，当执行recv函数时恰好缓冲区的内容大于255，
							就会导致内存泄漏，导致receptionState值小于零，解除阻塞状态。因此这里最好将第三个参数
							设置为receivedData的大小，那么当缓冲区内的数据小于255的时候
							只需要执行一次recv就可以将缓冲区的内容都拷贝出来，但当缓冲区的数据大
							于255的时候，就要执行多次recv函数。当缓冲区内没有内容的时候，会处于阻塞
							状态，这个while函数会停在这里。直到新的数据进来或者出现异常。
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
										//真的不想用C++了，太麻烦了
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
								//当receptionState == 0 说明客户端已断开连接。这里我们直接跳出循环去等待下次连接即可。
								printf("lost connection , ip = %s:%hu\n",
									inet_ntoa(remoteAddress.sin_addr),
									htons(remoteAddress.sin_port)
									);
								break;
							}
							if (receptionFlag < 0)
							{
								//当receptionState < 0 说明出现了异常 例如阻塞状态解除，或者读取数据时出现指针错误等。
								//所以我们这里要主动断开和客户端的链接，然后跳出循环去等待新的连接。
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