#pragma once
#include "RdtSender.h"
#include "DataStructure.h"

#include <vector>
#include <iostream>
using namespace std;
class GBNRdtSender :public RdtSender
{
public:
	bool getWaitingState();
	bool send(const Message& message);
	void receive(const Packet& packet);
	void timeoutHandler(int seqNum);
	void printWindow();

	GBNRdtSender();
	GBNRdtSender(int wndSize, int bitSize);
	virtual ~GBNRdtSender();


private:
	int baseNum;					//�����δȷ�ϵ����
	int nextWaitNum;				//��һ��������������
	const int WndSize;				//���ڴ�С
	const int SeqnumSize;				//������Ŷ�����λ��
	vector<Packet> packets;	//�ѷ�����δȷ�ϵ�Packets
};