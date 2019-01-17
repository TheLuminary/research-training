#include "../../application.h"
#include "threads.h"


#include "../../Message/message.h"
#include "../ParameterStore.h"

namespace Application
{
	namespace HUD
	{
		namespace Thread
		{
			void displayError(int handlerNumber,int errorCode,bool releaseCard = true);

			//该函数用于组合命令字
			unsigned short int combine1553BCommand(
				unsigned char remoteTerminalAddress, bool isTx, unsigned char subAddress, unsigned char dataLength);


			DWORD WINAPI receiveDataFrom1553B(LPVOID lpParam)
			{
				//0.初始化板卡
				unsigned short int deviceNumber, moduleNumber;
				/*
				puts("Default device number for single board when ExcConfig not used:\n");
				puts("M4k-1553MCH -> 25, PCI/MCH -> 29\n");

				printf("Enter the device number of the card you wish to use.\n");
				scanf("%hu", &deviceNumber);
				printf("Enter the module number.\n");
				scanf("%hu", &moduleNumber);
				printf("Calling Init_Module_Mch with device %hu module %hu\n", deviceNumber, moduleNumber);
				*/

				deviceNumber = 29;
				moduleNumber = 0;
				//这是固定的，不需要改动

				int errorCode;
				//char errorString[255];

				int handlerNumber = Init_Module_MCH(deviceNumber, moduleNumber);
				/*
				if (handlerNumber < 0)
				{
					//有错误发生  错误码就是上一个函数返回的负数
					errorCode = handlerNumber;
					Get_Error_String_MCH(errorCode, 200, errorString);
					//printf("Init_Module_MCH Failure. %s\n", errorString);
					//system("pause");
					//exit(0);		//在初始化时失败，故不需要释放资源
				}*/

				if(handlerNumber<0) displayError(handlerNumber,handlerNumber,false);

				//1.设置模式
				short int macroDefinedMode = BC_MODE_MCH;
				errorCode = Set_Mode_MCH(handlerNumber, macroDefinedMode);
				if (errorCode < 0)
				{
					/*
					Get_Error_String_MCH(errorCode, 200, errorString);
					//printf("%s\n", errorString);
					Release_Module_MCH(handlerNumber);
					system("pause");
					exit(0);
					*/
					displayError(handlerNumber,errorCode);
				}
				//真是看着原来机载总线的代码一步一步改出来的…………
				CMDENTRY commandEntry;

				for(UINT i = 0;i<=24;i++)
				{
					//要求终端i发送20个字的数据，存储到第i块data block
					commandEntry.control = SENDMSG | CW_BUS_A;
					commandEntry.command1 = combine1553BCommand(i,true,i,20);
					commandEntry.command2 = 0;
					commandEntry.status1 = 0;
					commandEntry.status2 = 0;
					commandEntry.datablk = i; /* use data block i */
					commandEntry.gotocmd = 0;
					commandEntry.timer = 0;
					errorCode = Set_BC_Cmd_MCH(handlerNumber, i, &commandEntry);
					if (errorCode < 0) displayError(handlerNumber,errorCode);
				}

				commandEntry.control = 0xa000;    // Interrupt and continue, Command Block accessed
				commandEntry.command1 = 0x0C23;
				commandEntry.command2 = 0;
				commandEntry.status1 = 0;
				commandEntry.status2 = 0;
				commandEntry.datablk = 0;             
				commandEntry.gotocmd = 0;
				commandEntry.timer = 0;
				errorCode = Set_BC_Cmd_MCH(handlerNumber,21, &commandEntry);
				if (errorCode < 0) displayError(handlerNumber,errorCode);

				commandEntry.control = GOTO_MSG;
				commandEntry.command1 = 0;
				commandEntry.command2 = 0;
				commandEntry.status1 = 0;
				commandEntry.status2 = 0;
				commandEntry.datablk = 0;
				commandEntry.gotocmd = 0;//go to message0
				commandEntry.timer = 0;
				errorCode = Set_BC_Cmd_MCH(handlerNumber,22,&commandEntry);
				if (errorCode < 0) displayError(handlerNumber,errorCode);

				//写到这里逐渐意识到了这套系统的内聚做得并不好= =

				errorCode = Set_Interrupt_Mask_MCH(handlerNumber, 0x0002);    //每一个命令块被执行  都产生一次中断
				if (errorCode < 0) displayError(handlerNumber,errorCode);

				//开始运行
				errorCode = Run_BC_MCH(handlerNumber);
				if (errorCode < 0) displayError(handlerNumber,errorCode);

				short int receivedDataBuffer[32];
				//用于暂存接收到的数据
				while (true)
				{
					Wait_For_Interrupt_MCH(handlerNumber, INFINITE);
					unsigned long int interruptCount;
					Get_Interrupt_Count_MCH(handlerNumber, &interruptCount);
					//printf("%lu ", interruptCount);
					//读取数据块中的数据并加以处理

					for(UINT8 dataBlockIndex = 1;dataBlockIndex<=20;dataBlockIndex++)
					{
						errorCode = Read_BC_Datablk_MCH(handlerNumber,dataBlockIndex,20,(unsigned short int *)receivedDataBuffer);
						if(errorCode<0) displayError(handlerNumber,errorCode);
						if(errorCode<0) exit(-1);
						//然后就单独处理吧

						switch(dataBlockIndex)
						{
						case 1:
							//把终端1作为惯性导航INS吧…………因为这是先做的部分
							//依次是俯仰、滚转、航向角
							Application::HUD::ParameterStore::getInstance()->setPitch((((double)receivedDataBuffer[0])/30000)*90);
							Application::HUD::ParameterStore::getInstance()->setRoll((((double)receivedDataBuffer[1])/30000)*180);
							Application::HUD::ParameterStore::getInstance()->setYaw((((double)receivedDataBuffer[2])/30000)*180);
							//为了简单我就把航向角的范围设计成了-180到180的范围，否则真的就太复杂了，
							//脑力不够，如果真的要实现可能是不是得设计一个循环链表…………想想就觉得很可怕
							break;
						case 2:
							//把终端2设置为大气计算机吧，这个也是有着明显意义的东西
							//给出高度、真空速/指示空速和马赫数三个量
							Application::HUD::ParameterStore::getInstance()->setHeight((double)((unsigned short int)receivedDataBuffer[0]));
							Application::HUD::ParameterStore::getInstance()->setSpeed((double)((unsigned short int)receivedDataBuffer[1]));
							Application::HUD::ParameterStore::getInstance()->setMach((UINT16)receivedDataBuffer[2]);
							break;
						case 3:
							//把终端3设置成…………无线电高度计？？才一个参数有意义吗
							//唉那就设置成ATL吧…………至少少一个把柄
							Application::HUD::ParameterStore::getInstance()->setAltitude((double)((unsigned short int)receivedDataBuffer[0]));
							//这个只有一个参数
							break;
						}
						

						//好困啊，现在是2019.1.17凌晨2:40，还有6个小时就要验收了，应该可以完成任务吧
					}
				}
			}
			//下面这个实现本来是RT的，现在还是觉得BC比较合适一些
			//其实也是也是因为老师要求用多个终端
			DWORD WINAPI receiveDataFrom1553B_legacy(LPVOID lpParam)
			{
				//0.初始化板卡
				unsigned short int deviceNumber, moduleNumber;
				/*
				puts("Default device number for single board when ExcConfig not used:\n");
				puts("M4k-1553MCH -> 25, PCI/MCH -> 29\n");

				printf("Enter the device number of the card you wish to use.\n");
				scanf("%hu", &deviceNumber);
				printf("Enter the module number.\n");
				scanf("%hu", &moduleNumber);
				printf("Calling Init_Module_Mch with device %hu module %hu\n", deviceNumber, moduleNumber);
				*/

				deviceNumber = 29;
				moduleNumber = 0;
				//这是固定的，不需要改动

				int errorCode;
				//char errorString[255];

				int handlerNumber = Init_Module_MCH(deviceNumber, moduleNumber);
				/*
				if (handlerNumber < 0)
				{
					//有错误发生  错误码就是上一个函数返回的负数
					errorCode = handlerNumber;
					Get_Error_String_MCH(errorCode, 200, errorString);
					//printf("Init_Module_MCH Failure. %s\n", errorString);
					//system("pause");
					//exit(0);		//在初始化时失败，故不需要释放资源
				}*/

				if(handlerNumber<0)
				{
					displayError(handlerNumber,handlerNumber,false);
				}

				//1.设置模式
				short int macroDefinedMode = RT_MODE_MCH;
				errorCode = Set_Mode_MCH(handlerNumber, macroDefinedMode);
				if (errorCode < 0)
				{
					/*
					Get_Error_String_MCH(errorCode, 200, errorString);
					//printf("%s\n", errorString);
					Release_Module_MCH(handlerNumber);
					system("pause");
					exit(0);
					*/
					displayError(handlerNumber,errorCode);
				}
				//todo
				//设置当前远程终端地址
				printf("请设置当前远程终端的地址\n");
				short int remoteTerminalAddress = 4;
				errorCode = Set_RT_Num_MCH(handlerNumber, remoteTerminalAddress);
				if (errorCode < 0)
				{
					/*
					Get_Error_String_MCH(errorCode, 200, errorString);
					//printf("%s\n", errorString);
					Release_Module_MCH(handlerNumber);
					system("pause");
					exit(0);
					*/
					displayError(handlerNumber,errorCode);
				}

				//设置子地址
				unsigned short int subAddress = 4;
				//这个子地址在主循环中还要使用

				//设置这个子地址产生接收中断
				errorCode = Set_Subaddr_Int_MCH(handlerNumber, subAddress, RECEIVE, STANDARD_CMD, ACCESS_AND_BROADCAST);
				if (errorCode < 0)
				{
					/*
					Get_Error_String_MCH(errorCode, 200, errorString);
					//printf("%s\n", errorString);
					Release_Module_MCH(handlerNumber);
					system("pause");
					exit(0);
					*/
					displayError(handlerNumber,errorCode);
				}

				//允许子地址产生中断
				/* Allow interrupts on sub address access. This is called once. */
				errorCode = Set_Interrupt_Mask_MCH(handlerNumber, SUBAD);
				if (errorCode < 0)
				{
					/*
					Get_Error_String_MCH(errorCode, 200, errorString);
					//printf("%s\n", errorString);
					Release_Module_MCH(handlerNumber);
					system("pause");
					exit(0);
					*/
					displayError(handlerNumber,errorCode);
				}

				errorCode = Run_RT_MCH(handlerNumber);
				if (errorCode < 0)
				{
					/*
					Get_Error_String_MCH(errorCode, 200, errorString);
					//printf("%s\n", errorString);
					Release_Module_MCH(handlerNumber);
					system("pause");
					exit(0);
					*/
					
					displayError(handlerNumber,errorCode);
				}

				short int dataBuffer[64] = {0};
				//设置完毕  进入主循环
				while (true)
				{
					Wait_For_Interrupt_MCH(handlerNumber, INFINITE);
					//Read_Datablk_MCH(handlerNumber, subAddress, 0, dataBuffer, STANDARD_CMD, RECEIVE);
					Read_Datablk_MCH(handlerNumber, subAddress, 0, (unsigned short int *)dataBuffer, STANDARD_CMD, RECEIVE);
					//BYTE i = 0;
					//for (i = 0; i < 32; i++) printf("%hu ", dataBuffer[i+2]);
					//printf("\n");

					//todo

					Application::HUD::ParameterStore::getInstance()->setPitch(((double)dataBuffer[2]/30000)*90);
					Application::HUD::ParameterStore::getInstance()->setRoll(((double)dataBuffer[3]/30000)*180);
				}
			}


			void displayError(int handlerNumber,int errorCode,bool releaseCard /* = true */)
			{
				//
				if (errorCode < 0)
				{
					char errorString[255];
					Get_Error_String_MCH(errorCode, 200, errorString);
					MessageBoxA(0,errorString,0,0);
					if(releaseCard) Release_Module_MCH(handlerNumber);
					//system("pause");
					//exit(0);
				}
			}
			unsigned short int combine1553BCommand(
				unsigned char remoteTerminalAddress, bool isTx, unsigned char subAddress, unsigned char dataLength)
			{
				unsigned short int temp16bit;
				unsigned short int commandWord;

				temp16bit =(unsigned short int) (remoteTerminalAddress&0x1f);//取其低5位
				commandWord = temp16bit << 11;

				temp16bit = (unsigned short int)(subAddress & 0x1f);//取低5位
				temp16bit = temp16bit << 5;
				commandWord |= temp16bit;

				temp16bit = (unsigned short int)(dataLength & 0x1f);//取低5位
				commandWord |= temp16bit;

				if (isTx)
				{
					//发送
					commandWord |= 0x0400;
				}
				else
				{
					//其他情况下默认作为接收
					commandWord &= (~0x0400);
				}

				return commandWord;
			}
		}
	}
}