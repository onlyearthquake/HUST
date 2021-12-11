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
	int baseNum;					//�����δȷ�ϵ����
	int nextWaitNum;				//��һ��������������
	const int WndSize;				//���ڴ�С
	const int SeqnumSize;				//������Ŷ�����λ��
	vector<Packet> packets;	//�ѷ�����δȷ�ϵ�Packets
};