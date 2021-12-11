// StopWait.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "Global.h"
#include "RdtSender.h"
#include "RdtReceiver.h"
#include "StopWaitRdtSender.h"
#include "StopWaitRdtReceiver.h"
#include "GBNRdtReceiver.h"
#include "GBNRdtSender.h"
#include "SRRdtReceiver.h"
#include "SRRdtSender.h"
#include "TCPRdtReceiver.h"
#include "TCPRdtSender.h"

static enum ServiceType { Default = 0, GBN, SR, TCP};
ServiceType type = ServiceType::TCP;
#define USE_FILE_OUTPUT false
#define LOG_FILE_PATH "result.txt"
int main(int argc, char* argv[])
{
	RdtSender* ps = new TCPRdtSender(16, 8);
	RdtReceiver* pr = new TCPRdtReceiver(8);
	switch (type) {
	case ServiceType::GBN:
		ps = new GBNRdtSender();
		pr = new GBNRdtReceiver(8);
		break;
	case ServiceType::SR:
		ps = new SRRdtSender(4,8);
		pr = new SRRdtReceiver(4,8);
		break;
	case ServiceType::TCP:
		ps = new TCPRdtSender(4, 8);
		pr = new TCPRdtReceiver(8);
		break;
	default:
		ps = new StopWaitRdtSender();
		pr = new StopWaitRdtReceiver();
	}

	FILE* pfile = NULL;
	if(USE_FILE_OUTPUT)
		freopen_s(&pfile, LOG_FILE_PATH, "w", stdout);
//	pns->setRunMode(0);  //VERBOSģʽ
	pns->setRunMode(1);  //����ģʽ
	pns->init();
	pns->setRtdSender(ps);
	pns->setRtdReceiver(pr);
	pns->setInputFile(INPUT_FILE_PATH);
	pns->setOutputFile(OUTPUT_FILE_PATH);

	pns->start();

	delete ps;
	delete pr;
	delete pUtils;									//ָ��Ψһ�Ĺ�����ʵ����ֻ��main��������ǰdelete
	delete pns;										//ָ��Ψһ��ģ�����绷����ʵ����ֻ��main��������ǰdelete
	
	return 0;
}

