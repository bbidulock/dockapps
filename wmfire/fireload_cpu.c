/* fireload_cpu .. accessory to wmfire - Zinx Verituse */
/* e-mail @ zinx@linuxfreak.com */

#include <config.h>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#ifdef __NetBSD__
/*********************************************************************************
 * This NetBSD load code is by Hubert Feyrer, not me.  Thank him, NetBSD fans ;) *
 *********************************************************************************/
int getload(int cpunum, int shownice) {
       double load;
       getloadavg(&load, 1);
       return (int)(load * 100);
}
#else /* use linux code */
int getload(int cpunum, int shownice) {
	static int lastloadstuff = 0, lastidle = 0, lastnice = 0;
	int loadline = 0, loadstuff = 0, nice = 0, idle = 0;
	FILE *stat = fopen("/proc/stat", "r");
	char temp[128], *p;

	if (!stat) return 100;

	while (fgets(temp, 128, stat)) {
		if (!strncmp(temp, "cpu", 3) && (temp[3] == ' ' && cpunum == 0) || (temp[3] != ' ' && atol(&temp[3]) == cpunum)) {
			p = strtok(temp, " \t\n");
			loadstuff = atol(p = strtok(NULL, " \t\n"));
			nice = atol(p = strtok(NULL, " \t\n"));
			loadstuff += atol(p = strtok(NULL, " \t\n"));
			idle = atol(p = strtok(NULL, " \t\n"));
			break;
		}
	}

	fclose(stat);

	if (!lastloadstuff && !lastidle && !lastnice) {
		lastloadstuff = loadstuff; lastidle = idle; lastnice = nice;
		return 0;
	}

	if (shownice) {
		loadline = (loadstuff-lastloadstuff)+(idle-lastidle);
	} else {
		loadline = (loadstuff-lastloadstuff)+(nice-lastnice)+(idle-lastidle);
	}
	if (loadline)
		loadline = ((loadstuff-lastloadstuff)*100)/loadline;
	else loadline = 100;

	lastloadstuff = loadstuff; lastidle = idle; lastnice = nice;

	return loadline;
}
#endif

void do_help(char *prgname) {
	printf("%s [-c<cpunum>] [-n] [-h]\n", prgname);
}

int main(int argc, char *argv[]) {
	int ch, cpunum = 0, shownice = 1;

	while ((ch = getopt(argc, argv, "c:hn")) != EOF) {
		switch (ch) {
			case 'c':
				cpunum = atoi(optarg);
				break;
			case 'n':
				shownice = 0;
				break;
			default: /* h is handled here */
				do_help(argv[0]);
				exit(0);
		}
	}

	printf("fireload\t0\t100\n"); fflush(stdout);

	for (;;) {
		ch = getload(cpunum, shownice);
		printf("%d\t%d\n", ch, ch); fflush(stdout);
		usleep(100000);
	}

	exit(0);
}
