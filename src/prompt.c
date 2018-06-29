#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <limits.h>
#include "prompt.h"

#define HOSTNAME_BUFF_SIZE 512
#define TIME_STR_BUFF_SIZE 256

static char get_dollar_char();
static char *get_host_name_string(char *hostname, char short_flag);
static char *get_pwd_string(char *pwdbuff, char short_flag);
static char *get_time_str(char *timebuff, const char *format);


int print_prompt()
{
	char *ps1, *p, *env;
	char hostname[HOSTNAME_BUFF_SIZE];
	char timebuff[TIME_STR_BUFF_SIZE];
	char pwdbuff[PATH_MAX];

	//ps1 = getenv("PS1");
	ps1 = "\\u@\\h \\W \\$";
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
				printf("%s", get_time_str(timebuff, "%a %b %d"));
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
				printf("%s", get_time_str(timebuff, "%H:%M:%S"));
				break;
			case 'T':
				printf("%s", get_time_str(timebuff, "%I:%M:%S"));
				break;
			case '@':
				printf("%s", get_time_str(timebuff, "%I:%M %p"));
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
				printf("%s", get_pwd_string(pwdbuff, 'w'));
				break;
			case 'W':
				printf("%s", get_pwd_string(pwdbuff, 'W'));
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


static char *get_pwd_string(char *pwdbuff, char short_flag)
{
	char *p, *q, *homepath;
	int i;
	char rootchar = '/', homechar = '~';

	getcwd(pwdbuff, PATH_MAX);
	p = pwdbuff;

	if (pwdbuff == NULL) {
		pwdbuff[0] = '\0';
		return pwdbuff;
	}

	homepath = getenv("HOME");

	if (homepath == NULL) {
		homepath[0] = '\0';
	}

	for (i = 0; homepath[i] == pwdbuff[i] && homepath[i] != '\0'; i++) {
	}
	i--;

	if (i > 1 && i == strlen(homepath) - 1) {    // HOME Path to "~"
		pwdbuff[i] = homechar;
		p = &pwdbuff[i];
	}

	if (short_flag == 'W') {
		q = strrchr(p, rootchar);
		if (q != NULL && !(q[0] == rootchar && q[1] == '\0')) { // "/" is "/"
			return ++q;
		}
	}

	return p;
}


static char *get_time_str(char *timebuff, const char *format)
{
	time_t now;
	struct tm *tp;

	now = time(NULL);
	tp = localtime(&now);
	strftime(timebuff, TIME_STR_BUFF_SIZE - 1, format, tp);

	return timebuff;
}

