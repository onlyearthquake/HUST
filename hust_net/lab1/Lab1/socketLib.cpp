#pragma once
#include "socketLib.h"
#pragma comment(lib,"ws2_32.lib")
using namespace std;
int error(int backdata, int errordata, string printword)
{
	//简单报错 sockt库很多函数会用到
	if (backdata == errordata)
	{
		printf("%s\n", printword);
		perror(printword.c_str());
		WSAGetLastError();
		return -1;
	}
	return 0;
}
void handle_connection(SOCKET client_socket, string filename) {

	//接受client数据
	char recvdata[DATA_BUFF_SIZE] = "";
	int condition = recv(client_socket, recvdata, DATA_BUFF_SIZE, 0);
	error(condition, SOCKET_ERROR, "接受数据失败！\n");
	printf("接受client%d数据内容:\n%s\n共接收到%d字节数据\n", client_socket, recvdata, strlen(recvdata));

	int i = 0, j = 0;
	char name[NAME_BUFF_SIZE] = " ";
	while (recvdata[i] != '/' && i < NAME_BUFF_SIZE) i++;

	while (recvdata[i + 1] != ' ' && i < NAME_BUFF_SIZE)
	{
		name[j] = recvdata[i + 1];
		i++, j++;
		if (j >= NAME_BUFF_SIZE - 1) {
			printf("File Name Is Too Long !\n");
			break;
		}
	}
	name[j] = '\0';
	printf("请求文件名：%s\n", name);
	char filename2[NAME_BUFF_SIZE * 2] = "";
	strcpy(filename2, filename.c_str());
	strcat(filename2, name);
	printf("文件路径:%s\n", filename2);
	own_send(client_socket, filename2);
}

void own_send(SOCKET s, string filename)
{
	string head = "HTTP/1.1 200 OK\r\n";
	string not_find = "HTTP/1.1 404 NOT FOUND\r\n";
	string extension = get_extension_name(filename);
	if (extension.empty()) {
		printf("InVaild Extension Name\n");
		send(s, not_find.c_str(), not_find.size(), 0);
		return;
	}
	string content_type = translate_extension(extension);

	//响应报文头
	int len = head.length(), file_len = -1;
	FILE* pfile = fopen(filename.c_str(), "rb");
	if (pfile == NULL) {
		printf("未能找到文件%s", filename.c_str());
		send(s, not_find.c_str(), not_find.length(), 0);
		return;
	}
	else if (send(s, head.c_str(), len, 0) == -1)
	{
		printf("Sending error!\n");
		return;
	}
	char temp_buff[NAME_BUFF_SIZE] = "Content-Length: ";
	//获取文件大小
	fseek(pfile, 0L, SEEK_END);
	file_len = ftell(pfile);
	//strcpy(temp_buff, "Content-Length: ");
	strcat(temp_buff, to_string(file_len).c_str());
	strcat(temp_buff, "\r\n");
	len = strlen(temp_buff);

	if (send(s, temp_buff, len, 0) == -1) {
		printf("Sending error!\n");
		return;
	}
	strcpy(temp_buff, "Content-type: ");
	if (content_type.size() > 0)
	{
		strcat(temp_buff, content_type.c_str());
		strcat(temp_buff, "\r\n");
		len = strlen(temp_buff);

		if (send(s, temp_buff, len, 0) == -1) {
			printf("Sending error!\n");
			return;
		}
	}
	send(s, "\r\n", 2, 0);

	char* cpy = (char*)malloc(file_len + 1);
	fseek(pfile, 0L, SEEK_SET);
	fread(cpy, file_len, 1, pfile);
	send(s, cpy, file_len, 0);
}
string get_extension_name(string filename)
{
	int index = filename.find_last_of('.');
	if (index != -1) {
		return filename.substr(index + 1);
	}
	else return "";
}
string translate_extension(string extension) {
	if (extension == "html")return "text/html";
	else if (extension == "gif")return "image/gif";
	else if (extension == "jpg") return "image/jpg";
	else if (extension == "png") return "image/png";
	return "text/plain";
}