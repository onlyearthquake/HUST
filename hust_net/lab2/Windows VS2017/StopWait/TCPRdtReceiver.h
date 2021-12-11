#pragma once
#include "DataStructure.h"
#include "RdtReceiver.h"
class TCPRdtReceiver :public RdtReceiver
{
public:
	TCPRdtReceiver(int bitSize);
	~TCPRdtReceiver();
public:
	void receive(const Packet& packet);

private:
	int expectedSeqnum;
	Packet lastAckPkt;
	const int SeqnumSize;

};