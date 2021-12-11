#include "stdafx.h"
#include "PersonalHelper.h"
#include "SRRdtReceiver.h"
SRRdtReceiver::~SRRdtReceiver(){}
SRRdtReceiver::SRRdtReceiver(int wndSize, int bitSize) :
	WndSize(wndSize),SeqnumSize(1 << bitSize),
	baseNum(0)
{
	status = vector<bool>(WndSize);
	cachePkts = vector<Packet>(WndSize);
	acks = vector<Packet>(WndSize);
}

void SRRdtReceiver::printWindow() {
	cout << "���շ��������ڱ�Ϊ  from" << this->baseNum << "to" << this->baseNum + WndSize << endl;
}
bool SRRdtReceiver::IsInWnd(int seqNum) {
	if (baseNum + WndSize >= SeqnumSize) {
		return seqNum >= baseNum || seqNum <= (baseNum + WndSize) % SeqnumSize;
	}
	else {
		return seqNum >= baseNum && seqNum <= (baseNum + WndSize) % SeqnumSize;
	}
}
void SRRdtReceiver::receive(const Packet& pkt) {
	int checksum = pUtils->calculateCheckSum(pkt);
	if (checksum == pkt.checksum && IsInWnd(pkt.seqnum)) {
		cachePkts[pkt.seqnum % WndSize] = pkt;
		status[pkt.seqnum % WndSize] = true;
		//�ظ�
		pUtils->printPacket("���շ����յ���ȷ����", pkt);
		acks[pkt.seqnum % WndSize] = gen_ordered_ack_pkt(pkt.seqnum);
		pns->sendToNetworkLayer(SENDER, acks[pkt.seqnum % WndSize]);
		//���������ߵ� �ƶ�����
		if (pkt.seqnum % SeqnumSize == baseNum) {
			while (status[baseNum % WndSize]) {
				Message msg;
				memcpy(msg.data, cachePkts[baseNum % WndSize].payload, sizeof(pkt.payload));
				pns->delivertoAppLayer(RECEIVER, msg);
				status[baseNum % WndSize] = false;
				baseNum = (baseNum + 1) % SeqnumSize;
				printWindow();
			}
		}
		pUtils->printPacket("���շ�����ACK����", acks[pkt.seqnum % WndSize]);

	}
	else
	{
		//����������
		if (checksum != pkt.checksum) {
			pUtils->printPacket("���շ����յ��𻵱���", pkt);
		}
		else if (!IsInWnd(pkt.seqnum)) {
			pns->sendToNetworkLayer(SENDER, acks[pkt.seqnum % WndSize]);
			pUtils->printPacket("���շ��ط���һ��ACK", pkt);
		}
	}
}