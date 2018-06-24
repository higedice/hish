#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "prompt.h"

#define HOSTNAME_BUFF_SIZE 512

static char get_dollar_char();
static char *get_host_name_string(char *hostname, char short_flag);
static char *get_pwd_string(char short_flag);



int print_prompt()
{
	char *ps1, *p, *env;
	char hostname[HOSTNAME_BUFF_SIZE];

	//ps1 = getenv("PS1");
	ps1 = "\\u@\\h \\$";
	p = ps1;

	if (ps1 == NULL || ps1[0] == '\0') {
		putchar(get_dollar_char());
		putchar(' ');
		return 0;
	}

	while (*p != '\0') {
		if (*p == '\\') {
			p++;
			switch (*p) {
			case 'a':
				putchar('\a');
				break;
			case 'd':
				putchar(*p);
				break;
			case 'e':
				putchar('\033');
				break;
			case 'h':
				printf("%s", get_host_name_string(hostname, 'h'));
				break;
			case 'H':
				printf("%s", get_host_name_string(hostname, 'H'));
				break;
			case 'n':
				putchar('\n');
				break;
			case 'r':
				putchar('\r');
				break;
			case 's':
				printf("%s", "hish");
				break;
			case 't':
				putchar(*p);
				break;
			case 'T':
				putchar(*p);
				break;
			case '@':
				putchar(*p);
				break;
			case 'u':
				env = getenv("USER");
				printf("%s", env ? env : "u");
				break;
			case 'v':
				putchar(*p);
				break;
			case 'V':
				putchar(*p);
				break;
			case 'w':
				printf("%s", get_pwd_string('w'));
				break;
			case 'W':
				printf("%s", get_pwd_string('W'));
				break;
			case '!':
				putchar(*p);
				break;
			case '#':
				putchar(*p);
				break;
			case '$':
				putchar(get_dollar_char());
				break;
			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':
				putchar(*p);
				break;
			case '\\':
				putchar(*p);
				break;
			case '[':
				putchar(*p);
				break;
			case ']':
				putchar(*p);
				break;
			default:
				putchar(*p);
			}
		} else {
			putchar(*p);
		}

		p++;
	}

	putchar(' ');

	return 0;
}


static char get_dollar_char()
{
	char c = '$';

	if (getuid() == 0) { // root
		c = '#';
	}

	return c;
}


static char *get_host_name_string(char *hostname, char short_flag)
{
	int err;
	char *p;

	err = gethostname(hostname, HOSTNAME_BUFF_SIZE);
	if (err != 0) {
		hostname = "";
	} else {
		if (short_flag == 'h') {
			p = strchr(hostname, '.');
			if (p != NULL) {
				*p = '\0';
			}
		}
	}

	return hostname;
}


static char *get_pwd_string(char short_flag)
{
	char *pwd, *p;

	pwd = getenv("PWD");

	if (short_flag == 'W') {
		p = strrchr(pwd, '/');
		if (p != NULL) {
			return ++p;
		}
	}

	return pwd;
}
