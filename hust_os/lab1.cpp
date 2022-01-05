#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<iostream>
#include<signal.h>
 
#define MAX_SEND 5
 
using namespace std;

int pipefd[2];
pid_t pid1,pid2;
int whole_count = 1,count1 = 0,count2 = 0;

void INT_handler(int sig);
void child_handler(int sig);
 
int main(void){
	pipe(pipefd);
	
	cout<<"Max Send Times:" << MAX_SEND <<endl;
 
	signal(SIGINT,INT_handler);
	if((pid1=fork())==0){
		close(pipefd[1]);
		signal(SIGUSR1,child_handler);
		char msg1[50];
		int len = 0;
		while(1){
			len = read(pipefd[0],msg1,sizeof(msg1));
			cout<<"child1 receive message:"<<msg1<<endl;
			if(len == 0){
				close(pipefd[0]);
				break;
			}
			count1++;
		}
	}else if((pid2=fork())==0){  //child2
		close(pipefd[1]);
		signal(SIGUSR2,child_handler);
		char msg2[50];
		int len = 0;
		while(1){
			len = read(pipefd[0],msg2,sizeof(msg2));
			cout<<"child2 receive message:"<<msg2<<endl;
			if(len == 0){
				close(pipefd[0]);
				break;
			}
			count2++;
		}
	}else{   
		//father
		close(pipefd[0]);
		while(whole_count <= MAX_SEND){
			string str = "I send you " + to_string(whole_count) + " times";
			write(pipefd[1],str.data(),strlen(str.data()));
			whole_count++;
			sleep(1);
		}
		close(pipefd[1]);
 
		kill(pid1,SIGUSR1);
		kill(pid2,SIGUSR2);
		waitpid(pid1,NULL,0);
		waitpid(pid2,NULL,0);
 
		cout << "parent send " << (whole_count - 1) << " times" << endl;
		cout<<"the send times is at the upper limit,parent process exit"<<endl;
	}
}


void INT_handler(int sig){
	close(pipefd[0]);
	close(pipefd[1]);
	if(pid1 == 0){
		cout<<"\nchild1 received "<<count1<<" times!\n";
		cout<<"Child Process 1 is Killed by INT!\n";
	
	}
	else if(pid2 == 0){
		cout<<"\nchild2 received "<<count2<<" times!\n";
		cout<<"Child Process 2 is Killed by INT!\n";
	}
	else{
		cout << "\nparent send" << whole_count - 1<< "times\n";
		cout << "Parent Process is Killed by INT!\n";
	}
	exit(0);
}
void child_handler(int sig){
	close(pipefd[0]);
	if(sig == SIGUSR1){
		cout<<"\nchild1 received "<<count1<<" times!\n";
		cout<<"Child Process 1 is Killed by Parent!\n";
	}
	else if(sig == SIGUSR2){
		cout<<"\nchild2 received "<<count2<<" times!\n";
		cout<<"Child Process 2 is Killed by Parent!\n";
	}
	exit(0);
}
