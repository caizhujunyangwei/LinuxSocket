#include "GameServer.h"
#include "../Utils/Log.h"

GameServer::GameServer(unsigned short _port, unsigned int max)
	:
	fd(0),
	sendnum(0),
	recvnum(0),
	serveraddr(nullptr),
	epfd(-1),
	isRun(false),
	m_nupool(nullptr)
{
	setPort(_port);
	setMaxRecvNum(max);
}

GameServer::~GameServer()
{
	close(fd);
	SAFE_DELETE(this->serveraddr);
}

GameServer * GameServer::create(unsigned short _port, unsigned int max)
{
	Log::Info("Create GameServer Port is %d,MaxRecv is %d", _port, max);
	
	GameServer* pRet = new GameServer(_port, max);
	if (!pRet || !pRet->init())
		SAFE_DELETE(pRet);

	return pRet;
}

bool GameServer::init()
{
	if (this->port < 1000 || this->maxrecv < 10)
	{
		Log::Error("Port or MaxRecv is invalid!");
		return false;
	}

	this->serveraddr = new sockaddr_in();
	this->fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	int opts;
	opts = fcntl(this->fd, F_GETFL);
	if (opts < 0) {
		Log::Error("fcntl(sock,GETFL)");
		return false;
	}
	opts = opts | O_NONBLOCK;
	if (fcntl(this->fd, F_SETFL, opts) < 0) {
		Log::Error("fcntl(sock,SETFL,opts)");
		return false;
	}

	/*int optionVal = 0;
	setsockopt(this->fd, SOL_SOCKET, SO_REUSEADDR, &optionVal, sizeof(optionVal));*/

	memset(this->serveraddr, 0, sizeof(sockaddr_in));
	this->serveraddr->sin_addr.s_addr = htons(INADDR_ANY);
	this->serveraddr->sin_family = AF_INET;
	this->serveraddr->sin_port = htons(this->port);

	int err = bind(this->fd, (sockaddr*)this->serveraddr, sizeof(sockaddr));
	if (0 != err)
	{
		Log::Error("bind server socket failed!,error[%d],%s\n",err,strerror(err));
		return false;
	}


	//开始加载其他模块
	//加载服务器线程池
	m_nupool = new NUPool(SERVER_THREAD_NUM, "Server NUPool start", this);
	if (!m_nupool) {
		return false;
	}
	
	return true;
}

bool GameServer::run()
{
	epfd = epoll_create1(0);

	int r = listen(this->getListenFD(), 2);
	Log::Info("GameServer Listen ret is %d",r);

	struct epoll_event ev;
	//设置与要处理的事件相关的文件描述符
	ev.data.fd = this->getListenFD();
	//设置要处理的事件类型
	ev.events = EPOLLIN | EPOLLET;
	//ev.events=EPOLLIN;
	//注册epoll事件
	epoll_ctl(epfd, EPOLL_CTL_ADD, this->getListenFD(), &ev);

	//开启线程池
	m_nupool->run(true);

	this->isRun = true;
	return true;
}

bool GameServer::stop()
{
	this->isRun = false;
	delete this;
	return true;
}

bool GameServer::sendSocketMsg(SOCKET socket, unsigned char * msg, unsigned int len, bool needClean)
{
	bool ret = true;
	if (socket > 0) {
		int Index = 0;
		int hasWrite = 0;

		while (true)
		{
			Index = write(socket, msg + hasWrite, len - hasWrite);
			if (Index == -1 || Index == 0 || (hasWrite += Index) >= len)
				break;
		}
		this->sendnum += 1; //已发送+1

		Log::Info("send msg success which length is %d", len);
	}
	else {
		Log::Error("sendMsg socket is invalid!");

		ret = false;
	}

	if (needClean) {
		SAFE_DELETE_ARRAY(msg);
	}

	return ret;
}