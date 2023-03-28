/**********************************************************************
 * Copyright (c) 2020-2023
 *  Sang-Hoon Kim <sanghoonkim@ajou.ac.kr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTIABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 **********************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

/***********************************************************************
 * run_command()
 *
 * DESCRIPTION
 *   Implement the specified shell features here using the parsed
 *   command tokens.
 *
 * RETURN VALUE
 *   Return 1 on successful command execution
 *   Return 0 when user inputs "exit"
 *   Return <0 on error
 */
int run_command(int nr_tokens, char *tokens[])
{
	static char aliaslist[32][128]; 
	static int aliascnt = 0;
	int ispipe = 0;
	if (strcmp(tokens[0], "exit") == 0) return 0;

	if(strcmp(tokens[0], "alias") == 0){
		if(tokens[1]==NULL){
			for(int i=0;i<aliascnt;i++) fprintf(stderr, "%s: %s\n", aliaslist[i], getenv(aliaslist[i]));
		}
		else{
			strcpy(aliaslist[aliascnt++],tokens[1]);

			char tmp[4096] = { '\0' };
			strcat(tmp, tokens[2]);
			for(int i=3;i<nr_tokens;i++){
				strcat(tmp, " ");
				strcat(tmp, tokens[i]);
			}
			strcat(tmp, "\0");
			setenv(tokens[1], tmp, 0);
		}
		return 1;
	}

	char tmp[4096] = { '\0' };
	for(int i=0;i<nr_tokens;i++){
		if(getenv(tokens[i])!=NULL) strcat(tmp, getenv(tokens[i]));
		else strcat(tmp, tokens[i]);
		strcat(tmp, " ");
	}

	char *cmd = strtok(tmp, " ");
	char *arg1[32] = { NULL };
	char *arg2[32] = { NULL };
	int idx1 = 0;
	int idx2 = 0;
	arg1[idx1++] = strdup(cmd);
	while((cmd = strtok(NULL, " "))){
		if(strcmp(cmd, "|")==0){
			ispipe = 1;
			continue;
		}
		if(ispipe) arg2[idx2++] = strdup(cmd);
		else arg1[idx1++] = strdup(cmd);
	}
	arg1[idx1] = NULL;
	arg2[idx2] = NULL;

	if(ispipe){
		int pipefd[2];
		pipe(pipefd);

		pid_t pid = fork();
		if(pid==0){
			close(pipefd[1]);
			dup2(pipefd[0], STDIN_FILENO);
			close(pipefd[0]);
			execvp(arg2[0], arg2);
			exit(-1);
		}
		else{
			pid_t ppid = fork();
			if(ppid==0){
				close(pipefd[0]);
				dup2(pipefd[1], STDOUT_FILENO);
				close(pipefd[1]);
				execvp(arg1[0], arg1);
				exit(-1);
			}
			else{
				close(pipefd[1]);
				close(pipefd[0]);
				int ret;
				wait(&ret);
				if(WEXITSTATUS(ret) == 0) return 1;
			}
			int ret;
			wait(&ret);
			if(WEXITSTATUS(ret) == 0) return 1;
		}
	}
	else{
		if (strcmp(arg1[0], "cd") == 0){
			if(arg1[1]==NULL||strcmp(arg1[1], "~")==0){
				chdir(getenv("HOME"));
			}
			else chdir(arg1[1]);
			return 1;
		}

		pid_t pid = fork();
		if(pid==0){
			execvp(arg1[0], arg1);
			exit(-1);
		}
		else{
			int ret;
			wait(&ret);
			if(WEXITSTATUS(ret) == 0) return 1;
		}
	}

	fprintf(stderr, "Unable to execute %s\n", arg1[0]);
	for (int i=0;i<idx1;i++) {
		free(arg1[i]);
	}
	for (int i=0;i<idx2;i++) {
		free(arg2[i]);
	}
	return 1;
}


/***********************************************************************
 * initialize()
 *
 * DESCRIPTION
 *   Call-back function for your own initialization code. It is OK to
 *   leave blank if you don't need any initialization.
 *
 * RETURN VALUE
 *   Return 0 on successful initialization.
 *   Return other value on error, which leads the program to exit.
 */
int initialize(int argc, char * const argv[])
{
	return 0;
}


/***********************************************************************
 * finalize()
 *
 * DESCRIPTION
 *   Callback function for finalizing your code. Like @initialize(),
 *   you may leave this function blank.
 */
void finalize(int argc, char * const argv[])
{
}

