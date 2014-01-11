#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
	int c;
	int opt;
	int skip = 0;
	while ((opt = getopt(argc, argv, "d")) != -1) {
		switch (opt) {
			case 'd':
				skip = 1;
				break;
			default:
				break;
		}
	}

	if (skip == 1) {
		while ((c = getchar()) != EOF)
			putchar(c);
	} else {
		while ((c = getchar()) != EOF) {
			if ((c >= 'A' && c <= 'Z') || (c >= '0' && c < '6'))
				putchar(c);
		}
	}
	return 0;
}
