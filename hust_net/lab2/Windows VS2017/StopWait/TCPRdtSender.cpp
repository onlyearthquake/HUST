
#include "stdafx.h"
#include "Global.h"
#include "PersonalHelper.h"
#include"TCPRdtSender.h"
TCPRdtSender::TCPRdtSender() :
	WndSize(4), SeqnumSize(1 << 8),
	baseNum(0), nextWaitNum(0), surplusNum(0)
{
	packets = vector<Packet>(WndSize);
}

TCPRdtSender::TCPRdtSender(int wndSize, int bitSize) :
	WndSize(wndSize < 0 || bitSize < 0 ? 4 : wndSize),
	SeqnumSize(wndSize < 0 || bitSize < 0 ? 8 : 1 << bitSize),
	baseNum(0), nextWaitNum(0),surplusNum(0)
{
	packets = vector<Packet>(WndSize);
}

TCPRdtSender::~TCPRdtSender() {}

bool TCPRdtSender::getWaitingState() {
	return  (baseNum + WndSize) % SeqnumSize == nextWaitNum;
}

void TCPRdtSender::printWindow() {
	cout << "滑动窗口变为  from" << this->baseNum << "to" << this->nextWaitNum << endl;
}
bool TCPRdtSender::send(const Message& message) {
	if (this->getWaitingState()) {
		cout << "队列已满" << endl;
		return false;
	}
	int curNum = nextWaitNum;
	Packet tmp_pkt = gen_data_pkt(curNum, message.data);
	this->packets[curNum % WndSize] = tmp_pkt;
	pUtils->printPacket("发送方发送报文", tmp_pkt);
	pns->sendToNetworkLayer(RECEIVER, tmp_pkt);
	//如果是滑窗的首个 启动定时器
	if (this->baseNum == curNum) {
		this->printWindow();
		pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
	}
	nextWaitNum = (nextWaitNum + 1) % SeqnumSize;

	return true;
}

void TCPRdtSender::receive(const Packet& ackPkt) {

	int checkSum = pUtils->calculateCheckSum(ackPkt);
	if (checkSum == ackPkt.checksum) {
		if (this->baseNum != ackPkt.acknum + 1) {
			//及时Reset  不然会有一个小概率BUG
			this->surplusNum = 0;
			this->baseNum = (ackPkt.acknum + 1) % SeqnumSize;
			pUtils->printPacket("发送方正确收到ACK", ackPkt);
			pns->stopTimer(SENDER, 0);
			if (baseNum != nextWaitNum) {
				pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
			}
			this->printWindow();
		}
		else {
			this->surplusNum++;
			if (this->surplusNum >= 3) {
				cout << "快速重发!" << endl;
				pns->stopTimer(SENDER, 0);
				//重发上一个
				pns->sendToNetworkLayer(RECEIVER, packets[this->baseNum % WndSize]);
				pUtils->printPacket("重发报文", packets[this->baseNum % WndSize]);
				pns->startTimer(SENDER, Configuration::TIME_OUT, 0);

				this->surplusNum = 0;
			}
		}
	}
	else {
		pUtils->printPacket("发送方收到损坏ACK", ackPkt);
	}
}

void TCPRdtSender::timeoutHandler(int seqNum) {
	cout << "超时重发!" << this->baseNum << endl;
	pns->stopTimer(SENDER, 0);
	//重发上最早未确认
	pns->sendToNetworkLayer(RECEIVER, packets[this->baseNum % WndSize]);
	pUtils->printPacket("重发报文", packets[this->baseNum % WndSize]);
	
	pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
}