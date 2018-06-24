#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "hish.h"
#include "builtin_commands.h"

static int hish_num_builtins(void);
static int hish_cd(char **args);
static int hish_source(char **args);
static int hish_help(char **args);
static int hish_exit(char **args);

static char *builtin_str[] = {
	"cd",
	"source",
	".",
	"help",
	"exit"
};


static int (*builtin_func[]) (char **) = {
	&hish_cd,
	&hish_source,
	&hish_source,
	&hish_help,
	&hish_exit
};


int exec_if_builtin_cmd(char *cmd, char **args)
{
	int i;

	for (i = 0; i < hish_num_builtins(); i++) {
		if (strcmp(args[0], builtin_str[i]) == 0) {
			return (*builtin_func[i])(args);
		}
	}

	return -1;
}


static int hish_num_builtins()
{
	return sizeof(builtin_str) / sizeof(char *);
}

static int hish_cd(char **args)
{
	if (args[1] == NULL) {
		fprintf(stderr, "hish: expected argument to \"cd\"\n");
	} else {
		if (chdir(args[1]) != 0) {
			perror("hish");
		}
	}

	return 1;
}


static int hish_source(char **args)
{
	FILE *fp;

	if (args[1] == NULL) {
		fprintf(stderr, "hish: expected argument to \"%s\"\n", args[0]);
	} else if (args[2] != NULL) {
		fprintf(stderr, "hish: too many arguments to \"%s\"\n", args[0]);
	} else {
		fp = fopen(args[1], "r");
		if (fp == NULL) {
			fprintf(stderr, "hish: can't open %s.\n", args[1]);
		} else {
			// Run command loop.
			hish_loop(fp);
			fclose(fp);
		}
	}

	return 1;
}



static int hish_help(char **args)
{
	int i;

	printf("HigeDice's Hish\n");
	printf("Type Program names and arguments, and hit enter.\n");
	printf("The following are built in:\n");

	for (i = 0; i< hish_num_builtins(); i++) {
		printf("  %s\n", builtin_str[i]);
	}

	printf("Use the man command for information on other programs.\n");
	printf("\n");
	printf("HigeDice http://www.ohigedokoro.xyz/\n");
	printf("         higedice@banjoyugi.net\n");
	printf("https://github.com/higedice/hish\n");
	printf("\n");

	return 1;
}


static int hish_exit(char **args)
{
	return 0;
}

