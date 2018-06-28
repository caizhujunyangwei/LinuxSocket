#include "./Server/GameServer.h"
#include "./Utils/ThirdParty/MD5.h"
#include "./Utils/ThirdParty/Base64.h"
#include "./Module/ModuleThread.h"
#include "./Server/SocketControl.h"
using namespace tinyxml2;


GameServer* g_server = nullptr;

int main(int argc, char *argv[])
{
	ModuleLock::getInstance();
	tm t = ModuleTime::getLocalTime();
	Log::Info("开启服务器");
	Log::Warn("Server Run Time : %s", ModuleTime::convert2ASCTime(t));
	Log::Warn("Server Version is %s", Server_Version);

	//初始化服务器
	g_server = GameServer::create(ServerPort, MaxRecvNum);
	if (g_server) {
		auto socketController = SocketControl::getInstance();
		g_server->run();


		//游戏主循环
		unsigned long long trigger = 0;
		while (++trigger) {

			//Log::Info("time for loop : %ld, trigger: %ld", ModuleTime::getLocalTimeNumber(), trigger);
			socketController->dealDeleteSockets();

			//500ms循环一次
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}
	}



	//END
	if (g_server) {
		SAFE_DELETE(g_server);
	}

	return 1;
}