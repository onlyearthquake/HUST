
#include "stdafx.h"
#include "Global.h"
#include "PersonalHelper.h"
#include"GBNRdtSender.h"
GBNRdtSender::GBNRdtSender() :
	WndSize(4), SeqnumSize(1 << 8),
	baseNum(0),nextWaitNum(0),
	packets(vector<Packet>(WndSize)){}

GBNRdtSender::GBNRdtSender(int wndSize, int bitSize):
	WndSize(wndSize < 0 || bitSize < 0 ? 4 : wndSize),
	SeqnumSize(wndSize < 0 || bitSize < 0 ? 8 : 1 << bitSize),
	baseNum(0), nextWaitNum(0)
{
	packets = vector<Packet>(WndSize);
} 

GBNRdtSender::~GBNRdtSender() {}

bool GBNRdtSender::getWaitingState() {
	return  (baseNum + WndSize) % SeqnumSize == nextWaitNum;
}

void GBNRdtSender::printWindow() {
	cout << "滑动窗口变为  from" << this -> baseNum << "to" << this->nextWaitNum<< endl;
}
bool GBNRdtSender::send(const Message& message) {
	if (this->getWaitingState()) {
		cout << "队列已满" << endl;
		return false;
	}
	//make_pkt
	int curNum = nextWaitNum;
	Packet tmp_pkt = gen_data_pkt(curNum, message.data);
	this->packets[curNum % WndSize] = tmp_pkt;
	pUtils->printPacket("发送方发送报文", tmp_pkt);
	pns->sendToNetworkLayer(RECEIVER, tmp_pkt);
	//如果是首个 启动定时器
	if (this->baseNum == curNum) {
		this->printWindow();
		pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
	}
	nextWaitNum = (nextWaitNum + 1) % SeqnumSize;

	return true;
}

void GBNRdtSender::receive(const Packet& ackPkt) {

	int checkSum = pUtils->calculateCheckSum(ackPkt);
	if (checkSum == ackPkt.checksum) {
		this->baseNum = (ackPkt.acknum + 1) % SeqnumSize;
		pUtils->printPacket("发送方正确收到ACK", ackPkt);
		pns->stopTimer(SENDER,0);
		this->printWindow();
		//没有确认完 继续计时
		if (baseNum != nextWaitNum) {
			pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
		}
	}
	else {
		pUtils->printPacket("发送方收到损坏ACK", ackPkt);
	}
}

void GBNRdtSender::timeoutHandler(int seqNum) {
	pns->stopTimer(SENDER, 0);
	cout << "超时重发! " ;
	cout << "From" << this -> baseNum << " to " << this -> nextWaitNum << endl;
	//重发所有未确认的分组
	for (int i = this -> baseNum; i != this -> nextWaitNum; i = (i + 1) % this -> SeqnumSize) {
		pns->sendToNetworkLayer(RECEIVER, packets[i % WndSize]);
		pUtils->printPacket("重发报文", packets[i % WndSize]);
	}
	pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
}