#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<dirent.h>
#include<sys/stat.h>

struct HeadDir{
	struct HeadDir* next;
	char name[260];
};

#define COPYFILE strcpy(cmd,"cp ");\
			strcat(cmd,(*path).d_name);\
			strcat(cmd," ");\
			strcat(cmd,destdir);\
			strcat(cmd,(*path).d_name);\
			printf("%s\n",cmd);\
			if(system(cmd)==-1){\
				printf("cp:error cope %s\n",(*path).d_name);\
			}

/*------------------find .o .a .so and executable file-------------------------------*/
int BitPreTreat(char* srcdir,char* destdir)
{
	DIR* dp;
	char cmd[10000],*temp;
	struct dirent *path;
	struct stat buf;
	if((dp=opendir(srcdir))==NULL){
		printf("CPreTreat:error open %s\n",srcdir);
		return -1;
	}
	chdir(srcdir);
	while(path=readdir(dp)){
		lstat((*path).d_name,&buf);
		/*If dir*/
		if(S_ISDIR((buf).st_mode)==1){
			/*Ignore . and ..*/
			if(strcmp((*path).d_name,"..")==0||strcmp((*path).d_name,".")==0)
				continue;
			/*Search for next dir*/
			else
				BitPreTreat((*path).d_name,destdir);
		}
		/*Deal with the .a .o .so file*/
		else if((temp=strstr((*path).d_name,".so"))!=NULL){
			if(temp[3]!='\0')
				continue;
			COPYFILE
		}
		else if((S_IXUSR&buf.st_mode)==S_IXUSR ||(S_IXGRP&buf.st_mode)==S_IXGRP \
			|| (S_IXOTH&buf.st_mode)==S_IXOTH ){
			if(strstr((*path).d_name,".")!=NULL)
				continue;
			COPYFILE
		}
	}
	chdir("..");
	closedir(dp);
	return 0;
}

/*------------------search all dir that include .h file--------------------*/
struct HeadDir* SearchHdir(char* srcdir,struct HeadDir *headDir)
{
	int sign=0;
	DIR *dp;
	char dir[260],*temp;
	struct dirent *path;
	struct stat buf;
	struct HeadDir *hDir=headDir;

	/*Go through all the file*/
	if((dp=opendir(srcdir))==NULL){
		printf("SearchHdir:error open %s\n",srcdir);
		return headDir;
	}
	chdir(srcdir);
	while(path=readdir(dp)){
		lstat((*path).d_name,&buf);
		/*If dir*/
		if(S_ISDIR((buf).st_mode)==1){
			/*Ignore . and ..*/
			if(strcmp((*path).d_name,"..")==0||strcmp((*path).d_name,".")==0)
				continue;
			/*Search for next dir*/
			else{
				strcpy(dir,srcdir);
				strcat(dir,(*path).d_name);
				strcat(dir,"/");
				headDir=SearchHdir(dir,headDir);
			}
		}
		/*Deal with the .c file*/
		else if((temp=strstr((*path).d_name,".h"))!=NULL){
			if(temp[2]=='\0')
				sign=1;
		}
	}

	/*Add Head Dir information*/
	if(sign==1||headDir!=hDir){
		hDir=NULL;
		hDir=(struct HeadDir*)malloc(sizeof(struct HeadDir));
		hDir->next=headDir;
		strcpy(hDir->name,srcdir);
		headDir=hDir;
	}
	chdir("..");
	closedir(dp);
	return headDir;
}

/*------------------Pretreat C file with gcc-------------------------------*/
int CPreTreat(char* srcdir,char* destdir,char* headdir)
{
	DIR* dp;
	char cmd[10000],*temp;
	struct dirent *path;
	struct stat buf;
	if((dp=opendir(srcdir))==NULL){
		printf("CPreTreat:error open %s\n",srcdir);
		return -1;
	}
	chdir(srcdir);
	while(path=readdir(dp)){
		lstat((*path).d_name,&buf);
		/*If dir*/
		if(S_ISDIR((buf).st_mode)==1){
			/*Ignore . and ..*/
			if(strcmp((*path).d_name,"..")==0||strcmp((*path).d_name,".")==0)
				continue;
			/*Search for next dir*/
			else
				CPreTreat((*path).d_name,destdir,headdir);
		}
		/*Deal with the .c file*/
		else if((temp=strstr((*path).d_name,".c"))!=NULL){
			if(temp[2]!='\0')
				continue;
			strcpy(cmd,"gcc -E ");
			strcat(cmd,headdir);
			strcat(cmd,(*path).d_name);
			strcat(cmd," -o ");
			strcat(cmd,destdir);
			strcat(cmd,(*path).d_name);
			printf("%s\n",cmd);
			if(system(cmd)==-1){
				printf("gcc:error -E %s",(*path).d_name);
			}
		}
	}
	chdir("..");
	closedir(dp);
	return 0;
}


int main(int argc,char* argv[])
{
	int i;
	char srcBit[260]="";
	char destBit[260]="";
	char srcC[260]="";
	char destC[260]="";
	char headdir[10000]="";
	struct HeadDir *headDir=NULL;
	i=1;
	while(argc!=i){
		if(i+3>argc){
			printf("error input parameters\n");
			return -1;
		}
		else if(strcmp(argv[i],"-b")==0){
			strcpy(srcBit,argv[i+1]);
			strcpy(destBit,argv[i+2]);
		}
		else if(strcmp(argv[i],"-c")==0){
			strcpy(srcC,argv[i+1]);
			strcpy(destC,argv[i+2]);
		}
		else{
			printf("error input parameters\n");
			return -1;
		}
		i=i+3;
	}
	if(argc==1){
		printf("no input parameters\n");
		return -1;
	}

	if(srcBit[0]!='\0'){
		BitPreTreat(srcBit,destBit);
	}

	if(srcC[0]!='\0'){
		headDir=SearchHdir(srcC,headDir);
		while(headDir!=NULL){
			strcat(headdir,"-I");
			strcat(headdir,headDir->name);
			strcat(headdir," ");
			headDir=headDir->next;
		}
		CPreTreat(srcC,destC,headdir);
	}

	return 0;
}