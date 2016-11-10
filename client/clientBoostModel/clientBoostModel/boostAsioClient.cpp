#pragma once
#include"stdafx.h"

boostAsioClient::boostAsioClient()
{
}

boostAsioClient::~boostAsioClient()
{
	// Init()
	if (nullptr != m_resolver)	delete m_resolver;
	if (nullptr != m_query)		delete m_query;
	if (nullptr != m_endpoint)	delete m_endpoint;
	if (nullptr != m_socket)	delete m_socket;
}

void boostAsioClient::Init(const HWND& hwnd)
{
	m_hWnd = hwnd;
	inputServerIP();

	using boost::asio::ip::tcp;
	m_resolver = new tcp::resolver(m_io_service);
	m_query = new tcp::resolver::query(m_serverIP/*boost::asio::ip::host_name()*/, "");

	m_endpoint = new tcp::endpoint(boost::asio::ip::address::from_string(m_serverIP), SERVERPORT);
	m_endpoint_iterator = m_resolver->resolve(*m_query);

	m_socket = new tcp::socket(m_io_service);

	// Error Detecting - 없어도 되는 듯 하다. 최신 버전에는 이 부분이 빠져있다.
	/*tcp::resolver::iterator end;
	boost::system::error_code error = boost::asio::error::host_not_found;
	while (error && m_endpoint_iterator != end) {
	m_socket->close();
	m_socket->connect(*m_endpoint_iterator++, error);
	}
	if (error) { throw boost::system::system_error(error); }*/

	//boost::asio::connect(*m_socket, m_endpoint_iterator);
	m_socket->async_connect(*m_endpoint, [&](const boost::system::error_code& error_code) {
#ifdef _DEBUG
		if (error_code) { cout << "connect failed : " << error_code.message() << "\n"; }
		else { cout << "\n\n\nServer Connected !!\n\n\n"; /*system("cls");*/ }
#endif // _DEBUG
		boostAsioClient::sendPacket_TEST();
	});

	// 초기화를 위해서 서버에서 접속한 후에, 한번 데이터를 받아와야 한다.

	m_io_service.run();
}

void boostAsioClient::inputServerIP_ReadtxtFile()
{
	ifstream getServerIP;
	getServerIP.open("ServerIP.txt", ios::in);
	getServerIP.getline(m_serverIP, 32);
	getServerIP.close();
}

void boostAsioClient::inputServerIP_cin()
{
	cout << "\nexample 127.0.0.1\nInput Server's IP : ";
	cin >> m_serverIP;
}

void boostAsioClient::inputServerIP()
{
#ifdef _DEBUG
	cout << "choose Server to connect.\n\n0. Read Server IP in .txt File\n1. Input Server IP, directly\n\nchoice method : ";
	short cmd{ 0 };
	cin >> cmd;

	if (cmd & 1) { inputServerIP_cin(); }
	else { inputServerIP_ReadtxtFile(); }

	cout << "\n\n\t--==** Connecting Server, Please Wait **==--\n\n\n";
#else
	inputServerIP_ReadtxtFile();
#endif
}

void boostAsioClient::sendPacket_TEST()
{
	m_sendBuf[0] = 3;
	m_sendBuf[1] = TEST;
	m_sendBuf[2] = 1;

	/*boost::system::error_code ignored_error;
	boost::asio::write(*m_socket, boost::asio::buffer(m_sendBuf), boost::asio::transfer_all(), ignored_error);*/

	boost::asio::async_write(*m_socket, boost::asio::buffer(m_sendBuf, m_sendBuf[0]), [&](const boost::system::error_code& error_code, size_t bytes_transferred) {
		
	});
}