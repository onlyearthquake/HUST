#pragma once
#include "RdtSender.h"
#include "DataStructure.h"

#include <vector>
#include <iostream>
using namespace std;
class TCPRdtSender :public RdtSender
{
public:
	bool getWaitingState();
	bool send(const Message& message);
	void receive(const Packet& packet);
	void timeoutHandler(int seqNum);
	void printWindow();

	TCPRdtSender();
	TCPRdtSender(int wndSize, int bitSize);
	virtual ~TCPRdtSender();


private:
	int surplusNum;
	int baseNum;					//最早的未确认的序号
	int nextWaitNum;				//下一个待发分组的序号
	const int WndSize;				//窗口大小
	const int SeqnumSize;				//报文序号二进制位数
	vector<Packet> packets;	//已发送且未确认的Packets
};