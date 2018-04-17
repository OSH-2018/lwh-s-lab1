#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>

int outputt(char *cmd);
int run (int x,char *spilt[128]);
int specialtreat(char cmd[256]);
int count;
int main() {
    char * args[128];
    while (1) 
	{
		char cmd[256];
		count=0;
        	printf("# ");
        	fflush(stdin);
        	fgets(cmd, 256, stdin);
        	int i,j;
        	for (i = 0; cmd[i] != '\n'; i++);
        	cmd[i] = '\0';
		for (j=0;j<256;j++)
		{
			if(cmd[j]=='|') 	count++;
		}
		//printf("count:%d\n",count);
		if (count!=0)
			specialtreat(cmd);
		else
			outputt(cmd);
	}
}
int specialtreat(char cmd[256])
{
	char *split[128];
	int i,j;
	int pre=0;
	int l=strlen(cmd);
	printf("l:%d\n",l);
	int n=0;
	for (i=0;i<=l;i++)
	{
		if (cmd[i]=='|'||i==l)
		{
			while(cmd[pre]==' ')
				pre++;
			j=i-1;
			while(cmd[j-1]==' ')
				j--;
			split[n]=&cmd[pre];
			split[n][j-pre+1]='\0';
			pre=i+1;
			//printf("%d: %s\n",n,split[n]);
			n++;
			//if (i==l)	printf("%d %d\n ",n,i);
		}	
	}
	run(count,split);
	return 1;
	
}
int run(int x,char *split[128])
{
	int fd[2];
	int i,j;
	pid_t gid;
	if (x==0)
	{
		 outputt(split[0]);
		 return 0;
	}
	else
	{
		if (pipe(fd)<=0)
		{
			printf("pipe error!\n");
			exit(0);
		}
		if ((gid=fork())<0)
		{
			printf("fork error!\n");
			exit(0);
		}
		if (gid==0)
		{
			pid_t pid=fork();
			if (pid<=0)
			{
				printf("pipe error!\n");
				exit(0);
			}
			if (pid==0)
			{
				close (fd[0]);
				dup2(fd[1],STDOUT_FILENO);
				close (fd[1]);
				run(count-1,split);
			}
			if (pid==1)
			{
				close(fd[1]);
				dup2(fd[0],STDIN_FILENO);
				close (fd[0]);
				wait(NULL);
				outputt(split[x]);
			}
		}
		if (gid==1) wait(NULL);
	}
	return 1;
}
int outputt(char *cmd)
{
	int i,j;
	char * args[128];
	args[0]=cmd;
	for (i = 0; *args[i]; i++)
            for (args[i+1] = args[i] + 1; *args[i+1]; args[i+1]++)
                if (*args[i+1] == ' ') 
		{
                    *args[i+1] = '\0';
                    args[i+1]++;
                    break;
                }
	args[i]=NULL;
	//for (j=0;j<=i;j++)
	//	printf("i:%d:/%s/\n",j,args[j]);
        if (!args[0])
		return 0;
        if (strcmp(args[0], "cd") == 0)
	{
            if (args[1])
	    {
               if (!chdir(args[1]))
	       {
			printf("chdir error!\n");
			exit(0);
	       }
               else return 0;
	    }
        }
        if (strcmp(args[0], "pwd") == 0) 
	{
            char wd[4096];
            puts(getcwd(wd, 4096));
            return 0;
	}
	int l;
	int temp=-1;
	char * arger;
        if (strcmp(args[0], "exit") == 0||(strcmp(args[0],"logout")==0))
             exit(0);
	if (strcmp(args[0],"export")==0)
	{
		if (args[1])
		{
			l=strlen(args[1]);
			for (i=0;i<l;i++)
			{
				if (args[1][i]=='=')
					temp=i;
			}
			printf("%d %d\n",temp,l);
			if (temp!=0&&temp!=-1&&temp!=l)
			{
				arger=args[i]+temp+1;
				args[1][temp]='\0';
				printf("%s\n",args[i]);
				printf("%s\n",arger);
			}
			else if (temp==-1&&args[2][0]=='='&&strlen(args[2])==1)
			{
				if (args[3])	arger=args[3];
				else 
				{
					printf("error\n");
					return 0;
				}
			}
			else if (temp==-1&&args[2][0]=='='&&strlen(args[2])>1)  arger=args[2]+1;
			else if (temp==l&&args[2])
			{
				arger=args[2];
				args[1][temp]='\0';
			}
			else
			{
				 printf("error");
				 return 0;
			}
			setenv(args[1],arger,1);
		}
		else 
		{
			printf("error input\n");
			return 0;
		}
		return 0;
	}
	//printf("here\n");
        pid_t pid = fork();
        if (pid == 0)
	{
	    //printf("here\n");
            execvp(args[0], args);
            return 255;
        }
        wait(NULL);
}
        	
