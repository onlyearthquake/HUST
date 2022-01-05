#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <signal.h>

#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include "setting.h"

using namespace std;

void V(int semid, int index);
void P(int semid, int index);
void GET();
void PUT();
int pid1, pid2;

int semid; 
int shmids[SHARE_SIZE];

union semun
{
	int val;

	struct semid_ds *buf;

	unsigned short *array;
};

int main(int argc,char *argv[])
{
	if(argc < 3){
		cout << "main error" << endl;
		exit(0);
	}
	for(int i = 0;i < SHARE_SIZE;i++){
		shmids[i] = shmget(BEGIN_KEY + i * DELTA, sizeof(share_buff), 0666 | IPC_CREAT);
	}
	//create singal
	semid = semget((key_t)SIGNAL_KEY,2, IPC_CREAT | 0666); 
	union semun arg1, arg2;
	
	arg1.val = SHARE_SIZE;
	semctl(semid, 0, SETVAL, arg1); //read buff num
	arg2.val = 0;
	semctl(semid, 1, SETVAL, arg2); //write buff num
	
	if ((pid1 = fork()) == 0)
	{
		char* args[3];
  		args[0] = "./readbuf"; 
  		args[1] = argv[1];
  		args[2] = NULL;  //NULL-end
		
		cout << "readbuf process created" << endl;
		execv("./readbuf", args);
	}
	else if ((pid2 = fork()) == 0)
	{ 
		char* args[3];  
  		args[0] = "./writebuf";
  		args[1] = argv[2];
  		args[2] = NULL;  //NULL-end
		
		cout << "writebuf process created\n" << endl;
		execv("./writebuf", args);
	}
	//Wait For End
	waitpid(pid1,NULL,0);
	waitpid(pid2,NULL,0);
	//Delete shm And semid
	for(int i = 0;i < SHARE_SIZE;i++){
		shmctl(shmids[i], IPC_RMID, 0);
	}
	semctl(semid, IPC_RMID, 0);
	cout << "Task End" << endl;
	return 0;
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
