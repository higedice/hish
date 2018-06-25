#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hish.h"
#include "builtin_commands.h"
#include "prompt.h"


static char **hish_split_line(char *line);
static char *hish_read_line(FILE *fpin);
static int hish_execute(char **args);
static int hish_launch(char **args);
static int load_config_files(void);
static int is_interactive(FILE *fpin);
static int fake_function_int_void(void);


int main(int argc, char **argv)
{
	FILE *fp;

	if (argc == 1) {
		load_config_files();
		// Run command loop.
		hish_loop(stdin);
	} else if (argc > 2) {
		fprintf(stderr, "hish: too many arguments.\n");
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



void hish_loop(FILE *fpin)
{
	char *line;
	char **args;
	int status;
	int (*pp)(void);

	if (is_interactive(fpin)) {
		pp = &print_prompt;
	} else {
		pp = &fake_function_int_void;
	}

	do {
		(*pp)();
		line = hish_read_line(fpin);
		args = hish_split_line(line);
		status = hish_execute(args);

		free(line);
		free(args);
	} while (status);
}



#define HISH_TOK_BUFSIZE 64
#define HISH_TOK_DELIM " \t\r\n\a"

static char **hish_split_line(char *line)
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



#define HISH_RL_BUFSIZE 1024

static char *hish_read_line(FILE *fpin)
{
	int bufsize = HISH_RL_BUFSIZE;
	int position = 0;
	char *buffer = malloc(sizeof(char) * bufsize);
	int c;
	char str_exit[] = "exit";

	if (!buffer) {
		fprintf(stderr, "hish: allocation error.\n");
		exit(EXIT_FAILURE);
	}

	while (1) {
		c = getc(fpin);

		if (c == EOF) {
			if (position == 0) {
				if (is_interactive(fpin)) {
					puts(str_exit);
				}
				if (HISH_RL_BUFSIZE > sizeof str_exit * sizeof(char) + 1) {
					strcpy(buffer, str_exit);
					return buffer;
				} else {
					exit(EXIT_FAILURE);
				}
			} else {
				buffer[position] = '\0';
				return buffer;
			}
		} else if (c == '\n') {
			buffer[position] = '\0';
			return buffer;
		} else if (c == '#') {
			buffer[position] = '\0';
			do {
				c = getc(fpin);
			} while (c != EOF && c != '\n');
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


static int hish_execute(char **args)
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



static int hish_launch(char **args)
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


static int load_config_files(void)
{
	char *homedir, *filename, *splitter, *fullpath;
	FILE *fp;
	char *source_args[3] = {"source", NULL, NULL};

	homedir = getenv("HOME");
	filename = ".hishrc";
	splitter = "/";

	fullpath = malloc(strlen(homedir) + strlen(splitter) + strlen(filename) + 1);
	if (!fullpath) {
		fprintf(stderr, "hish: allocation error.\n");
		exit(EXIT_FAILURE);
	}
	fullpath[0] = '\0';
	strcat(fullpath, homedir);
	strcat(fullpath, splitter);
	strcat(fullpath, filename);

	source_args[1] = fullpath;

	hish_execute(source_args);

	free(fullpath);

	return 0;
}



static int is_interactive(FILE *fpin)
{
	if (isatty(fileno(fpin)) && isatty(fileno(stdout))) {
		return 1;
	} else {
		return 0;
	}

}


static int fake_function_int_void(void) {
	return 0;
}


