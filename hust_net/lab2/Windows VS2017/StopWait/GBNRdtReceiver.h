#pragma once
#include "DataStructure.h"
#include "RdtReceiver.h"
class GBNRdtReceiver :public RdtReceiver
{
public:
	GBNRdtReceiver(int bitSize);
	~GBNRdtReceiver();
public:
	void receive(const Packet& packet);

private:
	int expectedSeqnum;
	Packet lastAckPkt;
	const int SeqnumSize;

};