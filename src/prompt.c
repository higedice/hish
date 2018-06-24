#include <stdio.h>
#include <unistd.h>
#include "prompt.h"


static char get_dollar_char();


int print_prompt()
{
	putchar(get_dollar_char());
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


