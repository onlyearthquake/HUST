#pragma once
#include "RdtSender.h"
#include "DataStructure.h"
#include "PersonalHelper.h"
#include <vector>
#include <iostream>
using namespace std;
class SRRdtSender :public RdtSender
{
public:
	bool getWaitingState();
	bool send(const Message& message);
	void receive(const Packet& packet);
	void timeoutHandler(int seqNum);
	void printWindow();

	SRRdtSender();
	SRRdtSender(int wsize, int serial);
	~SRRdtSender();
private:
	const int WndSize;
	const int SeqnumSize;
	int baseNum;
	int nextWaitNum;
	vector<bool> status;
	vector<Packet> packets;
	bool IsInWnd(int acknum);
};