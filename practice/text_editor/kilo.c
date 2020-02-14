/*** includes ***/

// defines for the getline() function.
#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#include <ctype.h> // iscntrl();
#include <errno.h> // errno(), EAGAIN;
#include <stdio.h> // printf(), sscanf(), perror(), getline();
#include <stdlib.h> // atexit(), exit();
#include <sys/ioctl.h> // ioctl(), TIOCGWINSZ;
#include <sys/types.h> // ssize_t;
#include <termios.h> // tcgetattr(), tcsetattr(), ECHO, etc;
#include <unistd.h> // read(), write(), STDOUT_FILENO;
#include <string.h> // memcpy();

/*** defines ***/
#define CTRL_KEY(k) ((k) & 0x1f)
#define ABUF_INIT {NULL, 0}
#define KILO_VERSION "0.0.1"

enum editorKey {
    ARROW_LEFT = 1000,
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN,
    DELETE_KEY,
    HOME_KEY,
    END_KEY,
    PAGE_UP,
    PAGE_DOWN
};

/*** data ***/

// a data type for storing a row of text in our editor.
typedef struct erow {
    int size;
    char *chars;
} erow;

// The editor settings.
struct editorConfig {
    int cx, cy;
    int screenrows;
    int screencols;
    int numrows;
    erow row;
    struct termios orig_termios;
};

struct editorConfig E;

// the buffer of dynamic strings.
struct abuf {
    char *b;
    int len;
};

/*** terminal init ***/

void initEditor();
void enableRawMode();
void disableRawMode();
void die (const char *s);
int editorReadKey();
int getCursorPosition(int *rows, int *cols);
int getWindowSize(int *rows, int * cols);
void editorOpen(char* filename);
void abAppend(struct abuf *ab, const char *s, int len);
void abFree(struct abuf *ab);
void editorProcessKeypress();
void editorMoveCursor(int key);
void editorRefreshScreen();
void editorDrawRows();

/*** init ***/

int main(int argc, char* argv[]) {
    enableRawMode();
    initEditor();
    if (argc >= 2) {
        editorOpen(argv[1]);
    }

    while(1) {
        editorRefreshScreen();
        editorProcessKeypress();
    }
    
    // success.
    return 0;
}

void initEditor() {
    // assign the cursor position.
    E.cx = 0;
    E.cy = 0;

    E.numrows = 0;

    // get the size of the terminal.
    if (getWindowSize(&E.screenrows, &E.screencols) == -1) die("getWindowSize");
}

/*** terminal ***/

void enableRawMode() {
    if (tcgetattr(STDIN_FILENO, &E.orig_termios) == -1) die("tcgetattr");
    atexit(disableRawMode);

    struct termios raw = E.orig_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

void disableRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1)
        die("tcsetattr");
}

void die (const char *s) {
    // refrech the screen when it's died.
    editorRefreshScreen();
    system("clear");

    perror(s);
    write(STDOUT_FILENO, "\r\n", 3); // carrage return.
    exit(1);
}

int editorReadKey() {
    int nread;
    char c;
    // expects the input from an user. Then stores a character into the c var.
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
        if (nread == -1 && errno != EAGAIN) die("read");
    }

    // if we read an escape character...
    if (c == '\x1b') {
        char seq[3];

        // we immediately read the next two bytes into "seq" buffer.
        if (read(STDIN_FILENO, &seq[0], 1) != 1) return '\x1b';
        if (read(STDIN_FILENO, &seq[1], 1) != 1) return '\x1b';

        // depending on which escape sequence was given we recognize
        // and return a particular key.
        if (seq[0] == '[') {
            if (seq[1] >= '0' && seq[1] <= '9') {
                if (read(STDIN_FILENO, &seq[2], 1) != 1) return '\x1b';
                if (seq[2] == '~') {
                    switch (seq[1]) {
                        case '1': return HOME_KEY;
                        case '3': return DELETE_KEY;
                        case '4': return END_KEY;
                        case '5': return PAGE_UP;
                        case '6': return PAGE_DOWN;
                        case '7': return HOME_KEY;
                        case '8': return END_KEY;
                    }
                }
            }
            else {
                switch (seq[1]) {
                    case 'A': return ARROW_UP;
                    case 'B': return ARROW_DOWN;
                    case 'C': return ARROW_RIGHT;
                    case 'D': return ARROW_LEFT;
                    case 'H': return HOME_KEY;
                    case 'F': return END_KEY;
                }
            }
        }
        else if (seq[0] == 'O') {
            switch (c) {
                case 'H': return HOME_KEY;
                case 'F': return END_KEY;
            }
        }

        return '\x1b';
    }
    else {
        return c;   
    }
}

int getCursorPosition(int *rows, int *cols) {
    char buf[32];
    unsigned int i = 0;

    // write the \x1b[6n escape sequance to get the position of the cursor.
    if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4) return -1;

    // write the received result of the form [28;94R into the buffer.
    // we don't write the [ and R into the buffer.
    while (i < sizeof(buf) - 1) {
        if (read(STDIN_FILENO, &buf[i], 1) != 1) break;
        if (buf[i] == 'R') break;
        i++;
    }
    buf[i] = '\0';

    // check "is the result received from \x1b[6n an escape sequance?".
    if (buf[0] != '\x1b' || buf[1] != '[') return -1;
    // write the numbers of the obtained result from the buffer into
    // the rows and cols variables.
    if (sscanf(&buf[2], "%d;%d", rows, cols) != 2) return -1;

    return 0;
}

int getWindowSize(int *rows, int * cols) {
    struct winsize ws;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) return -1;
        return getCursorPosition(rows, cols);
    }
    else {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
}

/*** file i/o ***/

void editorOpen(char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) die("fopen");

    char* line = NULL;
    size_t linecap = 0; // line capacity
    ssize_t linelen;
    // store the first line of a file(from fp to line) and return
    // the number of characters read without the null byte '\0'.
    linelen = getline(&line, &linecap, fp);
    // if success:
    if (linelen != -1) {
        // remove the delimiter character, but not including the termi‐
        // nating null byte ('\0').
        while (linelen > 0 && (line[linelen - 1] == '\n' ||
                               line[linelen - 1] == '\r'))
            linelen--;

        // write the lenght of the read string into E.row.size
        // and copy the sting into the E.row.chars buffer.
        E.row.size = linelen;
        E.row.chars = malloc(linelen + 1);
        memcpy(E.row.chars, line, linelen);
        E.row.chars[linelen] = '\0';
        E.numrows = 1;
    }
    // free the allocated blocks of memory.
    free(line);
    fclose(fp);
}

/*** append buffer ***/

void abAppend(struct abuf *ab, const char *s, int len) {
    // reallocate the block of memory for append the given string to the
    // existing string in abuf.
    char *new = realloc(ab->b, ab->len + len);

    if (new == NULL) return;
    // copy the given string to the new space of the new block.
    memcpy(&new[ab->len], s, len);
    // update all params in the structure.
    ab->b = new;
    ab->len += len;
}

void abFree(struct abuf *ab) {
    free(ab->b);
}

/*** input ***/

void editorProcessKeypress() {
    // getting a character.
    int c = editorReadKey();

    switch (c) {
        case CTRL_KEY('q'):
            // refrech the screen when it's quited.
            editorRefreshScreen();
            system("clear");
            exit(0);
            break;

        case HOME_KEY:
            E.cx = 0;
            break;
        case END_KEY:
            E.cx = E.screencols - 1;
            break;

        case PAGE_UP: case PAGE_DOWN:
            // we move the cursor to up or down as many times as the size
            // of the terminal is.
            {
                int times = E.screenrows;
                c = (c == PAGE_UP) ? ARROW_UP : ARROW_DOWN;
                while (times--)
                    editorMoveCursor(c);
            }
            break;

        case ARROW_UP: case ARROW_DOWN: case ARROW_LEFT: case ARROW_RIGHT:
            editorMoveCursor(c);
            break;
    }
}

void editorMoveCursor(int key) {
    switch (key) {
        case ARROW_LEFT:
            if (E.cx > 0)
                E.cx--;
            break;
        case ARROW_RIGHT:
            if (E.cx < E.screencols - 1)
                E.cx++;
            break;
        case ARROW_UP:
            if (E.cy > 0)
                E.cy--;
            break;
        case ARROW_DOWN:
            if (E.cy < E.screenrows - 1)
                E.cy++;
            break;
    }
}

/*** output ***/

void editorRefreshScreen() {
    struct abuf ab = ABUF_INIT;
    // hide the cursor while the screen is refreshing.
    abAppend(&ab, "\x1b[?25l", 6);
    // move the cursor to the top-left corner.
    abAppend(&ab, "\x1b[H", 3);

    // draw the tildes.
    editorDrawRows(&ab);

    // and then move the cursor to the cx, cy position.
    char buf[32];
    snprintf(buf, sizeof(buf), "\x1b[%d;%dH", E.cy + 1, E.cx + 1);
    abAppend(&ab, buf, strlen(buf));

    // show the cursor after the screen was refreshing.
    abAppend(&ab, "\x1b[?25h", 6);

    write(STDOUT_FILENO, ab.b, ab.len);
    abFree(&ab);
}

void editorDrawRows(struct abuf *ab) {
    // my version.

    for (int y = 0; y < E.screenrows - 1; y++) {
        if (y >= E.numrows) {
            // draws a tilde and erases the part of the line to the
            // right of the cursor.
            if (E.numrows == 0 && y == E.screenrows / 3) {
                char welcome[80];
                int welcomelen = snprintf(welcome, sizeof(welcome),
                    "Kilo editor -- version %s", KILO_VERSION);
                if (welcomelen > E.screencols) welcomelen = E.screencols;

                int padding = (E.screencols - welcomelen) / 2;
                abAppend(ab, "~", 1);
                if (padding > 1) {
                    padding--;
                    while (padding--) abAppend(ab, " ", 1);
                    abAppend(ab, welcome, welcomelen);
                }
            }
            else {
                abAppend(ab, "~", 1);
            }
        }
        else {
            int len = E.row.size;
            if (len > E.screencols) len = E.screencols;
            abAppend(ab, E.row.chars, len);
        }

        abAppend(ab, "\x1b[K\r\n", 5);
    }
    abAppend(ab, "~", 1);

    // original version.
    /*
    for (int y = 0; y < E.screenrows; y++) {
        abAppend(ab, "~", 1);

        abAppend(ab, "\x1b[K", 3);
        if (y < E.screenrows - 1) {
            abAppend(ab, "\r\n", 2);
        }
    }
    */
}
