#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <ncurses.h>

#define MAX_FILE_SIZE 512

const char *filename;
char* value_file;

static void init_window();
static void resize_window(int sig);
static void load_text(const char *filename);
static char* read_file(const char filename[]);

int main(int argc, char *argv[]) {
	int x, y, c;
	if(argc < 2)
	{
		fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
	}
	filename = argv[1];

	init_window();
	load_text(filename);

	char* s = read_file(filename);
	printw("%s", s);
	move(1, 0);

	getyx(stdscr, y, x);

	while (1)
	{
		c = getch();
		switch (c)
		{
			case KEY_LEFT:
				x = (x - 1 < 0) ? 0 : x - 1;
				break;
			case KEY_RIGHT:
				x = (x + 1 >= COLS) ? COLS - 1 : x + 1;
				break;
			case KEY_UP:
				y = (y - 1 < 0) ? 0 : y - 1;
				break;
			case KEY_DOWN:
				y = (y + 1 >= LINES) ? LINES - 1 : y + 1;
				break;
			case KEY_BACKSPACE:
				x = (x - 1 < 0) ? 0 : x - 1;
				delch();
				break;
			case 27:
		        endwin();
				free(value_file);
				return 0;
			default:
				x = (x + 1 >= COLS) ? COLS - 1 : x + 1;
				break;
		}

		move(y, x);
		refresh();
	}

	refresh();
	getch();
	endwin();

	free(value_file);

	return 0;
}

static void init_window()
{
	initscr();
	raw();
	keypad(stdscr, TRUE);
	curs_set(1);
	signal(SIGWINCH, resize_window);
}

static void resize_window(int sig)
{
	endwin();
	refresh();
	init_window();
	load_text(filename); // Use global variable. Code in main: filename = argv[1];
}

static void load_text(const char *filename)
{
	move(0, 0);
	clrtoeol();
	refresh();

	move(0, (getmaxx(stdscr) - strlen("GPL 3.0 CNuteConsole") - strlen(filename)) / 2);
	attron(A_STANDOUT);
	printw("%s %s\n", "GPL 3.0 CNute Console", filename);
	attroff(A_STANDOUT);

	refresh();
}

static char* read_file(const char filename[]) {
    value_file = malloc(MAX_FILE_SIZE + 1);
    FILE *file;

    file = fopen(filename, "r");
    if (file == NULL)
        return NULL;

    fread(value_file, 1, MAX_FILE_SIZE, file);
    value_file[MAX_FILE_SIZE] = '\0';
    fclose(file);

    return value_file;
}
