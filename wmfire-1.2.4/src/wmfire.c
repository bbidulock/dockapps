/******************************************/
/* Wmfire - Flaming Monitor Dock          */
/******************************************/

/******************************************/
/* This program is free software; you can redistribute it and/or
/* modify it under the terms of the GNU General Public License
/* as published by the Free Software Foundation; either version 2
/* of the License, or (at your option) any later version.
/* 
/* This program is distributed in the hope that it will be useful,
/* but WITHOUT ANY WARRANTY; without even the implied warranty of
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/* GNU General Public License for more details.
/* 
/* You should have received a copy of the GNU General Public License
/* along with this program; if not, write to the Free Software
/* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
/******************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <pwd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <math.h>

#include <gdk/gdk.h>
#include <gdk/gdkx.h>

#include <glibtop.h>
#include <glibtop/cpu.h>
#include <glibtop/mem.h>
#include <glibtop/netload.h>

#include "session.h"
#include "flamedefault.h"
#include "flamecorona.h"
#include "flameblue.h"
#include "flamegreen.h"

#include "master.xpm"
#include "icon.xpm"

/******************************************/
/* Defines                                */
/******************************************/

#define XMAX 56
#define YMAX 56
#define CMAPSIZE (XMAX * YMAX)
#define RGBSIZE (XMAX * YMAX * 3)
#define NCOLOURS 256
#define NFLAMES 4

#define FIRE_NONE	0
#define FIRE_CPU	1
#define FIRE_MEM	2
#define FIRE_NET	3
#define FIRE_FILE	4

#define NET_SPD_PPP	56
#define NET_SPD_ETH	100

#define UPDATE_SEC	4
#define UPDATE		1000000 / UPDATE_SEC
#define REFRESH		20000

/******************************************/
/* Structures                             */
/******************************************/

typedef struct {
	Display *display;	/* X11 display */
	GdkWindow *win;		/* Main window */
	GdkWindow *iconwin;	/* Icon window */
	GdkGC *gc;		/* Drawing GC */
	GdkPixmap *pixmap;	/* Main pixmap */
	GdkBitmap *mask;	/* Dockapp mask */

	int x;			/* Window X position */
	int y;			/* Window Y position */
	int sticky;		/* Window sticky */

	unsigned char *flame;
	unsigned char cmap[CMAPSIZE];
	unsigned char rgb[RGBSIZE];
} wmfire_data;

typedef struct {
	char *text;
	unsigned char *data;
} flame_data;

/******************************************/
/* Functions                              */
/******************************************/

int update_cpu();
int update_mem();
int update_net();
int update_file();
int change_cpu(int);
void change_flame(int);
GdkCursor *setup_cursor();
void burn_spot(int, int, int);
inline void draw_fire(unsigned int);
static void make_wmfire_dockapp();
void read_config(int, char **);
void do_help(void);

/******************************************/
/* Globals                                */
/******************************************/

static wmfire_data bm;

char *session_id = NULL;

flame_data fire[] = {
	{"Natural", flamedefault},
	{"Coronal", flamecorona},
	{"Blue", flameblue},
	{"Green", flamegreen},
};

int monitor = FIRE_CPU;
int load = 100;
int cpu_av = 1;
int cpu_id = 0;
int cpu_nice = 1;
char net_dev[16] = "ppp0";
int net_spd = 0;
char *file_name = NULL;
int file_max = 100;
int file_min = 0;
int cmap = 0;
int lock = 0;
int proximity = 0;
int broken_wm = 0;

/******************************************/
/* Main                                   */
/******************************************/

int
main(int argc, char **argv)
{
	GdkEvent *event;
	GdkCursor *cursor;
	int i;
	int timer = 0;
	int next = 0;

	/* Initialise random seed */
	srand(time(NULL));

	/* Initialize GDK */
	if (!gdk_init_check(&argc, &argv)) {
		fprintf(stderr, "GDK init failed. Check \"DISPLAY\" variable.\n");
		exit(-1);
	}

	/* Initialise invisible cursor */
	cursor = setup_cursor();

	/* Zero main data structures */
	memset(&bm, 0, sizeof (bm));

	/* Parse command line */
	read_config(argc, argv);
#ifdef SESSION
	smc_connect(argc, argv, session_id);
#endif

	/* Create dockapp window. creates windows, allocates memory, etc */
	make_wmfire_dockapp();

	while (1) {
		while (gdk_events_pending()) {
			event = gdk_event_get();
			if (event) {
				switch (event->type) {
				case GDK_DESTROY:
				case GDK_DELETE:
					gdk_cursor_destroy(cursor);
					exit(0);
					break;
				case GDK_BUTTON_PRESS:
					if (event->button.button == 1)
						next = 1;
					else if (event->button.button == 2)
						cpu_nice = !cpu_nice;
					else if (event->button.button == 3)
						change_flame(0);
					break;
				case GDK_SCROLL:
					if (event->scroll.direction == GDK_SCROLL_UP)
						next = 1;
					else if (event->scroll.direction == GDK_SCROLL_DOWN)
						next = 1;
					break;
				case GDK_ENTER_NOTIFY:
					proximity = 1;
					gdk_window_set_cursor(bm.win, cursor);
					break;
				case GDK_LEAVE_NOTIFY:
					proximity = 0;
					gdk_window_set_cursor(bm.win, NULL);
					break;
				default:
					break;
				}
				gdk_event_free(event);
			}
		}

		/* Change monitored statistics */
		if (next) {
			next = 0;
			
			if (!lock) {
				if (monitor == FIRE_CPU && change_cpu(-1))
					monitor = FIRE_MEM;
				else if (monitor == FIRE_MEM)
					monitor = FIRE_NET;
				else if (monitor == FIRE_NET)
					if (file_name)
						monitor = FIRE_FILE;
					else
						monitor = FIRE_CPU;
				else if (monitor == FIRE_FILE)
					monitor = FIRE_CPU;
			}
		}

		/* Update statistics at intervals */
		if ((timer += REFRESH) >= UPDATE) {
			if (monitor == FIRE_CPU)
				load = update_cpu();
			else if (monitor == FIRE_MEM)
				load = update_mem();
			else if (monitor == FIRE_NET)
				load = update_net();
			else if (monitor == FIRE_FILE)
				load = update_file();

			if (load > 100)
				load = 100;
			else if (load < 0)
				load = 0;

			timer = 0;
		}

		draw_fire(load);

		usleep(REFRESH);

		/* Draw the rgb buffer to screen */
		if (!broken_wm)
			gdk_draw_rgb_image(bm.iconwin, bm.gc, 4, 4, XMAX, YMAX, GDK_RGB_DITHER_NONE, bm.rgb, XMAX * 3);
		else
			gdk_draw_rgb_image(bm.win, bm.gc, 4, 4, XMAX, YMAX, GDK_RGB_DITHER_NONE, bm.rgb, XMAX * 3);
	}

	return 0;
}

/******************************************/
/* Update cpu load statistics             */
/******************************************/

int
update_cpu()
{
	glibtop_cpu cpu;
	unsigned long load, total;
	static unsigned long oldload = 0, oldtotal = 0;
	int percent;

	glibtop_get_cpu(&cpu);

	if (cpu_av) {
		if (cpu_nice)
			load = cpu.user + cpu.nice + cpu.sys;
		else
			load = cpu.user + cpu.sys;

		total = cpu.total;
	} else {
		if (cpu_nice)
			load = cpu.xcpu_user[cpu_id] + cpu.xcpu_nice[cpu_id] + cpu.xcpu_sys[cpu_id];
		else
			load = cpu.xcpu_user[cpu_id] + cpu.xcpu_sys[cpu_id];

		total = cpu.xcpu_total[cpu_id];
	}

	if (total != oldtotal)
		percent = 100 * (load - oldload) / (total - oldtotal);
	else
		percent = 0;

	oldload = load;
	oldtotal = total;

	return percent;
}

/******************************************/
/* Update memory statistics               */
/******************************************/

int
update_mem()
{
	glibtop_mem mem;
	int percent;

	glibtop_get_mem(&mem);

	if (mem.total)
		percent = 100 * (mem.user + mem.shared) / mem.total;
	else
		percent = 0;

	return percent;
}

/******************************************/
/* Update network statistics              */
/******************************************/

int
update_net()
{
	glibtop_netload netload;
	static guint64 oldtotal = 0;
	int percent;

	glibtop_get_netload(&netload,net_dev);

	/* Disregard full duplex transmission */
	percent = 100 * (netload.bytes_total - oldtotal) / (net_spd / UPDATE_SEC);

	oldtotal = netload.bytes_total;

	return percent;
}

/******************************************/
/* Update file statistics                 */
/******************************************/

int
update_file()
{
	char buf[128];
	float percent, number;
	FILE *fp;

	if (!(fp = fopen(file_name, "r")))
		return 100;

	/* First number only. Complex parsing should be done in */
	/* external program and value saved to monitored file.  */

	fgets(buf, sizeof (buf), fp);
	number = atof(buf);
	fclose(fp);

	percent = 100 * (number - file_min) / (file_max - file_min);

	return (int) percent;
}

/******************************************/
/* Change CPU monitor                     */
/******************************************/

int
change_cpu(int which)
{
	glibtop_cpu cpu;

	glibtop_get_cpu(&cpu);

	/* This should work, but I have a lonely uniprocessor system */

	if (which >= 0) {
		cpu_id = which;
		cpu_av = 0;
	} else {
		cpu_id++;
		cpu_av = 0;
	}

	if (cpu_id >= glibtop_global_server->ncpu || cpu_id >= GLIBTOP_NCPU) {
		cpu_id = 0;
		cpu_av = 1;
		return 1;
	}
	
	return 0;
}

/******************************************/
/* Change flame effect                    */
/******************************************/

void
change_flame(int which)
{
	if (which > 0 && which <= NFLAMES)
		cmap = which - 1;
	else if (!lock)
		cmap++;

	cmap = cmap % NFLAMES;

	bm.flame = fire[cmap].data;
}

/******************************************/
/* Setup invisible cursor                 */
/******************************************/

GdkCursor *
setup_cursor()
{
	GdkPixmap *source, *mask;
	GdkColor col = { 0, 0, 0, 0 };
	GdkCursor *cursor;
	unsigned char hide[] = { 0x00 };

	/* No obviously invisible cursor available though */
	/* X/GDK, so using a custom 1x1 bitmap instead    */

	source = gdk_bitmap_create_from_data(NULL, hide, 1, 1);
	mask = gdk_bitmap_create_from_data(NULL, hide, 1, 1);

	cursor = gdk_cursor_new_from_pixmap(source, mask, &col, &col, 0, 0);

	gdk_pixmap_unref(source);
	gdk_pixmap_unref(mask);

	return cursor;
}

/******************************************/
/* Burn spot                              */
/******************************************/

void burn_spot(int x, int y, int c)
{
	int i;

	/* Co-ordinates from top left corner */
	for (i = y; i < (y+c); i++)
		memset(&bm.cmap[i*XMAX+x], 255, c);

}

/******************************************/
/* Draw fire                              */
/******************************************/

inline void
draw_fire(unsigned int load)
{
	int x, y, i, j;
	double psi;

	/* Setup hot spots */
	for (i = 0; i < ((load >> 3) + 2); i++)
		bm.cmap[(random() % XMAX) + ((YMAX - 1) * XMAX)] = random() % NCOLOURS;
	for (i = 0; i < ((100 - load) >> 4); i++)
		bm.cmap[(random() % XMAX) + ((YMAX - 1) * XMAX)] = bm.cmap[i + ((YMAX - 1) * XMAX)] >> 1;

	/* Mouse in window */
	if (proximity) {
		gdk_window_get_pointer(bm.win, &x, &y, NULL);

		/* Burn spot at mouse position */
		if ((y > 1 && y < 53) && (x > 1 && x < 53))
			burn_spot(x,y,3);

		/* Show after two seconds */
		if (proximity++ > 100) {

			if (monitor == FIRE_CPU) {
				if (cpu_av) {
					/* Horizontal bar for average cpu */
					memset(&bm.cmap[27 * XMAX + 20], 255, 16);
					memset(&bm.cmap[28 * XMAX + 20], 255, 16);
					memset(&bm.cmap[29 * XMAX + 20], 255, 16);
				} else {
					/* Horizontal line of spots per cpu */
					for (i = 0; i <= cpu_id; i++) {
						j = 5 + 44 * (i + 1) / (cpu_id + 2);
						burn_spot(j, 27, 3);
					}
				}
			} else if (monitor == FIRE_MEM) {
				/* Grid effect for memory arrays */
				for (i = 0; i < 4; i++) {
					for (j = 0; j < 4; j++)
						burn_spot(13+i*10, 13+j*10, 3);
				}
			} else if (monitor == FIRE_NET) {
				/* Marching ants for network traffic */
				for (i = 7; i < XMAX-10; i+=6) {
					j = i + (proximity/12) % 6;
					burn_spot(j, 27, 3);
				}
			} else if (monitor == FIRE_FILE) {
				/* Rotating disk platter */
				burn_spot(26, 26, 3);
				for (i = (proximity/4) % 4; i < 40; i+=4) {
					psi = i * 3.14 / 20.0;
					x = floor(sin(psi) * 18) + 26;
					y = floor(-cos(psi) * 8) + 26;
					burn_spot(x, y, 3);
				}
			}
		}
	}

	/* Flame algorithm */
	for (y = (YMAX - 2); y >= 0; y--) {
		i = y * XMAX;
		for (x = 1; x < (XMAX - 1); x++) {
			j = i + x;

			bm.cmap[j] = (bm.cmap[j + XMAX - 1] + bm.cmap[j + XMAX] + bm.cmap[j + XMAX + 1] + bm.cmap[j]) >> 2;
		}
	}

	/* Convert colourmap to rgb */
	for (i = 0; i < (CMAPSIZE - XMAX); i++)
		memcpy(&bm.rgb[i * 3], &bm.flame[bm.cmap[i] * 3], 3);
}

/******************************************/
/* Create dock app window                 */
/******************************************/

static void
make_wmfire_dockapp(void)
{
#define MASK GDK_BUTTON_PRESS_MASK | GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK | GDK_POINTER_MOTION_HINT_MASK

	GdkWindowAttr attr;
	GdkWindowAttr attri;
	Window win;
	Window iconwin;

	GdkPixmap *icon;

	XSizeHints sizehints;
	XWMHints wmhints;

	memset(&attr, 0, sizeof (GdkWindowAttr));

	attr.width = 64;
	attr.height = 64;
	attr.title = "wmfire";
	attr.event_mask = MASK;
	attr.wclass = GDK_INPUT_OUTPUT;
	attr.visual = gdk_visual_get_system();
	attr.colormap = gdk_colormap_get_system();
	attr.wmclass_name = "wmfire";
	attr.wmclass_class = "wmfire";
	attr.window_type = GDK_WINDOW_TOPLEVEL;

	/* Make a copy for the iconwin - parameters are the same */
	memcpy(&attri, &attr, sizeof (GdkWindowAttr));
	attri.window_type = GDK_WINDOW_CHILD;

	sizehints.flags = USSize;
	sizehints.width = 64;
	sizehints.height = 64;

	bm.win = gdk_window_new(NULL, &attr, GDK_WA_TITLE | GDK_WA_WMCLASS | GDK_WA_VISUAL | GDK_WA_COLORMAP);
	if (!bm.win) {
		fprintf(stderr, "FATAL: Cannot make toplevel window\n");
		exit(1);
	}

	bm.iconwin = gdk_window_new(bm.win, &attri, GDK_WA_TITLE | GDK_WA_WMCLASS);
	if (!bm.iconwin) {
		fprintf(stderr, "FATAL: Cannot make icon window\n");
		exit(1);
	}

	win = GDK_WINDOW_XWINDOW(bm.win);
	iconwin = GDK_WINDOW_XWINDOW(bm.iconwin);
	XSetWMNormalHints(GDK_WINDOW_XDISPLAY(bm.win), win, &sizehints);

	wmhints.initial_state = WithdrawnState;
	wmhints.icon_window = iconwin;
	wmhints.icon_x = 0;
	wmhints.icon_y = 0;
	wmhints.window_group = win;
	wmhints.flags = StateHint | IconWindowHint | IconPositionHint | WindowGroupHint;

	bm.gc = gdk_gc_new(bm.win);

	bm.pixmap = gdk_pixmap_create_from_xpm_d(bm.win, &(bm.mask), NULL, master_xpm);
	gdk_window_shape_combine_mask(bm.win, bm.mask, 0, 0);
	gdk_window_shape_combine_mask(bm.iconwin, bm.mask, 0, 0);

	gdk_window_set_back_pixmap(bm.win, bm.pixmap, False);
	gdk_window_set_back_pixmap(bm.iconwin, bm.pixmap, False);

#if 0
        gdk_window_set_type_hint(bm.win, GDK_WINDOW_TYPE_HINT_DOCK);
#else
        gdk_window_set_decorations(bm.win, 0);
        gdk_window_set_skip_taskbar_hint(bm.win, 1);
#endif

	icon = gdk_pixmap_create_from_xpm_d(bm.win, NULL, NULL, icon_xpm);
	gdk_window_set_icon(bm.win, bm.iconwin, icon, NULL);

	gdk_window_show(bm.win);

	/* Moved after gdk_window_show due to change in GTK 2.4 */
	XSetWMHints(GDK_WINDOW_XDISPLAY(bm.win), win, &wmhints);

	if (bm.x > 0 || bm.y > 0)
		gdk_window_move(bm.win, bm.x, bm.y);
	if (bm.sticky)
		gdk_window_stick(bm.win);
#undef MASK
}

/******************************************/
/* Read config file                       */
/******************************************/

void
read_config(int argc, char **argv)
{
	int i, j;

	/* Initialise flame to default */
	bm.flame = fire[cmap].data;

	/* Parse command options */
	while ((i = getopt(argc, argv, "c:mni:s:xF:H:L:pf:lbhg:yS:")) != -1) {
		switch (i) {
		case 'S':
			if (optarg)
				session_id = strdup(optarg);
			break;
		case 'g':
			if (optarg) {
				j = XParseGeometry(optarg, &bm.x, &bm.y, &j, &j);

				if (j & XNegative)
					bm.x = gdk_screen_width() - 64 + bm.x;
				if (j & YNegative)
					bm.y = gdk_screen_height() - 64 + bm.y;
			}
			break;
		case 'y':
			bm.sticky = 1;
			break;
		case 'c':
			monitor = FIRE_CPU;
			if (optarg)
				change_cpu(atoi(optarg));
			break;
		case 'm':
			monitor = FIRE_MEM;
			break;
		case 'n':
			monitor = FIRE_NET;
			break;
		case 'i':
			if (optarg)
				strncpy(net_dev, optarg, sizeof (net_dev));
			break;
		case 's':
			if (optarg)
				net_spd = atoi(optarg);
				if (strchr(optarg, 'k') || strchr(optarg, 'K'))
					net_spd = net_spd * 1024 / 8;
				else if (strchr(optarg, 'm') || strchr(optarg, 'M'))
					net_spd = net_spd * 1024 * 1024 / 8;
			break;
		case 'x':
			cpu_nice = 0;
			break;
		case 'F':
			monitor = FIRE_FILE;
			if (optarg)
				file_name = strdup(optarg);
			break;
		case 'H':
			if (optarg)
				file_max = atof(optarg);
			break;
		case 'L':
			if (optarg)
				file_min = atof(optarg);
			break;
		case 'p':
			monitor = FIRE_NONE;
			break;
		case 'f':
			if (optarg)
				change_flame(atoi(optarg));
			break;
		case 'l':
			lock = 1;
			break;
		case 'b':
			broken_wm = 1;
			break;
		case 'h':
		default:
			do_help();
			exit(1);
		}
	}

	if (!net_spd) {
		net_spd = NET_SPD_PPP * 1024 / 8;

		if (strstr(net_dev, "eth"))
			net_spd = NET_SPD_ETH * 1024 * 1024 / 8;
	}
}

/******************************************/
/* Help                                   */
/******************************************/

void
do_help(void)
{
	int i;

	fprintf(stderr, "\nWmfire - Flaming Monitor Dock V %s\n\n", VERSION);
	fprintf(stderr, "Usage: wmfire [ options... ]\n\n");
	fprintf(stderr, "\t-g [{+-}X{+-}Y]\t\tinital window position\n");
	fprintf(stderr, "\t-y\t\t\tset window sticky\n");
	fprintf(stderr, "\t-c [0..%d]\t\tmonitor smp cpu X\n", GLIBTOP_NCPU-1);
	fprintf(stderr, "\t-m\t\t\tmonitor memory load\n");
	fprintf(stderr, "\t-n\t\t\tmonitor network load\n");
	fprintf(stderr, "\t-F [...]\t\tmonitor file\n");
	fprintf(stderr, "\t-i [...]\t\tchange network interface (default:%s)\n", net_dev);
	fprintf(stderr, "\t-s [...]\t\tchange network speed (ppp:%dK) (eth:%dM)\n", NET_SPD_PPP, NET_SPD_ETH);
	fprintf(stderr, "\t-x\t\t\texclude nice'd cpu load\n");
	fprintf(stderr, "\t-H [...]\t\tset file maximum (high) value\n");
	fprintf(stderr, "\t-L [...]\t\tset file minumum (low) value\n");
	fprintf(stderr, "\t-p\t\t\tfire effect only\n");
	fprintf(stderr, "\t-f [1..%d]\t\tchange flame colour\n\t\t\t\t", NFLAMES);
	for (i = 0; i < NFLAMES; i++)
		fprintf(stderr, "%d:%s ", i + 1, fire[i].text);
	fprintf(stderr, "\n\t-l\t\t\tlock flame colour and monitor\n");
	fprintf(stderr, "\t-b\t\t\tactivate broken window manager fix\n");
	fprintf(stderr, "\t-h\t\t\tprints this help\n");
}
