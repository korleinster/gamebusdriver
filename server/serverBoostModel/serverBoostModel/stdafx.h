// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once
#pragma comment(lib, "winmm.lib")

#include <iostream>
#include <cstdio>
#include <vector>
#include <thread>
using namespace std;

// boost asio 클라와 연동되는 protocol.h
#include"protocol.h"

// boost asio 관련 헤더
#include <memory>
#include"boostAsioServer.h"

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
