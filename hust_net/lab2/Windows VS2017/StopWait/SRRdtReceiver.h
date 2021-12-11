#pragma once
#include <vector>
#include "stdafx.h"
#include "Global.h"
#include "RdtReceiver.h"

class SRRdtReceiver :public RdtReceiver
{
private:
	const int WndSize;
	const int SeqnumSize;
	int baseNum;
	vector<bool> status;
	vector<Packet> cachePkts;
	vector<Packet> acks;
	bool IsInWnd(int seqnum);
public:
	SRRdtReceiver(int wndSize, int bitSize);
	~SRRdtReceiver();
	void receive(const Packet& packet);
	void printWindow();
};