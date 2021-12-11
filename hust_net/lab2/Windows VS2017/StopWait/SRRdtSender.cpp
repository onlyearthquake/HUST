
#include "stdafx.h"
#include "SRRdtSender.h"
SRRdtSender::~SRRdtSender(){}

SRRdtSender::SRRdtSender():
	WndSize(4),SeqnumSize(1 << 8),
	baseNum(0),nextWaitNum(0)
{
	packets = vector<Packet>(WndSize);
	status = vector<bool>(WndSize);
}
SRRdtSender::SRRdtSender(int wndSize, int bitSize) :
	WndSize(wndSize < 0 || bitSize < 0 ? 4 : wndSize),
	SeqnumSize(wndSize < 0 || bitSize < 0 ? 8 : 1 << bitSize),
	baseNum(0), nextWaitNum(0)
{
	packets = vector<Packet>(WndSize);
	status = vector<bool>(WndSize);
}
bool SRRdtSender:: getWaitingState() {
	return (baseNum + WndSize) % SeqnumSize == nextWaitNum;
}


void SRRdtSender::printWindow() {
	cout << "滑动窗口变为  from" << this->baseNum << "to" << this->nextWaitNum << endl;
}

bool SRRdtSender::IsInWnd(int seqNum) {
	//不排除Reveiver发超界ACK的可能
	if (baseNum + WndSize >= SeqnumSize) {
		return seqNum >= baseNum || seqNum <= (baseNum + WndSize) % SeqnumSize;
	}
	else {
		return seqNum >= baseNum && seqNum <= (baseNum + WndSize) % SeqnumSize;
	}		
}

bool SRRdtSender::send(const Message& message)
{
	if (this->getWaitingState()) {
		cout << "队列已满" << endl;
		return false;
	}
	int curNum = nextWaitNum;
	Packet tmp_pkt = gen_data_pkt(curNum, message.data);
	this->packets[curNum % WndSize] = tmp_pkt;
	this->status[curNum % WndSize] = false;
	pUtils->printPacket("发送方发送报文", tmp_pkt);
	pns->sendToNetworkLayer(RECEIVER, tmp_pkt);
	//启动定时器
	pns->startTimer(SENDER, Configuration::TIME_OUT, curNum % SeqnumSize);
	this->printWindow();
	nextWaitNum = (nextWaitNum + 1) % SeqnumSize;

	return true;
}

void SRRdtSender::receive(const Packet& ackPkt) {

	int checkSum = pUtils->calculateCheckSum(ackPkt);
	if (checkSum == ackPkt.checksum) {
		if (!IsInWnd(ackPkt.seqnum)) {
			pUtils->printPacket("发送方收到越界ACK", ackPkt);
			return;
		}
		status[ackPkt.acknum % WndSize] = true;
		if (ackPkt.acknum == baseNum) {
			//尽可能移动
			while (status[baseNum % WndSize]) {
				status[baseNum % WndSize] = false;
				baseNum = (baseNum + 1) % SeqnumSize;
			}
			this->printWindow();
		}
		pUtils->printPacket("发送方正确收到ACK", ackPkt);
		pns->stopTimer(SENDER, ackPkt.acknum % SeqnumSize);
	}
	else {
		pUtils->printPacket("发送方收到损坏ACK", ackPkt);
	}
}
void SRRdtSender::timeoutHandler(int seqNum) {
	pns->stopTimer(SENDER, seqNum % SeqnumSize);
	cout << "超时重发! ";
	cout << seqNum % SeqnumSize << endl;
	//重发指定分组
	pns->sendToNetworkLayer(RECEIVER, packets[seqNum % WndSize]);
	pUtils->printPacket("重发报文", packets[seqNum % WndSize]);
	
	pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum % SeqnumSize);
}

