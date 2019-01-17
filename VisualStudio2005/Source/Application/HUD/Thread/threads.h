#pragma once
#include <windows.h>

//DWORD WINAPI thread(LPVOID lpParam);
/*
typedef PTHREAD_START_ROUTINE LPTHREAD_START_ROUTINE;
……
typedef DWORD (WINAPI *PTHREAD_START_ROUTINE)(
LPVOID lpThreadParameter
);
*/
//CreateThread的参数LPTHREAD_START_ROUTINE规定了线程函数必须是这个格式
//所以传参只能通过一个map传递参数

//调用格式	DWORD threadId;
//无参数	HANDLE threadHandle = CreateThread(NULL, 0, thread, NULL, 0, &threadId);
//带参数	HANDLE threadHandle = CreateThread(NULL, 0, thread, (LPVOID *)(), 0, &threadId);

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