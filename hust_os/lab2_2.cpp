#include<sys/types.h>
#include<iostream>
#include<pthread.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<stdio.h>

using namespace std;
#define MAX 100

union semun {
    int val; /* value for SETVAL */
    struct semid_ds *buf; /* buffer for IPC_STAT, IPC_SET */
    unsigned short *array; /* array for GETALL, SETALL */
    struct seminfo *__buf; /* buffer for IPC_INFO */
};
union semun arg;
struct sembuf sem;

int sum = 0,semid,whole_count = 1; //共享缓冲区

void P(int semid,int index);
void V(int semid,int index);

void *calculate(void*);
void *print_even(void*);
void *print_odd(void*);

int main(){
	pthread_t calcu,p1,p2;
	semid = semget(IPC_PRIVATE,3,IPC_CREAT|0666);
	//0: init=1 calculate
	//1: init=0 evenprint
	//2: init=0 odd print

	arg.val = 1;
	semctl(semid,0,SETVAL,arg);
	
	arg.val=0;
	semctl(semid,1,SETVAL,arg);
	semctl(semid,2,SETVAL,arg);
	
	//create threads
	pthread_create(&calcu,NULL,calculate,NULL);
	pthread_create(&p1,NULL,print_even,NULL);
	pthread_create(&p2,NULL,print_odd,NULL);

	pthread_join(calcu,NULL);
	pthread_join(p1,NULL);
	pthread_join(p2,NULL);

	semctl(semid,3,IPC_RMID,arg);
	return 0;
}
void *calculate(void*){
	while(whole_count <= MAX){
		P(semid,0);
		sum = sum + whole_count;
		whole_count++;
		if(sum % 2 == 0) V(semid,1);
		else V(semid,2);
	}
	cout <<"calculate thread create nums count: "<< whole_count - 1<<endl;
	return nullptr;	
}
void *print_even(void*){
	int count1 = 0;
	while(whole_count <= MAX){
		P(semid,1);
		cout <<"thread1 print even num: "<< sum <<endl;
		count1++;
		V(semid,0);
	}
	cout <<"thread 1 print nums count: "<< count1 <<endl;
	return nullptr;
}
void *print_odd(void*){
	int count2 = 0;
	while(whole_count <= MAX){
		P(semid,2);
		cout <<"thread2 print odd num: "<< sum <<endl;
		count2++;
		V(semid,0);
	}
	cout <<"thread 2 print nums count: "<< count2 <<endl;
	return nullptr;
}
void P(int semid,int index)
{	  
	struct sembuf sem;	
    sem.sem_num = index;
    sem.sem_op = -1;	
    sem.sem_flg = 0;	//操作标记：0或IPC_NOWAIT等	
    semop(semid,&sem,1);	//1:表示执行命令的个数
    return;
}
void V(int semid,int index)
{	 
	struct sembuf sem;	
    sem.sem_num = index;
    sem.sem_op =  1;
    sem.sem_flg = 0;	
    semop(semid,&sem,1);	
    return;
}