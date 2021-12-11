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
	cout << "接收方滑动窗口变为  from" << this->baseNum << "to" << this->baseNum + WndSize << endl;
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
		//回复
		pUtils->printPacket("接收方接收到正确报文", pkt);
		acks[pkt.seqnum % WndSize] = gen_ordered_ack_pkt(pkt.seqnum);
		pns->sendToNetworkLayer(SENDER, acks[pkt.seqnum % WndSize]);
		//如果是最左边的 移动窗口
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
		pUtils->printPacket("接收方发送ACK报文", acks[pkt.seqnum % WndSize]);

	}
	else
	{
		//处理错误情况
		if (checksum != pkt.checksum) {
			pUtils->printPacket("接收方接收到损坏报文", pkt);
		}
		else if (!IsInWnd(pkt.seqnum)) {
			pns->sendToNetworkLayer(SENDER, acks[pkt.seqnum % WndSize]);
			pUtils->printPacket("接收方重发上一个ACK", pkt);
		}
	}
}