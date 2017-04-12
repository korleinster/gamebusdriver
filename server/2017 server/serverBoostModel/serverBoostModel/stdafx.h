// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include <iostream>
#include <cstdio>
#include <vector>
#include <thread>
using namespace std;

// 시야리스트 관리용 STL
#include <unordered_set>
#include <mutex>
#include <queue>

// boost asio 클라와 연동되는 protocol.h
#include "protocol.h"

// boost asio 관련 헤더
#include <memory>
#include <boost\asio.hpp>

extern boost::asio::io_service g_io_service;

#include "DB.h"
#include "TimerQueue.h"
#include "player_sesstion.h"
#include "boostAsioServer.h"

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.

// 플레이어가 담긴 변수
extern mutex g_clients_lock;
extern vector<player_session*> g_clients;
//static AI_session g_AIs[MAX_AI_NUM];

// DB 통신용 변수
extern DB database;
extern TimerQueue time_queue;