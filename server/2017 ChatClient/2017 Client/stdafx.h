#pragma once

#pragma comment(lib, "ws2_32")

#include <WinSock2.h>
#include <windows.h>
#include <iostream>
using namespace std;

#include <unordered_map>

#include "../../2017 server/serverBoostModel/serverBoostModel/protocol.h"
#include "resource.h"
#define WM_SOCKET (WM_USER + 1)

#include "player_class.h"
#include "network_class.h"
#include "win_main_class.h"
