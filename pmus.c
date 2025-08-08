#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#include <X11/keysym.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "config.h"

#define EXIT_CLEANUP(status)                             \
	do {                                             \
		printf("Exiting.\n");                    \
		XUngrabKeyboard(display, CurrentTime);   \
		XCloseDisplay(display);                  \
		return (status);                         \
	} while (0)

#define ARRAY_LEN(arr) (sizeof(arr) / sizeof((arr)[0]))

static int play_music(char *, pid_t *);
static void GrabKeyboard(void);
static void die(char *);

static Window root;
static Display *display;

int main() {
	KeySym keysym;
	XEvent event;
	pid_t pid;
	int isplaying = 0;

	GrabKeyboard();
	while (1) {
		XNextEvent(display, &event);
		if (event.type != KeyPress)
			continue;
		keysym = XLookupKeysym(&event.xkey, 0);


		if (isplaying) {
			kill(pid, 9);
			EXIT_CLEANUP(0);
		}

		int len = ARRAY_LEN(binding);
		for (int i = 0; i < len ; i++) {
			printf("%i is pressed %i expected\n", keysym, binding[i].keysym);
			if (keysym == binding[i].keysym) {
				play_music(binding[i].key, &pid);
				isplaying = 1;
			}
		}

		if (!isplaying) {
			EXIT_CLEANUP(1);
		}

		XFlush(display);
	}
	return 0;
}

void
die(char *errmsg)
{
	fprintf(stderr, "%s\n", errmsg);
	exit(1);
}

void
GrabKeyboard(void)
{
	display = XOpenDisplay(NULL);
	if (!display)
		die("Failed to open Xdisplay");

	struct timespec ts = { .tv_sec = 0, .tv_nsec = 1000000  };

	root = DefaultRootWindow(display);
	/* Try to grab keyboard, we may have to wait for another process to ungrab */
	for (int i = 0; i < 1000; i++) {
		if (XGrabKeyboard(display, root, True, GrabModeAsync, GrabModeAsync, CurrentTime) == GrabSuccess)
			return;

		nanosleep(&ts, NULL);
	}
	die("Failed to grab keyboard");
}

int
play_music(char *musicfile, pid_t *pid)
{
	char *args[] = {MUSIC_PLAYER, NULL, NULL};
	struct passwd *pw = getpwuid(getuid());
	const char *homedir = pw->pw_dir;


	char file[256];
	sprintf(file, "%s/%s/%s", homedir, MUSIC_DIRECTORY, musicfile);
	args[1] = file;

	*pid = fork();
	if (*pid == 0)  {
		XUngrabKeyboard(display, CurrentTime);
		execvp(MUSIC_PLAYER, args);
	}
	return 0;
}
