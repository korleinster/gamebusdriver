#pragma once
#pragma comment(lib, "ws2_32")

#include"protocol.h"

#include<WinSock2.h>
#include<Windows.h>
#include<stdlib.h>

#include<iostream>
#include<thread>
#include<vector>




// iocp 서버의 몸통이 되는 가장 핵심적인 클래스의 선언이 들어 있다.
#include"iocpServerClass.h"