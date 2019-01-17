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

			//�ú����������������
			unsigned short int combine1553BCommand(
				unsigned char remoteTerminalAddress, bool isTx, unsigned char subAddress, unsigned char dataLength);


			DWORD WINAPI receiveDataFrom1553B(LPVOID lpParam)
			{
				//0.��ʼ���忨
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
				//���ǹ̶��ģ�����Ҫ�Ķ�

				int errorCode;
				//char errorString[255];

				int handlerNumber = Init_Module_MCH(deviceNumber, moduleNumber);
				/*
				if (handlerNumber < 0)
				{
					//�д�����  �����������һ���������صĸ���
					errorCode = handlerNumber;
					Get_Error_String_MCH(errorCode, 200, errorString);
					//printf("Init_Module_MCH Failure. %s\n", errorString);
					//system("pause");
					//exit(0);		//�ڳ�ʼ��ʱʧ�ܣ��ʲ���Ҫ�ͷ���Դ
				}*/

				if(handlerNumber<0) displayError(handlerNumber,handlerNumber,false);

				//1.����ģʽ
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
				//���ǿ���ԭ���������ߵĴ���һ��һ���ĳ����ġ�������
				CMDENTRY commandEntry;

				for(UINT i = 0;i<=24;i++)
				{
					//Ҫ���ն�i����20���ֵ����ݣ��洢����i��data block
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

				//д����������ʶ��������ϵͳ���ھ����ò�����= =

				errorCode = Set_Interrupt_Mask_MCH(handlerNumber, 0x0002);    //ÿһ������鱻ִ��  ������һ���ж�
				if (errorCode < 0) displayError(handlerNumber,errorCode);

				//��ʼ����
				errorCode = Run_BC_MCH(handlerNumber);
				if (errorCode < 0) displayError(handlerNumber,errorCode);

				short int receivedDataBuffer[32];
				//�����ݴ���յ�������
				while (true)
				{
					Wait_For_Interrupt_MCH(handlerNumber, INFINITE);
					unsigned long int interruptCount;
					Get_Interrupt_Count_MCH(handlerNumber, &interruptCount);
					//printf("%lu ", interruptCount);
					//��ȡ���ݿ��е����ݲ����Դ���

					for(UINT8 dataBlockIndex = 1;dataBlockIndex<=20;dataBlockIndex++)
					{
						errorCode = Read_BC_Datablk_MCH(handlerNumber,dataBlockIndex,20,(unsigned short int *)receivedDataBuffer);
						if(errorCode<0) displayError(handlerNumber,errorCode);
						if(errorCode<0) exit(-1);
						//Ȼ��͵��������

						switch(dataBlockIndex)
						{
						case 1:
							//���ն�1��Ϊ���Ե���INS�ɡ���������Ϊ���������Ĳ���
							//�����Ǹ�������ת�������
							Application::HUD::ParameterStore::getInstance()->setPitch((((double)receivedDataBuffer[0])/30000)*90);
							Application::HUD::ParameterStore::getInstance()->setRoll((((double)receivedDataBuffer[1])/30000)*180);
							Application::HUD::ParameterStore::getInstance()->setYaw((((double)receivedDataBuffer[2])/30000)*180);
							//Ϊ�˼��ҾͰѺ���ǵķ�Χ��Ƴ���-180��180�ķ�Χ��������ľ�̫�����ˣ�
							//����������������Ҫʵ�ֿ����ǲ��ǵ����һ��ѭ����������������;��úܿ���
							break;
						case 2:
							//���ն�2����Ϊ����������ɣ����Ҳ��������������Ķ���
							//�����߶ȡ������/ָʾ���ٺ������������
							Application::HUD::ParameterStore::getInstance()->setHeight((double)((unsigned short int)receivedDataBuffer[0]));
							Application::HUD::ParameterStore::getInstance()->setSpeed((double)((unsigned short int)receivedDataBuffer[1]));
							Application::HUD::ParameterStore::getInstance()->setMach((UINT16)receivedDataBuffer[2]);
							break;
						case 3:
							//���ն�3���óɡ����������ߵ�߶ȼƣ�����һ��������������
							//���Ǿ����ó�ATL�ɡ�������������һ���ѱ�
							Application::HUD::ParameterStore::getInstance()->setAltitude((double)((unsigned short int)receivedDataBuffer[0]));
							//���ֻ��һ������
							break;
						}
						

						//��������������2019.1.17�賿2:40������6��Сʱ��Ҫ�����ˣ�Ӧ�ÿ�����������
					}
				}
			}
			//�������ʵ�ֱ�����RT�ģ����ڻ��Ǿ���BC�ȽϺ���һЩ
			//��ʵҲ��Ҳ����Ϊ��ʦҪ���ö���ն�
			DWORD WINAPI receiveDataFrom1553B_legacy(LPVOID lpParam)
			{
				//0.��ʼ���忨
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
				//���ǹ̶��ģ�����Ҫ�Ķ�

				int errorCode;
				//char errorString[255];

				int handlerNumber = Init_Module_MCH(deviceNumber, moduleNumber);
				/*
				if (handlerNumber < 0)
				{
					//�д�����  �����������һ���������صĸ���
					errorCode = handlerNumber;
					Get_Error_String_MCH(errorCode, 200, errorString);
					//printf("Init_Module_MCH Failure. %s\n", errorString);
					//system("pause");
					//exit(0);		//�ڳ�ʼ��ʱʧ�ܣ��ʲ���Ҫ�ͷ���Դ
				}*/

				if(handlerNumber<0)
				{
					displayError(handlerNumber,handlerNumber,false);
				}

				//1.����ģʽ
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
				//���õ�ǰԶ���ն˵�ַ
				printf("�����õ�ǰԶ���ն˵ĵ�ַ\n");
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

				//�����ӵ�ַ
				unsigned short int subAddress = 4;
				//����ӵ�ַ����ѭ���л�Ҫʹ��

				//��������ӵ�ַ���������ж�
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

				//�����ӵ�ַ�����ж�
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
				//�������  ������ѭ��
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

				temp16bit =(unsigned short int) (remoteTerminalAddress&0x1f);//ȡ���5λ
				commandWord = temp16bit << 11;

				temp16bit = (unsigned short int)(subAddress & 0x1f);//ȡ��5λ
				temp16bit = temp16bit << 5;
				commandWord |= temp16bit;

				temp16bit = (unsigned short int)(dataLength & 0x1f);//ȡ��5λ
				commandWord |= temp16bit;

				if (isTx)
				{
					//����
					commandWord |= 0x0400;
				}
				else
				{
					//���������Ĭ����Ϊ����
					commandWord &= (~0x0400);
				}

				return commandWord;
			}
		}
	}
}