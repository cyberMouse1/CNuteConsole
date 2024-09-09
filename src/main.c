#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <ncurses.h>

const char *filename;
char* value_file;

static void init_window();
static void resize_window(int sig);
static void load_text(const char *filename);
static char* read_file(const char filename[]);
static void display_text(char* text, int offset);

int main(int argc, char *argv[]) {
    int x, y, c;
    int offset = 0; // Offset for scrolling
    int total_lines = 0; // Total number of lines in the file

    if(argc < 2)
    {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }
    filename = argv[1];

    init_window();

    char* s = read_file(filename);
    if (s == NULL) {
        endwin();
        fprintf(stderr, "Failed to read file: %s\n", filename);
        return 1;
    }

    // Calculate total number of lines in the file
    for (int i = 0; i < strlen(s); i++) {
        if (s[i] == '\n') {
            total_lines++;
        }
    }
    total_lines++; // Add one for the last line

    // Display the initial text
    display_text(s, offset);
    refresh();

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
			    if (y > 0) {
                    y--;
                } else if (offset > 0) {
                    offset--;
                    clear();
                    display_text(s, offset);
                    move(0, 0);
                }
                break;
            case KEY_DOWN:
			    if (y < LINES - 1) {
                    y++;
                } else if (offset + LINES < total_lines) {
                    offset++;
                    clear();
                    display_text(s, offset);
                    move(0, 0);
                }
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
    load_text(filename);
}

static void resize_window(int sig)
{
    endwin();
    refresh();
    init_window();
    load_text(filename);
}

static void load_text(const char *filename)
{
    move(0, 0);
    clrtoeol();
    refresh();

    move(0, (getmaxx(stdscr) - strlen("GPL 3.0 CNute") - strlen(filename) - strlen("^S - Save Esc - Exit")) / 2);
    attron(A_STANDOUT);
    printw("%s %s ^S - Save Esc - Exit\n", "GPL 3.0 CNute", filename);
    attroff(A_STANDOUT);

    move(1, 0);
    refresh();
}

static char* read_file(const char filename[]) {
    FILE *file;
    long file_size;

    file = fopen(filename, "r");
    if (file == NULL)
        return NULL;

    // Get file size
    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    rewind(file);

    // Allocate memory for the file content
    value_file = malloc(file_size + 1);
    if (value_file == NULL) {
        fclose(file);
        return NULL;
    }

    // Read the file content
    fread(value_file, 1, file_size, file);
    value_file[file_size] = '\0';
    fclose(file);

    return value_file;
}

static void display_text(char* text, int offset) {
    int line_count = 0;
    int i = 0;
    int start_index = 0;

    // Find the starting index for the offset
    while (line_count < offset && i < strlen(text)) {
        if (text[i] == '\n') {
            line_count++;
        }
        i++;
    }
    start_index = i;

    // Display the visible part of the text
    line_count = 0;
    while (line_count < LINES && i < strlen(text)) {
        if (text[i] == '\n') {
            line_count++;
        }
        printw("%c", text[i]);
        i++;
    }

    move(1, 0);
}