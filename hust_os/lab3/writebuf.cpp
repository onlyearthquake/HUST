#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include <signal.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string>
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
int main(int argc,char* argv[])
{
	if(argc < 2){
		cout << "wirte error ,args too short" << endl;
		exit(0);
	}
	//Init
	share_buff* bufs[SHARE_SIZE];
	int shmids[SHARE_SIZE];
	
	for(int i = 0;i < SHARE_SIZE;i++){
		shmids[i] = shmget(BEGIN_KEY + i * DELTA,sizeof(share_buff), 0666);
		bufs[i] = (share_buff*)shmat(shmids[i], 0, 0);
	}
	int semid = semget((key_t)SIGNAL_KEY, 2, IPC_CREAT | 0666); 
	
	//Open The File
	FILE *fp = fopen(argv[1], "wb");
	if (fp  == NULL)
	{
		cout << "open file error" << endl;
		exit(0);
	}
	int index = 0,whole_count = 0;
	while (1)
	{
		int len = 0;
		P(semid, 1);
		
		len = bufs[index] -> len;
		if (len == 0)
		{
			fclose(fp);
			break;
		}
		fwrite(bufs[index] -> data,1,len, fp);
		whole_count++;
		cout << "write process use " << index <<" shm"<<endl;
		index = (index + 1) % SHARE_SIZE;
		
		
		V(semid, 0);
	}
	cout << "writebuf finish, run " <<whole_count<<" times"<< endl;
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