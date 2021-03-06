// serverBoostModel.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

boost::asio::io_service g_io_service;

// 플레이어가 담긴 변수
mutex g_clients_lock;
vector<player_session*> g_clients;

// DB 통신용 변수
DB g_database;
TimerQueue g_time_queue;

// protocol 통신 페이지 & boostAsioServer.cpp 에서 값 갱신해준다.
int MAX_AI_SLIME;
int MAX_AI_GOBLIN;
int MAX_AI_NUM;
int MAX_AI_BOSS;

int main()
{
	try
	{
		_wsetlocale(LC_ALL, L"korean");
		boostAsioServer server;
	}
	catch (const std::exception& e)
	{
		std::cout << "Main Function's Exception = " << e.what() << "\n";
	}
}