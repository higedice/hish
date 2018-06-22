#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "builtin_commands.h"
#include "prompt.h"


int hish_launch(char **args)
{
	pid_t pid;
	int status;

	pid = fork();
	if (pid == 0) {
		// Child process
		if (execvp(args[0], args) == -1) {
			perror("hish");
		}
		exit(EXIT_FAILURE);
	} else if (pid < 0) {
		// Error forking
		perror("hish");
	} else {
		// Parent process
		do {
			waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}

	return 1;
}


int hish_execute(char **args)
{
	int i;
	int status;

	if (args[0] == NULL) {
		// An empty command was entered.
		return 1;
	}

	if ((status = exec_if_builtin_cmd(args[0], args)) >= 0) {
		return status;
	}

	return hish_launch(args);
}


#define HISH_RL_BUFSIZE 1024

char *hish_read_line(FILE *fpin)
{
	int bufsize = HISH_RL_BUFSIZE;
	int position = 0;
	char *buffer = malloc(sizeof(char) * bufsize);
	int c;

	if (!buffer) {
		fprintf(stderr, "hish: allocation error.\n");
		exit(EXIT_FAILURE);
	}

	while (1) {
		c = getc(fpin);

		if (c == EOF) {
			exit(EXIT_SUCCESS);
		} else if (c == '\n') {
			buffer[position] = '\0';
			return buffer;
		} else {
			buffer[position] = c;
		} 
		position++;

		if (position >= bufsize) {
			bufsize += HISH_RL_BUFSIZE;
			buffer = realloc(buffer, bufsize);
			if (!buffer) {
				fprintf(stderr, "hish: allocation error.\n");
				exit(EXIT_FAILURE);
			}
		}
	}
}


#define HISH_TOK_BUFSIZE 64
#define HISH_TOK_DELIM " \t\r\n\a"

char **hish_split_line(char *line)
{
	int bufsize = HISH_TOK_BUFSIZE, position = 0;
	char **tokens = malloc(bufsize * sizeof(char*));
	char *token, **tokens_backup;

	if (!tokens) {
		fprintf(stderr, "hish: allocation error\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, HISH_TOK_DELIM);
	while (token != NULL) {
		tokens[position] = token;
		position++;

		if (position >= bufsize) {
			bufsize += HISH_TOK_BUFSIZE;
			tokens_backup = tokens;
			tokens = realloc(tokens, bufsize * sizeof(char*));
			if (!tokens) {
				free(tokens_backup);
				fprintf(stderr, "hish: allocation error\n");
				exit(EXIT_FAILURE);
			}
		}

		token = strtok(NULL, HISH_TOK_DELIM);
	}

	tokens[position] = NULL;

	return tokens;
}


void hish_loop(FILE *fpin)
{
	char *line;
	char **args;
	int status;

	do {
		if (fpin == stdin) {
			print_prompt();
		}
		line = hish_read_line(fpin);
		args = hish_split_line(line);
		status = hish_execute(args);

		free(line);
		free(args);
	} while (status);
}


int main(int argc, char **argv)
{
	FILE *fp;

	// load config files, if any.


	if (argc == 1) {
		// Run command loop.
		hish_loop(stdin);
	} else if (argc > 2) {
		fprintf(stderr, "hish: too match arguments.\n");
	} else {
		fp = fopen(argv[1], "r");
		if (fp == NULL) {
			fprintf(stderr, "hish: can't open %s.\n", argv[1]);
			exit(EXIT_FAILURE);
		} else {
			// Run command loop.
			hish_loop(fp);
			fclose(fp);
		}
	}
	if (ferror(stdout)) {
		fprintf(stderr, "hish: error writing stdout.\n");
		exit(EXIT_FAILURE);
	}



	// Perform any shutdown/cleanup.

	return EXIT_SUCCESS;
}







