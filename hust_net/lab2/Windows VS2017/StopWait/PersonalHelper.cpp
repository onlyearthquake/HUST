
#include "stdafx.h"
#include "PersonalHelper.h"
Packet gen_pkt(int seqNum, int ackNum, const char data[Configuration::PAYLOAD_SIZE]) {
	Packet pkt;
	pkt.acknum = ackNum;
	pkt.seqnum = seqNum;
	memcpy(pkt.payload, data, Configuration::PAYLOAD_SIZE);
	pkt.checksum = pUtils->calculateCheckSum(pkt);
	return pkt;
}

Packet gen_data_pkt(int seqNum, const char data[Configuration::PAYLOAD_SIZE]) {
	return gen_pkt(seqNum, -1, data);
}
Packet gen_ordered_ack_pkt(int seqNum) {
	char data[Configuration::PAYLOAD_SIZE] = "ACK";
	return gen_pkt(seqNum, seqNum, data);
}
Packet gen_ack_pkt(int seqNum) {
	char data[Configuration::PAYLOAD_SIZE] = "ACK";
	return gen_pkt(-1, seqNum, data);
}