#include "stdafx.h"
#include "Global.h"
#include "PersonalHelper.h"
#include "GBNRdtReceiver.h"
GBNRdtReceiver::~GBNRdtReceiver() {}

GBNRdtReceiver::GBNRdtReceiver(int bitSize) :
	SeqnumSize(bitSize > 0 ? (1 << bitSize) : (1 << 8)),
	expectedSeqnum(0),
	lastAckPkt(gen_ack_pkt(-1)) {}

void GBNRdtReceiver::receive(const Packet& pkt) {
	int checksum = pUtils->calculateCheckSum(pkt);
	if (checksum == pkt.checksum && expectedSeqnum == pkt.seqnum) {
		pUtils->printPacket("接收方接收到正确报文", pkt);

		lastAckPkt = gen_ack_pkt(pkt.seqnum);
		Message msg;
		memcpy(msg.data, pkt.payload, sizeof(pkt.payload));
		pns->delivertoAppLayer(RECEIVER, msg);
		pUtils->printPacket("接收方发送ACK报文", lastAckPkt);

		expectedSeqnum = (expectedSeqnum + 1) % SeqnumSize;
	}
	else 
	{
		if (checksum != pkt.checksum) {
			pUtils->printPacket("接收方接收到损坏报文", pkt);
		}
		else if (pkt.seqnum != expectedSeqnum) {
			//不接受失序数据
			pUtils->printPacket("接收方接收到失序报文", pkt);
		}
		
		pUtils->printPacket("接收方重发上一个ACK", lastAckPkt);
	}
	pns->sendToNetworkLayer(SENDER, lastAckPkt);
}
