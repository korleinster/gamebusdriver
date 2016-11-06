// serverBoostModel.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

/*// Session 클래스는 서버에 접속한 클라이언트
class Session {
public:
	Session(boost::asio::io_service &io_service) : m_Socket(io_service) {}

	boost::asio::ip::tcp::socket& Socket() { return m_Socket; }

	void PostReceive() {
		m_Socket.async_read_some(boost::asio::buffer(m_ReceiveBuffer), boost::bind(&Session::handle_receive, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}

private:

	void handle_write(const boost::system::error_code&, size_t) {}
	void handle_receive(const boost::system::error_code& error, size_t bytes_transferred) {
		if (error) {
			if (error == boost::asio::error::eof) {	cout << "클라이언트와 연결이 끊겼습니다.\n"; }
			else { cout << "에러 번호 : " << error.value() << " 에러 메세지 : " << error.message() << endl;	}
		}
		else {
			cout << "클라이언트에서 받은 메세지 : " << m_ReceiveBuffer << endl;

			// async_write ( 데이터를 보낼 클라이언트 소켓, 보낼 데이터가 담긴 버퍼, 전송 후 호출할 함수 )
			// bind ( 완료하면 호출할 함수 (여기선 handle_write), 함수를 멤버로 가지는 클래스의 인스턴스 (handle_write 자기자신), write 함수에 넘길 첫 인자, write 함수에 넘길 두번째 인자
			/// void handle_write(const boost::system::error_code&, size_t) = 첫 인자 Eroor 코드 , 두번째 인자 데이터 크기
			boost::asio::async_write(m_Socket, boost::asio::buffer(m_WritreMessage), boost::bind(&Session::handle_write, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
			
			PostReceive();
		}
	}

	// async_write 에서의 버퍼는, 데이터가 다 보내지기 전 ( 완료 함수가 불려지기 전 ) 까지는 값이 보존되어야 한다. ( 비동기 이기 때문에 )
	char m_WritreMessage[MAX_BUF_SIZE]{ 0 };
	char m_ReceiveBuffer[MAX_BUF_SIZE]{ 0 };
	boost::asio::ip::tcp::socket m_Socket;
};

class TCP_Server {
public:
	TCP_Server(boost::asio::io_service& io_service) : m_acceptor(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), SERVERPORT)) {
		m_pSession = nullptr;
		StartAccept();
	}

	~TCP_Server() {	if (m_pSession != nullptr) { delete m_pSession; }}

private:
	void StartAccept() {
		cout << "클라이언트 접속 대기....\n";

		m_pSession = new Session(m_acceptor.get_io_service());
		m_acceptor.async_accept(m_pSession->Socket(), boost::bind(&TCP_Server::handle_accept, this, m_pSession, boost::asio::placeholders::error));
	}

	void handle_accept(Session* pSession, const boost::system::error_code& error) {
		if (!error) { cout << "클라이언트 접속 성공\n"; pSession->PostReceive(); }
	}

	int m_nSeqNumber;
	boost::asio::ip::tcp::acceptor m_acceptor;
	Session* m_pSession;
};*/

int main()
{
	//// io_service 는 OS 커널과 프로그램의 중개 매개체 역할을 한다.
	//boost::asio::io_service io_service;
	//
	//// endpoint 는 네트워크 주소를 설정한다.
	//boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), SERVERPORT);
	//// acceptor 클래스는 클라이언트의 접속을 받아들이는 역할을 한다. 인자는 io_service 와 endpoint
	//boost::asio::ip::tcp::acceptor acceptor(io_service, endpoint);
	//
	//// socket 을 통해서, 클라이언트가 보낸 메시지를 주고 받아야 하므로, io_service 할당
	//boost::asio::ip::tcp::socket socket(io_service);
	//// accept 멤버 함수에, 클라이언트용 socket 객체를 넘겨주면, 서버가 클라이언트를 접속 받을 준비는 끝났다.
	///// 동기형 방식이라, 완료될 때 까지 대기
	//acceptor.accept(socket);
	//
	//std::cout << "클라이언트 접속\n";
	//
	//while (true)
	//{
	//	char buf[MAX_BUF_SIZE]{ 0 };
	//
	//	// error_code 클래스는 시스템에 발생한 에러코드를 랩핑한 클래스, 에러 발생 시 에러 코드와 에러 메세지 획득 가능
	//	boost::system::error_code error;
	//	// read_some 멤버 함수로 클라이언트가 보낸 데이터를 수신 ( 리턴은 데이터 값 )
	//	/// 동기형 방식이라, 데이터를 다 받을 때 까지 대기한다.
	//	size_t len = socket.read_some(boost::asio::buffer(buf), error);
	//
	//	if (error) {
	//		if (error == boost::asio::error::eof) {
	//			// 클라이언트의 접속이 끊어지는 경우, read_some 멤버 함수를 통해 알 수 있다. 이럴 땐 eof 가 설정이 됨.
	//			cout << "클라이언트와 연결이 끊김\n";
	//		}
	//		else {
	//			cout << "error No : " << error.value() << " error Messgage : " << error.message() << "\n";
	//		}
	//		break;
	//	}
	//	cout << "클라이언트에서 받은 메세지 : " << buf << "\n";
	//
	//	char szMessage[MAX_BUF_SIZE]{ 0 };
	//	// 버퍼에 값을 출력 ( 또는 대입 ) 하는 함수
	//	sprintf_s(szMessage, MAX_BUF_SIZE - 1, "Re : %s", buf);
	//
	//	int nMsgLen = strnlen_s(szMessage, MAX_BUF_SIZE - 1);
	//
	//	boost::system::error_code ignored_error;
	//	socket.write_some(boost::asio::buffer(szMessage, nMsgLen), ignored_error);
	//	cout << "클라이언트에 보낸 메세지 : " << szMessage << "\n";
	//} // 여기까지 동기 형식의 에코 서버
	
	//boost::asio::io_service io_service;
	//TCP_Server server(io_service);
	//// io_service 의 run 함수는 비동기 요청이 있을 경우, 요청이 끝날 때 까지 무한히 대기 -> 비동기 요청을 하기 전에 run() 을 한다면, 그냥 종료되어 버린다.
	///// 남은 요청이 없다면, 빠져나와 다음 작업을 진행
	//io_service.run();
	//cout << "네트워크 접속 종료\n";

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