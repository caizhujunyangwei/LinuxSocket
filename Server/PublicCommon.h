#ifndef __PUBLIC_COMMON_H__
#define __PUBLIC_COMMON_H__
/**************************************************************************************
 * FILE:publiccommon.h
 * DATE:2017/12/11
 * AUTH:YangW
 * INTR:程序中常用的一些定义，声明
**************************************************************************************/
#define Server_Version		"0.0.0"

//一些锁的定义
#define Log_Lock			-100
#define Lock_NUPool			-101
#define Lock_AllSockets		-102
#define Lock_DeleteSockets	-103

#ifndef SOCKET
#define SOCKET	int			  
#endif // !SOCKET

#define MAX_EPOLL_NUM		2			//服务器线程池最大监听epoll数量
#define SERVER_THREAD_NUM	2			//服务器线程池数量


#define MAX_RECV_SIZE		1024 * 5  //收到SOCKET消息最大大小

#define ServerPort			6080
#define MaxRecvNum			1000	//最大接受人数

//套接字类型
enum SOCKET_TYPE
{
	SOCKET_TYPE_NULL = 0x00,	//无
	SOCKET_TYPE_CLIENT = 0x01,	//客户端
	SOCKET_TYPE_SERVER = 0x02, //服务器
};

//连接套接字客户端状态
enum SOCKET_STATE
{
	SOCKET_STATE_NULL = 0x00,	//无
	SOCKET_STATE_CONNECTED = 0x01,	//连接成功正常状态
	SOCKET_STATE_DISCONNECTED = 0x02, //离线状态
	SOCKET_STATE_WAIT_FINISH = 0x04, //等待结束状态，没有什么可做的等待释放
};

//delete删除地址
#define SAFE_DELETE(_TYPE_)															\
do{																					\
	if (_TYPE_){																	\
		delete _TYPE_;																\
		_TYPE_ = nullptr;															\
	}																				\
}while(_TYPE_);																			

//delet删除组
#define SAFE_DELETE_ARRAY(_TYPE_)													\
do{																					\
	if (_TYPE_){																	\
		delete[] _TYPE_;															\
		_TYPE_ = nullptr;															\
	}																				\
}while(_TYPE_);		

//工厂宏
#define CREATE_FUNC(_TYPE_)															\
static _TYPE_* create(){															\
	_TYPE_* pRet = new (std::nothrow) _TYPE_();										\
	if(pRet == nullptr || !pRet->init()){											\
		delete pRet;																\
		pRet = nullptr;																\
	}																				\
	return pRet;																	\
}																		

//生成变量的get，set方法
#define SYNTHESIZE(varType, varName, funName)										\
protected: varType varName;															\
public: virtual varType get##funName(void) const { return varName; }				\
public: virtual void set##funName(varType var) { varName = var; }	


#endif // !__PUBLIC_COMMON_H__
