/* fireload_file .. accesory to wmfire - Zinx Verituse */
/* e-mail @ zinx@linuxfreak.com */

#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

float getheight(char *scanstring, char *filename) {
	char buffer[1024], *num;
	FILE *f;
	int ch;
	float numf;

	if (!(f = fopen(filename, "r"))) return 100;

	for (; *scanstring && !feof(f); scanstring++) {
		switch (*scanstring) {
			case '%':
				if (scanstring[1] == 's') {
					while (!feof(f) && isspace(ch = fgetc(f)));
					while (!feof(f) && !isspace(ch = fgetc(f)));
					scanstring++;
				}
				break;
			default:
				while (!feof(f) && (ch = fgetc(f)) != *scanstring);
				break;
		}
	}

	if (feof(f)) {
		fclose(f);
		return 100;
	}
	/* Premature EOF */

	fgets(buffer, 1024, f);

	num = buffer + strspn(buffer, " \t\n");
	num[strcspn(num, " \t\n")] = 0;
	numf = atof(num);

	fclose(f);
	
	return numf;
}

void do_help(char *prgname) {
	printf("%s -F <file> -S <string> -m <minimum> -x <maximum>\n", prgname);
}

char *showfmt(float num) {
	static char show[64];
	char *ext[] = { "", "K", "M", "G" };
	int extn;

	for (extn = 0; (num >= 1024) && extn < 3; extn++, num /= 1024) ; 
	snprintf(show, 64, "%.1f%s", num, ext[extn]);	

	return show;
}

int main(int argc, char *argv[]) {
	int ch;

	float min = 0, max = 0, num;
	char *filename = NULL, *scanstr = NULL;

	while ((ch = getopt(argc, argv, "F:S:m:x:h")) != EOF) {
		switch (ch) {
			case 'F':
				filename = optarg;
				break;
			case 'S':
				scanstr = optarg;
				break;
			case 'm':
				min = atof(optarg);
				break;
			case 'x':
				max = atof(optarg);
				break;
			default:
				do_help(argv[0]);
				exit(0);
				break;
		}
	}

	if (!filename || !scanstr || !(max - min)) {
		do_help(argv[0]);		
		exit(-1);
	}

	printf("fireload\t%f\t%f\n", min, max); fflush(stdout);

	for (;;) {
		num = getheight(scanstr, filename);
		printf("%f\t%s\n", num, showfmt(num)); fflush(stdout);
		usleep(100000);
	}

	exit(0);
}
