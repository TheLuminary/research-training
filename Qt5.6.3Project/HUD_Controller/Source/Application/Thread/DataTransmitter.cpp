#include "DataTransmitter.h"

#include "../ParameterStore.h"
#include "../MIL-STD-1553B/Proto_mch.h"

#include <QMessageBox>

DataTransmitter::DataTransmitter(quint8 address, quint8 subAddress)
{
    this->address = address;
    this->subAddress = subAddress;
    //设置地址和子地址
}


void displayError(int handlerNumber,int errorCode,bool releaseCard = true);

void DataTransmitter::run()
{
    //跑起来~
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
    short int remoteTerminalAddress = this->address;
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
    unsigned short int subAddress = this->subAddress;
    //这个子地址在主循环中还要使用

    //设置这个子地址产生发送中断
    errorCode = Set_Subaddr_Int_MCH(handlerNumber, subAddress, TRANSMIT, STANDARD_CMD, ACCESS);
    if (errorCode < 0) displayError(handlerNumber,errorCode);

    //允许子地址产生中断
    /* Allow interrupts on subaddress access. This is called once. */
    errorCode = Set_Interrupt_Mask_MCH(handlerNumber, SUBAD);
    if (errorCode < 0) displayError(handlerNumber,errorCode);

    errorCode = Run_RT_MCH(handlerNumber);
    if (errorCode < 0) displayError(handlerNumber,errorCode);

    WORD dataBuffer[64];
    //初次装载数据
    this->loadData(this->address,dataBuffer);
    //dataBuffer[0] = (WORD)(ParameterStore::getInstance()->getPitch());
    //dataBuffer[1] = (WORD)(ParameterStore::getInstance()->getRoll());
    //qDebug("%hu,%hu\n",dataBuffer[0],dataBuffer[1]);
    Load_Datablk_MCH(handlerNumber, subAddress, 0, dataBuffer);
    //设置完毕  进入主循环
    while (true)
    {
        Wait_For_Interrupt_MCH(handlerNumber, INFINITE);

        //在这里要针对不同的终端地址采取不同的措施
        //根据终端地址的不同，可以识别现在是作为INS还是ADC，抑或是ALT

        this->loadData(this->address,dataBuffer);
        Load_Datablk_MCH(handlerNumber, subAddress, 0, dataBuffer);
    }
}

void DataTransmitter::loadData(quint8 address, unsigned short int *dataBuffer)
{
    switch(address)
    {
    case 1:
        //装载INS数据
        dataBuffer[0] = (UINT16)ParameterStore::getInstance()->getPitch();
        dataBuffer[1] = (UINT16)ParameterStore::getInstance()->getRoll();
        dataBuffer[2] = (UINT16)ParameterStore::getInstance()->getYaw();
        break;
    case 2:
        //ADC
        dataBuffer[0] = (UINT16)ParameterStore::getInstance()->getHeight();
        dataBuffer[1] = (UINT16)ParameterStore::getInstance()->getSpeed();
        dataBuffer[2] = (UINT16)ParameterStore::getInstance()->getMach();
        break;
    case 3:
        //ATL
        dataBuffer[0] = (UINT16)ParameterStore::getInstance()->getAltitude();
        break;
    }

    qDebug("%hu,%hu,%hu",dataBuffer[0],dataBuffer[1],dataBuffer[2]);
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

        qDebug("%s",errorString);
    }
}
