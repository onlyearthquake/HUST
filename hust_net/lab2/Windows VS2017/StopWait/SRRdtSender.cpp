
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
	cout << "�������ڱ�Ϊ  from" << this->baseNum << "to" << this->nextWaitNum << endl;
}

bool SRRdtSender::IsInWnd(int seqNum) {
	//���ų�Reveiver������ACK�Ŀ���
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
		cout << "��������" << endl;
		return false;
	}
	int curNum = nextWaitNum;
	Packet tmp_pkt = gen_data_pkt(curNum, message.data);
	this->packets[curNum % WndSize] = tmp_pkt;
	this->status[curNum % WndSize] = false;
	pUtils->printPacket("���ͷ����ͱ���", tmp_pkt);
	pns->sendToNetworkLayer(RECEIVER, tmp_pkt);
	//������ʱ��
	pns->startTimer(SENDER, Configuration::TIME_OUT, curNum % SeqnumSize);
	this->printWindow();
	nextWaitNum = (nextWaitNum + 1) % SeqnumSize;

	return true;
}

void SRRdtSender::receive(const Packet& ackPkt) {

	int checkSum = pUtils->calculateCheckSum(ackPkt);
	if (checkSum == ackPkt.checksum) {
		if (!IsInWnd(ackPkt.seqnum)) {
			pUtils->printPacket("���ͷ��յ�Խ��ACK", ackPkt);
			return;
		}
		status[ackPkt.acknum % WndSize] = true;
		if (ackPkt.acknum == baseNum) {
			//�������ƶ�
			while (status[baseNum % WndSize]) {
				status[baseNum % WndSize] = false;
				baseNum = (baseNum + 1) % SeqnumSize;
			}
			this->printWindow();
		}
		pUtils->printPacket("���ͷ���ȷ�յ�ACK", ackPkt);
		pns->stopTimer(SENDER, ackPkt.acknum % SeqnumSize);
	}
	else {
		pUtils->printPacket("���ͷ��յ���ACK", ackPkt);
	}
}
void SRRdtSender::timeoutHandler(int seqNum) {
	pns->stopTimer(SENDER, seqNum % SeqnumSize);
	cout << "��ʱ�ط�! ";
	cout << seqNum % SeqnumSize << endl;
	//�ط�ָ������
	pns->sendToNetworkLayer(RECEIVER, packets[seqNum % WndSize]);
	pUtils->printPacket("�ط�����", packets[seqNum % WndSize]);
	
	pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum % SeqnumSize);
}

