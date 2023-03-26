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
	if (strcmp(tokens[0], "exit") == 0) return 0;

	if(strcmp(tokens[0], "alias") == 0){
		if(tokens[1]==NULL);
		else{
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
	char *arg[32] = { NULL };
	int idx = 0;
	arg[idx++] = strdup(cmd);
	while((cmd = strtok(NULL, " "))){
		arg[idx++] = strdup(cmd);
	}
	arg[idx] = NULL;

	if (strcmp(arg[0], "cd") == 0){
		if(arg[1]==NULL||strcmp(arg[1], "~")==0){
			chdir(getenv("HOME"));
		}
		else chdir(arg[1]);
		return 1;
	}

	pid_t pid = fork();
	if(pid==0){
		execvp(arg[0], arg);
		exit(-1);
	}
	else{
		int ret;
		wait(&ret);
		if(WEXITSTATUS(ret) == 0) return 1;
	}

	fprintf(stderr, "Unable to execute %s\n", arg[0]);
	for (int i=0;i<idx;i++) {
		free(arg[i]);
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

