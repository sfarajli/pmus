#include <X11/keysym.h>

#define MUSIC_PLAYER "mpv"
#define MUSIC_DIRECTORY ".local/bin/music"

typedef struct {
	char key[50];
    	KeySym keysym;
} Pair;

Pair binding[] = {
	{"nientedadire.mp3", XK_n}
};
