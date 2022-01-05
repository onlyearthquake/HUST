#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include <signal.h>
#include <stddef.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include "setting.h"

using namespace std;
void V(int semid, int index);
void P(int semid, int index);
union semun
{
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};
int main(int argc,char *argv[])
{
	if(argc < 2){
		cout << "read error" << endl;
		exit(0);
	}
	//Init
	share_buff *buf[SHARE_SIZE];
	int shmids[SHARE_SIZE];
	
	for(int i = 0;i < SHARE_SIZE;i++){
		shmids[i] = shmget(BEGIN_KEY + i * DELTA,sizeof(share_buff), 0666);
		buf[i] = (share_buff *)shmat(shmids[i], 0, 0);
	}
	
	int semid;
	semid = semget((key_t)SIGNAL_KEY, 2, IPC_CREAT | 0666);

	FILE *fp = fopen(argv[1], "rb");
	if(fp == nullptr){
		cout << "open file error" << endl;
		exit(0);
	}
	int index = 0, whole_count = 0;
	while (1)
	{
		P(semid, 0);
		
		//Get The Len
        int len =  fread(buf[index] -> data, 1,100, fp);
        buf[index] -> len = len;
        
       // cout << len <<endl;
        if(!len){
			V(semid, 1);
			fclose(fp);
			break;
        }
		whole_count++;
		cout << "readbuf process use "<< index <<" shm"<<endl;
		index = (index + 1) % SHARE_SIZE;
		
		V(semid, 1);
	}
	cout << "readbuf finish, run " <<whole_count<<" times"<< endl;
	exit(0);
}

void P(int semid, int index)
{
	struct sembuf sem;
	sem.sem_num = index;
	sem.sem_op = -1;
	sem.sem_flg = 0;
	semop(semid, &sem, 1);
	return;
}

void V(int semid, int index)
{
	struct sembuf sem;
	sem.sem_num = index;
	sem.sem_op = 1;
	sem.sem_flg = 0;
	semop(semid, &sem, 1);
	return;
}