#pragma once
#include"stdafx.h"

void boostAsioClient::processPacket()
{
	switch (m_recvBuf[1])
	{
	case TEST:
#ifdef _DEBUG
		cout << "Server is Running. TEST Packet Recived Successfully.\n";
#endif
		break;
	case KEYINPUT:
		break;
	default:
		break;
	}
}
