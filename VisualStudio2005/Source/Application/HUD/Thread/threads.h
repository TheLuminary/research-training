#pragma once
#include <windows.h>

//DWORD WINAPI thread(LPVOID lpParam);
/*
typedef PTHREAD_START_ROUTINE LPTHREAD_START_ROUTINE;
����
typedef DWORD (WINAPI *PTHREAD_START_ROUTINE)(
LPVOID lpThreadParameter
);
*/
//CreateThread�Ĳ���LPTHREAD_START_ROUTINE�涨���̺߳��������������ʽ
//���Դ���ֻ��ͨ��һ��map���ݲ���

//���ø�ʽ	DWORD threadId;
//�޲���	HANDLE threadHandle = CreateThread(NULL, 0, thread, NULL, 0, &threadId);
//������	HANDLE threadHandle = CreateThread(NULL, 0, thread, (LPVOID *)(), 0, &threadId);

namespace Application
{
	namespace HUD
	{
		namespace Thread
		{
			DWORD WINAPI receiveMessageBySocket(LPVOID lpParam);
			DWORD WINAPI receiveDataFrom1553B(LPVOID lpParam);
		}
	}
}