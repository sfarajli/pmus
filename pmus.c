#include <X11/Xlib.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
// #include <signal.h>

#define music1 "gimmemore.opus"
#define music2 "sexyback.opus"
#define music3 "tillicollapse.opus"
#define music4 "drop.opus"

Display *display;

static void
grabkeyboard(void)
{
	struct timespec ts = { .tv_sec = 0, .tv_nsec = 1000000  };
	int i;

	/* try to grab keyboard, we may have to wait for another process to ungrab */
	for (i = 0; i < 1000; i++) {
		if (XGrabKeyboard(display, DefaultRootWindow(display), True, GrabModeAsync,
		                  GrabModeAsync, CurrentTime) == GrabSuccess)
			return;
		nanosleep(&ts, NULL);
	}

	fprintf(stderr, "Failed to grab the keyboard\n");
	exit(1);
}

int main() {
	// Display *display;
	Window root_window;
	XEvent event;
	struct passwd *pw = getpwuid(getuid());
	const char *homedir = pw->pw_dir;
	char *args[] = {"mpv", NULL, NULL};
	char *musicfile;
	int isplaying = 0;
	pid_t pid;

	// Open the X display
	display = XOpenDisplay(NULL);
	if (display == NULL) {
		fprintf(stderr, "Unable to open X display\n");
		exit(1);
	}

	// Get the root window (the desktop window)
	root_window = DefaultRootWindow(display);

	grabkeyboard();

	while (1) {
		XNextEvent(display, &event);

		if (event.type == KeyPress) {
			if (isplaying) {
				kill(pid, 9);					
				return 0;
			}

			switch (event.xkey.keycode) {
				case 10:
					musicfile = music1;
					break;
				case 11:
					musicfile = music2;
					break;
				case 12:
					musicfile = music3;
					break;
				case 13:
					musicfile = music4;
					break;
				default:
					return 1;

			}
			char file[256];
			sprintf(file, "%s/.local/bin/music/%s", homedir, musicfile);
			args[1] = file;

			isplaying = 1;
			pid = fork();
			if (pid == 0)  {
				XUngrabKeyboard(display, CurrentTime);
				execvp("mpv", args);
			}
		}
	}

	XUngrabKeyboard(display, CurrentTime);
	printf("Keyboard released.\n");
	XCloseDisplay(display);
	return 0;
}
