#include<stdio.h>
#include<stdlib.h> /*exit()*/
#include<unistd.h>
#include<string.h>
#include<fcntl.h>
#include<sys/wait.h>
	
#define BUFSIZE 1024
#define MAXW 100
#define YES 1
#define NO 0

void forkexec(int, char *[]);
int parsarg(char [], char *, int , char *[]);
void foundPiporRedirn(int , char *[]);
void pipefun(char *[], char *[]);
void rednfun(char *[], char *[]);
void errxt(char *);
void initclear(char *[]);
int PipRedirnCheck(char *);

int main(int argc, char **argv)
{
	int myargc, found;
	char array[BUFSIZE] = "";
	char *myargv[MAXW];
	char *ptr;
	char *dlm1 = "	    \n\t,;";
	char *dlm2 = "	    |>\n\t,;";

	initclear(myargv);/*imp*/
	found = NO;
	while(1){
		printf("sushith@balu$ ");
		if(fgets(array, sizeof(array), stdin) != 0)
			;
		else{
			printf("\nexit\n");
			break;
		}
		ptr = array;
		found = PipRedirnCheck(ptr);
		if(found == YES){
			myargc = parsarg(array, dlm1, strlen(array), myargv);
			foundPiporRedirn(myargc, myargv);
			found = NO;
		} else {
			myargc = parsarg(array, dlm2, strlen(array), myargv);
			forkexec(myargc, myargv);
		}
	}
	return 0;
}

void forkexec( int arc, char *argv[])
{
	int fd;

	fd = fork();
	if(fd == -1)
		errxt("fork");
	if(fd == 0){
		execvp(argv[0], argv);
		errxt("execvpn");
	} else {
		wait(NULL);
	}
}

void foundPiporRedirn(int arc, char *arg[])
{
        int  i, k, found, fp = 0;
	char *arg1[MAXW];
	char *arg2[MAXW];
	
	for(i = 0; i <= arc; i++){
		if((strcmp(arg[i],"|") == 0)|| strcmp(arg[i], ">") == 0){
			found = i;
		}
	}
	initclear(arg1);
	for(i = 0; i < found; i++){
		arg1[i] = arg[i];
	}
	k = found +1;
	initclear(arg2);
	for(i = k; i <= arc; i++){
		arg2[fp] = arg[i];
		fp++;
	}
	arg2[fp] = NULL;	/*important*/
	arg1[found] = NULL;

	for(i = 0; i < arc; i++){
		if(strcmp(arg[i], "|") == 0){
			pipefun(arg1, arg2);
		} else if(strcmp(arg[i], ">") == 0){
			rednfun(arg1, arg2);
		}
	}
}

void pipefun(char *argp1[], char *argp2[])
{
	int fd1, fd2, p[2], status;

	if(pipe(p) == -1) 
		errxt("pipe");
	fd1 = fork();
	if(fd1 == -1)
		errxt("fork");
	if(fd1 == 0) {
		close(1); /*close normal stdout*/
		dup(p[1]);
		close(p[0]);
		execvp(argp1[0], argp1);
		close(p[1]);
		exit(EXIT_SUCCESS);
	} else {
		fd2 = fork();
		if(fd2 == -1)
			errxt("fork");
		if(fd2 == 0){
			close(0); /*close normal stdin*/
			dup(p[0]);
			close(p[1]);
			execvp(argp2[0], argp2);
			close(p[0]);
			exit(EXIT_SUCCESS);
		} else {
			close(p[0]);
			close(p[1]);
			wait(&status);
			wait(NULL);
		}
	}
}

void rednfun(char *arg1[], char *arg2[])
{
	int fd, fdw;

	fdw = open(arg2[0], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	fd = fork();
	if(fd == -1)
		errxt("fork");
	if(fd == 0){
		close(1);
		dup(fdw);
		execvp(arg1[0], arg1);
		errxt("execvpr");
	} else{
		wait(NULL);
	}
}

int parsarg(char p[], char *dlm, int size, char *argv[])
{
	int i, nwords = 0, argc = 0;
	
	for(i = 0;  i < size; i++){
		if(p[i] == ' ' || p[i] == '\t')
			nwords++;
	}
	argv[0] = strtok(p, dlm);
	for(i = 1; i <= nwords; i++){
		argv[i] = strtok(NULL, dlm);
		if(argv[i] !=NULL)
			argc++;
		}
	argv[argc+1] = NULL;
	return argc;	
}

int PipRedirnCheck(char *ptr)
{
	int i, found;

	for(i = 0; i < sizeof(ptr); i++){
		if((ptr[i-1] == ' ')&&(ptr[i+1] == ' ')){
			if((ptr[i] == '|')||ptr[i] == '>'){
				found = YES;
			}
		}
	}
	return found;
}

void errxt(char *err)
{
	perror(err);
	exit(EXIT_FAILURE);
}

void initclear(char *argv[])
{
	int i;
	for(i = 0; i < MAXW; i++){
		argv[i] = NULL;
	}
}
