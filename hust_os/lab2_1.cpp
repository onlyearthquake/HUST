#include<sys/types.h>
#include<iostream>
#include<pthread.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<stdio.h>

using namespace std;
union semun {
    int val; /* value for SETVAL */
    struct semid_ds *buf; /* buffer for IPC_STAT, IPC_SET */
    unsigned short *array; /* array for GETALL, SETALL */
    struct seminfo *__buf; /* buffer for IPC_INFO */
};

int whole_count=100,semid;
union semun arg;
struct sembuf sem;


void P(int semid,int index);
void V(int semid,int index);

void *subpf1(void*);
void *subpf2(void*);
void *subpf3(void*);

int main(){
	semid = semget(IPC_PRIVATE,1,IPC_CREAT|0666);

	arg.val = 1;
	if(semctl(semid,0,SETVAL,arg)<0){
		cout<<"semctl error!";
		return 1;
	}
	pthread_t subp1,subp2,subp3;
	//create threads
	pthread_create(&subp1,NULL,subpf1,NULL);
	pthread_create(&subp2,NULL,subpf2,NULL);
	pthread_create(&subp3,NULL,subpf3,NULL);

	//wait for end
	pthread_join(subp1,NULL);
	pthread_join(subp2,NULL);
	pthread_join(subp3,NULL);
	
	//delete
	semctl(semid,0,IPC_RMID,arg);
	return 0;

}

void *subpf1(void*){
	int count1 = 0;
	while(whole_count>0){
		P(semid,0);
		if(!whole_count){
			V(semid,0);
			break;
		}
		whole_count--;
		cout<<"subp1 sell,remain: "<<whole_count<<endl;
		count1++;
		V(semid,0);
	}
	cout<<"thread 1 sold totally: "<<count1<<endl;
	return nullptr;
}
void *subpf2(void*){
	int count2=0;
	while(whole_count>0){
		P(semid,0);
		if(!whole_count){
			V(semid,0);
			break;
		}
		whole_count--;
		cout<<"subp2 sell,remain: "<<whole_count<<endl;
		count2++;
		V(semid,0);
	}
	cout<<"thread 2 sold totally: "<<count2<<endl;
	return nullptr;
}

void *subpf3(void*){
	int count3 = 0;
	while(whole_count>0){
		P(semid,0);
		if(!whole_count){
			V(semid,0);
			break;
		}
		whole_count--;
		cout<<"subp3 sell,remain: "<<whole_count<<endl;
		count3++;
		V(semid,0);
	}
	cout<<"thread 3 sold totally: "<<count3<<endl;
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