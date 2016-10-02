#include"stdafx.h"

void AsynchronousClientClass::ProcessWinMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	// 오류 발생 여부 확인
	if (WSAGETSELECTERROR(lParam)) {
		error_display("ProcessWinMessage::", WSAGETSELECTERROR(lParam), __LINE__);
		return;
	}

	// 메세지 처리
	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_READ:	// 데이터 수신
		//retval = recvn();

		DWORD ioByteSize;
		retval = WSARecv(sock, &wsa_recvbuf, 1, NULL, &ioByteSize, 0, NULL);
		if (SOCKET_ERROR == retval) {
			int err_no = WSAGetLastError();
			error_display("FD_READ::", err_no, __LINE__);
			return;
		}
		recvbytes = retval;

		// recvbuf 에 받은 내용을 토대로 데이터 처리
		processPacket();

		break;
	case FD_WRITE:	// 데이터 송신
		break;
	case FD_CLOSE:	// 서버에서 통신을 끊음
		break;
	default:
		break;
	}
}