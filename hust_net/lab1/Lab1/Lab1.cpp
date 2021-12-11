#include<thread>
#include<mutex> 
#include<condition_variable>
#include<queue>
#include "socketLib.h"
#pragma comment(lib,"ws2_32.lib")

#define MAX_THREAD_NUM 100
#define CONFIG_FILE_PATH "D:/config.txt"

using namespace std;
mutex resource_mutex;
char filename[NAME_BUFF_SIZE];
queue<unsigned int> client_queue = queue<unsigned int>();
void thread_function(int n);

int main(void) {
	int thread_num = 0;
	WSADATA wsadata;
	//初始化WSA
	error(WSAStartup(0x0202, &wsadata), WSAEINVAL, "Fail To Start up WSA\n");

	//创建监听socket
	SOCKET server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	error(server_socket, INVALID_SOCKET, "Fail To Create The listen_socket\n");

	//读取配置文件
	int port = 0;
	char inaddr[20] = "";
	std::ifstream config(CONFIG_FILE_PATH);
	if (!config.is_open()) {
		printf("InVaild Config File Path:%s\n",CONFIG_FILE_PATH);
		return -1;
	}

	config >> port;
	config >> inaddr;
	config >> filename;

	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;//ipv4
	//换成网络字节序
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = inet_addr(inaddr);


	int condition = bind(server_socket, (LPSOCKADDR)&server_addr, sizeof(server_addr));
	error(condition, SOCKET_ERROR, "Fail To Bind The listen_socket！\n");

	//监听
	condition = listen(server_socket, 5);
	error(condition, SOCKET_ERROR, "设置等待连接状态失败！\n");


	sockaddr_in client_addr;
	int cllen = sizeof(client_addr);
	while (1) {
		SOCKET client_socket = accept(server_socket, (LPSOCKADDR)&client_addr, &cllen);
		if (client_queue.size() >= MAX_THREAD_NUM) {
			printf("Too Much Connection!\nHave To Throw Socket:%d\n", client_socket);
			continue;
		}
		printf("client %d     IP:%s  port: %d\n",client_socket,inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
		error(client_socket, INVALID_SOCKET, "连接失败！\n");
		resource_mutex.lock();
		client_queue.push(client_socket);
		resource_mutex.unlock();

		thread ask(thread_function, thread_num);
		ask.detach();
		thread_num = (thread_num + 1) % MAX_THREAD_NUM;
	}
}
void thread_function(int n) {

	resource_mutex.lock();
	SOCKET client_id = client_queue.front();
	client_queue.pop();
	resource_mutex.unlock();
	if (client_id != NULL) {
		resource_mutex.lock();
		printf("开始处理client %d 的请求\n", client_id);
		resource_mutex.unlock();
		handle_connection(client_id, filename);
		closesocket(client_id);
	}
	return ;
}