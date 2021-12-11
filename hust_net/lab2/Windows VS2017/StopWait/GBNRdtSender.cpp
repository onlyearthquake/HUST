
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
	cout << "�������ڱ�Ϊ  from" << this -> baseNum << "to" << this->nextWaitNum<< endl;
}
bool GBNRdtSender::send(const Message& message) {
	if (this->getWaitingState()) {
		cout << "��������" << endl;
		return false;
	}
	//make_pkt
	int curNum = nextWaitNum;
	Packet tmp_pkt = gen_data_pkt(curNum, message.data);
	this->packets[curNum % WndSize] = tmp_pkt;
	pUtils->printPacket("���ͷ����ͱ���", tmp_pkt);
	pns->sendToNetworkLayer(RECEIVER, tmp_pkt);
	//������׸� ������ʱ��
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
		pUtils->printPacket("���ͷ���ȷ�յ�ACK", ackPkt);
		pns->stopTimer(SENDER,0);
		this->printWindow();
		//û��ȷ���� ������ʱ
		if (baseNum != nextWaitNum) {
			pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
		}
	}
	else {
		pUtils->printPacket("���ͷ��յ���ACK", ackPkt);
	}
}

void GBNRdtSender::timeoutHandler(int seqNum) {
	pns->stopTimer(SENDER, 0);
	cout << "��ʱ�ط�! " ;
	cout << "From" << this -> baseNum << " to " << this -> nextWaitNum << endl;
	//�ط�����δȷ�ϵķ���
	for (int i = this -> baseNum; i != this -> nextWaitNum; i = (i + 1) % this -> SeqnumSize) {
		pns->sendToNetworkLayer(RECEIVER, packets[i % WndSize]);
		pUtils->printPacket("�ط�����", packets[i % WndSize]);
	}
	pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
}