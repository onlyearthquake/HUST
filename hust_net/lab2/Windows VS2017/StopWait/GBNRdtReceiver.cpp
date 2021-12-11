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
		pUtils->printPacket("���շ����յ���ȷ����", pkt);

		lastAckPkt = gen_ack_pkt(pkt.seqnum);
		Message msg;
		memcpy(msg.data, pkt.payload, sizeof(pkt.payload));
		pns->delivertoAppLayer(RECEIVER, msg);
		pUtils->printPacket("���շ�����ACK����", lastAckPkt);

		expectedSeqnum = (expectedSeqnum + 1) % SeqnumSize;
	}
	else 
	{
		if (checksum != pkt.checksum) {
			pUtils->printPacket("���շ����յ��𻵱���", pkt);
		}
		else if (pkt.seqnum != expectedSeqnum) {
			//������ʧ������
			pUtils->printPacket("���շ����յ�ʧ����", pkt);
		}
		
		pUtils->printPacket("���շ��ط���һ��ACK", lastAckPkt);
	}
	pns->sendToNetworkLayer(SENDER, lastAckPkt);
}
