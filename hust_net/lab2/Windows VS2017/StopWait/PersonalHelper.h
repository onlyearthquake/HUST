#pragma once
#include "stdafx.h"
#include "Global.h"
#include "DataStructure.h"

Packet gen_pkt(int seqNum, int ackNum, const char data[Configuration::PAYLOAD_SIZE]);
Packet gen_data_pkt(int seqNum, const char data[Configuration::PAYLOAD_SIZE]);
Packet gen_ack_pkt(int seqNum);
Packet gen_ordered_ack_pkt(int seqNum);