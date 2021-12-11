#pragma once

#define NAME_BUFF_SIZE 50
#define DATA_BUFF_SIZE 2048
#include<iostream>
#include<fstream>
#include<Winsock2.h>
#include<string>
using namespace std;
int error(int backdata, int errordata, string printword);
void handle_connection(SOCKET client_id, string filename);
void own_send(SOCKET s, string filename);
string translate_extension(string extension);
string get_extension_name(string filename);