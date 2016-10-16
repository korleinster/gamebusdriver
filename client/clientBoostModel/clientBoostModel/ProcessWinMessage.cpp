#pragma once
#include"stdafx.h"

void boostAsioClient::ProcessWinMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// 오류 발생 여부 확인
	/*if (WSAGETSELECTERROR(lParam)) {
	error_display("ProcessWinMessage::", WSAGETSELECTERROR(lParam), __LINE__);
	return;
	}*/

	// 메세지 처리
	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_READ: {	// 데이터 수신
		boost::system::error_code error = boost::asio::error::host_not_found;
		m_socket->read_some(boost::asio::buffer(m_recvBuf), error);

		// recvbuf 에 받은 내용을 토대로 데이터 처리
		processPacket();
	}
				  break;
	case FD_WRITE:	// 데이터 송신
		break;
	case FD_CLOSE:	// 서버에서 통신을 끊음
		break;
	default:
		break;
	}
}