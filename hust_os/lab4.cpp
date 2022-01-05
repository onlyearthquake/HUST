#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <iostream>
#include <ctime>
#include <pwd.h>
using namespace std;
void printdir(char *dir, int depth);

int main(int arc,char* arg[]){
	if(arc < 2){
		cout << "args too short!"<<endl;
    	return 0;
	}
    printdir(arg[1],0);
    return 0;
}
void printdir(char *dir, int depth){
    DIR *dp = opendir(dir);
    struct dirent *entry;
    struct stat statbuf;
    if(dp == nullptr){     
        cout << "opendir error!" << endl;
        cout << dir;
        return;
    }
    if(chdir(dir) == -1){
    	cout << "chdir error!" << endl;
    	return;
    }   
    
    while((entry=readdir(dp))!=NULL){
    	lstat(entry->d_name,&statbuf);
        if(S_ISDIR(statbuf.st_mode)){
        	//skip if equal "." ".."
        	if(!strcmp(entry->d_name,".")||!strcmp(entry->d_name,".."))
            	continue;
        }   
        //depth uid gid
        cout << "Depth : "<< depth;
        cout << " Owner: "<< getpwuid(statbuf.st_uid) -> pw_name ;
        cout << " OwnerGroup: "<<getpwuid(statbuf.st_gid) -> pw_name <<endl;
        //Size && time && name
        cout << "FileSize : "<< statbuf.st_size;
        cout <<" Time: "<<ctime(&statbuf.st_atime);
        cout << " File Name: " << entry->d_name << endl;
        
        if(S_ISDIR(statbuf.st_mode)){
    		//cout << "enter dir: "<<entry->d_name<<endl;
    		printdir(entry->d_name,depth+4);
    		//cout << "return to directory: " << dir << endl;
        }
    }
    if(depth != 0 && chdir("..") == -1){
    	cout << "chdir error!\n";
    	return;
    }   

    closedir(dp); 
    return;
}