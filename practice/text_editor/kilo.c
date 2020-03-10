/*** includes ***/

// defines for the getline() function.
#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#include <ctype.h> // iscntrl();
#include <errno.h> // errno(), EAGAIN;
#include <fcntl.h> // open(), O_RDWR, O_CREAT;
#include <stdio.h> // printf(), sscanf(), perror(), getline();
#include <stdarg.h> // va_list, va_start(), va_end();
#include <stdlib.h> // atexit(), exit();
#include <sys/ioctl.h> // ioctl(), TIOCGWINSZ;
#include <sys/types.h> // ssize_t;
#include <termios.h> // tcgetattr(), tcsetattr(), ECHO, etc;
#include <time.h> // time_t, time();
#include <unistd.h> // read(), write(), frtuncate(), close(), STDOUT_FILENO;
#include <string.h> // memcpy(), strdup(), memmove(), strerror();

/*** defines ***/
#define CTRL_KEY(k) ((k) & 0x1f)
#define ABUF_INIT {NULL, 0}
#define KILO_VERSION "0.0.1"
#define KILO_TAB_STOP 8
#define KILO_QUIT_TIMES 3

enum editorKey {
    BACKSPACE = 127,
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

// a data type (buffer) for storing a row of text in our editor.
typedef struct erow {
    int size; // size of a line.
    int rsize;
    char* chars; // the line.
    char* render;
} erow;

// The editor settings.
struct editorConfig {
    int cx, cy; // the cursor position in erow.
    int rx; // the x cursor position in render.
    int rowoff; // row offset.
    int coloff; // coloumn offset.
    int screenrows; // the heigth of the terminal.
    int screencols; // the weight of the terminal.
    int numrows; // the number of the file rows.
    erow *row; // buffer for storing a text from a file.
    int dirty; // dirty variable.
    char* filename; // the name of the opened file.
    char statusmsg[80];
    time_t statusmsg_time;
    struct termios orig_termios; // holds the terminal attributes.
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
void die (const char* s);
int editorReadKey();
int getCursorPosition(int* rows, int* cols);
int getWindowSize(int* rows, int* cols);
int editorRowCxToRx(erow* row, int cx);
void editorUpdateRow(erow* row);
void editorInsertRow(int at, char* s, size_t len);
void editorFreeRow(erow* row);
void editorDeleteRow(int at);
void editorRowAppendString(erow* row, char* s, size_t len);
void editorRowInsertChar(erow* row, int at, int c);
void editorRowDeleteChar(erow* row, int at);
void editorInsertChar(int c);
void editorInsertNewLine();
void editorDeleteChar();
char* editorRowsToString(int* buflen);
void editorSave();
void editorOpen(char* filename);
void abAppend(struct abuf* ab, const char* s, int len);
void abFree(struct abuf* ab);
void editorProcessKeypress();
void editorMoveCursor(int key);
void editorScroll();
void editorDrawStatusBar(struct abuf *ab);
void editorRefreshScreen();
void editorSetStatusMessage(const char* fmt, ...);
void editorDrawRows();

/*** init ***/

int main(int argc, char* argv[]) {
    // preparing of the terminal to an text editor view.
    enableRawMode();
    initEditor();
    // open the file.
    if (argc >= 2) {
        editorOpen(argv[1]);
    }

    editorSetStatusMessage("HELP: Ctrl-S = save | Ctrl-Q = quit");

    // run the editor process.
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
    // assing x position of the cursor in render.
    E.rx = 0;
    // initialization of the globas variables that keep track a row/column of the file
    // the user is currently scrolled to.
    E.rowoff = 0;
    E.coloff = 0;
    // assign the number of the text rows stored in the buffer.
    E.numrows = 0;
    // initialize the pointer of text row to NULL.
    E.row = NULL;
    // init the dirty variable to 0.
    E.dirty = 0;
    // initialize the pointer to the file name.
    E.filename = NULL;
    // initialize the status message to an empty string.
    E.statusmsg[0] = '\0';
    // initialize the counter of time that the message is shown by.
    E.statusmsg_time = 0;

    // get the size of the terminal.
    if (getWindowSize(&E.screenrows, &E.screencols) == -1) die("getWindowSize");
    // allocate the place for the status bar at the bottom of the terminal.
    E.screenrows -= 2;
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

/*** row operations ***/

int editorRowCxToRx(erow* row, int cx) {
    int rx = 0;
    for (int i = 0; i < cx; i++) {
        if (row->chars[i] == '\t')
            rx += (KILO_TAB_STOP - 1) - (rx % KILO_TAB_STOP);
        rx++;
    }
    return rx;
}

void editorUpdateRow(erow* row) {
    // count how many tabs keeps a line (also called row).
    int tabs = 0;
    for (int i = 0; i < row->size; i++)
        if (row->chars[i] == '\t') tabs++;

    // allocate the nessessary amount of memory.
    free(row->render);
    row->render = malloc(row->size + tabs * (KILO_TAB_STOP - 1) + 1);

    // copy the line from chars to render with spaces instead of tabs.
    int idx = 0;
    for (int i = 0; i < row->size; i++) {
        if (row->chars[i] == '\t')
            do
                row->render[idx++] = ' ';
            while (idx % KILO_TAB_STOP != 0);
        else
            row->render[idx++] = row->chars[i];
    }
    row->render[idx] = '\0';
    row->rsize = idx;
}

void editorInsertRow(int at, char* s, size_t len) {
    // valid the cursor position.
    if (at < 0 || at > E.numrows) return;

    // reallocate the block of memory for the new row.
    E.row = realloc(E.row, sizeof(erow) * (E.numrows + 1));
    // move the below text one position down.
    memmove(&E.row[at + 1], &E.row[at], sizeof(erow) * (E.numrows - at));

    // write the lenght of the read string into E.row.size
    // and copy the sting into the E.row.chars buffer.
    E.row[at].size = len;
    E.row[at].chars = malloc(len + 1);
    memcpy(E.row[at].chars, s, len);
    E.row[at].chars[len] = '\0';

    E.row[at].rsize = 0;
    E.row[at].render = NULL;
    editorUpdateRow(&E.row[at]);

    E.numrows++;
}

void editorFreeRow(erow* row) {
    /* frees the memory owned by "row". */

    free(row->render);
    free(row->chars);
}

void editorDeleteRow(int at) {
    // validate the "at" index.
    if (at < 0 || at >= E.numrows) return;
    editorFreeRow(&E.row[at]);
    // overwrite the deleted row struct with the rest of the rows that come after it.
    memmove(&E.row[at], &E.row[at + 1], sizeof(erow) * (E.numrows - at - 1));
    E.numrows--;
    E.dirty++;
}

void editorRowAppendString(erow* row, char* s, size_t len) {
    // reallocate the block of the memory for the append string.
    row->chars = realloc(row->chars, row->size + len + 1);
    // copy the string we want to the end of the precending row.
    memcpy(&row->chars[row->size], s, len);
    // increase the resultiing string size.
    row->size += len;
    // add the NULL byte to the end of the resulting string.
    row->chars[row->size] = '\0';
    // update the row to show the changes.
    editorUpdateRow(row);
    E.dirty++;
}

void editorRowInsertChar(erow* row, int at, int c) {
    // check if the cursor position is proper by "x".
    if (at < 0 || at > row->size) return;
    // realloc the space of the row for the new character (add 2 for the NULL byte).
    row->chars = realloc(row->chars, row->size + 2);
    // move the right part from "at" position to the right one position.
    memmove(&row->chars[at + 1], &row->chars[at], row->size - at + 1);
    // the size is increased.
    row->size++;
    // insert the given character to "at" position.
    row->chars[at] = c;
    // update the row to show the changes.
    editorUpdateRow(row);
    E.dirty++;
}

void editorRowDeleteChar(erow* row, int at) {
    // check if the cursor position is proper by "x".
    if (at < 0 || at > row->size) at = row->size;
    // move the right part from "at" position to the left one position.
    memmove(&row->chars[at - 1], &row->chars[at], row->size - at + 1);
    // reallocate the block of memory to free the space the deleted character was occupied.
    row->chars = realloc(row->chars, row->size);
    // deleting of a character means decrementing of a row size.
    row->size--;
    // update the row to show the changes.
    editorUpdateRow(row);
    //changes has been noticed.
    E.dirty++;
}

/*** editor operations ***/

void editorInsertChar(int c) {
    if (E.cy == E.numrows) {
        editorInsertRow(E.cy, "", 0);
    }
    
    // call the function to insert the given character.
    editorRowInsertChar(&E.row[E.cy], E.cx, c);
    // after inserting move the cursor forward.
    E.cx++;
}

void editorInsertNewLine() {
    if (E.cx == 0) {
        editorInsertRow(E.cy, "", 0);
    }
    else {
        erow* row = &E.row[E.cy];
        // split the line we are on into two rows.
        editorInsertRow(E.cy + 1, &row->chars[E.cx], row->size - E.cx);
        // rewrite the current line again.
        row = &E.row[E.cy];
        // truncate the current row's contents.
        row->size = E.cx;
        row->chars[row->size] = '\0';
        editorUpdateRow(row);
    }
    E.cy++;
    E.cx = 0;
}

void editorDeleteChar() {
    if (E.cx > 0) {
        // call the function which will delete a character to the next of the cursor.
        editorRowDeleteChar(&E.row[E.cy], E.cx);
        // after deleting move the cursor one to the left.
        E.cx--;
    }
    else if (E.cx == 0 && E.cy != 0) {
        E.cx = E.row[E.cy - 1].size;
        editorRowAppendString(&E.row[E.cy - 1], E.row[E.cy].chars, E.row[E.cy].size);
        editorDeleteRow(E.cy);
        E.cy--;
    }
}

/*** file i/o ***/

char* editorRowsToString(int* buflen) {
    /* converts the array of "erow" structs into a single string. */

    // count how many character has the buffer (len of the whole file).
    // we also add 1 to each one for the newline character. 
    int totlen = 0;
    for (int i = 0; i < E.numrows; i++)
        totlen += E.row[i].size + 1;
    *buflen = totlen;

    // copy the rows of the text to new variable.
    char* buf = malloc(totlen);
    // the second pointer to the allocated block of memory
    // where the string should by saved.
    char* p = buf;
    for (int i = 0; i < E.numrows; i++) {
        // copy a row to string variable
        memcpy(p, E.row[i].chars, E.row[i].size);
        // move to the end of the row
        p += E.row[i].size;
        // and change the NULL byte '\0' by '\n' escape sequence.
        *p = '\n';
        // move one byte forward.
        p++;
    }

    // return buf address excepting the caller free() the memory.
    return buf;
}

void editorSave() {
    // check the errors.
    if (E.filename == NULL) return;

    // get the length and the address of text.
    int len;
    char* buf = editorRowsToString(&len);

    // open the file for reading and wrtitting.(O_RDWR)
    // If it doesn't exist then crean a new file.(O_CREATE)
    // 0644 is the mode (the permissions) the new file should have.
    int fd = open(E.filename, O_RDWR | O_CREAT, 0644);
    if (fd != -1) {
        // set the file's size to the specified length.
        if (ftruncate(fd, len) != -1) {
            // write the byte to the file.
            if (write(fd, buf, len) == len) {
                // close the file (free RAM).
                close(fd);
                // free the block of memory containing the text.
                free(buf);
                // reset the dirty variable.
                E.dirty = 0;
                // print a message to the status bar.
                editorSetStatusMessage("%d bytes written to disk", len);
                return;
            }
        }
        // close the file (free RAM).
        close(fd);
    }
    // free the block of memory containing the text.
    free(buf);
    editorSetStatusMessage("Can't save! I/O error: %s", strerror(errno));
}

void editorOpen(char* filename) {
    // duplicate the file name to the global variable.
    free(E.filename);
    E.filename = strdup(filename);

    // open the given file.
    FILE* fp = fopen(filename, "r");
    if (!fp) die("fopen");

    char* line = NULL;
    size_t linecap = 0; // line capacity
    ssize_t linelen;
    
    // store each line of the file(from fp to line) while the end of the
    // file is not reached and return the number of characters read
    // without the null byte '\0'.
    while ((linelen = getline(&line, &linecap, fp)) != -1) {
        // remove the delimiter character, but not including the termi‐
        // nating null byte ('\0').
        while (linelen > 0 && (line[linelen - 1] == '\n' ||
                               line[linelen - 1] == '\r'))
            linelen--;
        
        // appending a string row to the buffer.
        editorInsertRow(E.numrows, line, linelen);
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
    static int quit_times = KILO_QUIT_TIMES;

    // getting a character.
    int c = editorReadKey();

    switch (c) {
        // react to the "enter" key.
        case '\r':
            editorInsertNewLine();
            break;

        case CTRL_KEY('q'):
            if (E.dirty && quit_times > 0) {
                editorSetStatusMessage("WARNING!!! File has unsaved changes. "
                "Press Ctrl-Q %d more times to quit", quit_times);
                quit_times--;
                return;
            }
            // refrech the screen when it's quited.
            editorRefreshScreen();
            system("clear");
            exit(0);
            break;

        case CTRL_KEY('s'):
            editorSave();
            break;

        case HOME_KEY:
            E.cx = 0;
            break;

        case END_KEY:
            E.cx = E.row[E.cy].size;
            break;

        case BACKSPACE:
        case CTRL_KEY('h'):
        case DELETE_KEY:
            if (c == DELETE_KEY) {
                // do nothing if when DELETE key pressed and the cursor is at the end of the file.
                if (E.cx == E.row[E.cy].size && E.cy == E.numrows - 1) return;
                editorMoveCursor(ARROW_RIGHT);
            }
            editorDeleteChar();
            break;

        case PAGE_UP:
        case PAGE_DOWN:
            // we move the cursor to up or down as many times as the size
            // of the terminal is.
            {
                // if the cursor is somewhere at the middle of the terminal then
                // move the cursor to the up or down of the terminal edge.
                if (E.cy > E.rowoff && E.cy < E.rowoff + E.screenrows - 1 &&
                    E.cy < E.numrows - E.screenrows) {
                    if (c == PAGE_UP)
                        E.cy = E.rowoff;
                    else if (c == PAGE_DOWN)
                        E.cy = E.rowoff + E.screenrows - 1;
                }
                // if the cursor if at the up/down-terminal-edge then
                // move the cursor through the entire terminal window size.
                else {
                    int times = E.screenrows;
                    c = (c == PAGE_UP) ? ARROW_UP : ARROW_DOWN;
                    while (times--)
                        editorMoveCursor(c);
                }
            }
            break;
        
        case ARROW_UP:
        case ARROW_DOWN:
        case ARROW_LEFT:
        case ARROW_RIGHT:
            editorMoveCursor(c);
            break;

        // ignore the 27 escape sequenses ("ESC" key).
        case CTRL_KEY('l'):
        case '\x1b':
            break;

        // else insert the character.
        default:
            editorInsertChar(c);
            break;
    }
    // reset quit_times.
    quit_times = KILO_QUIT_TIMES;
}

void editorMoveCursor(int key) {
    switch (key) {
        case ARROW_LEFT:
            if (E.cx > 0)
                E.cx--;
            else if (E.cy > 0) {
                // move the cursor one line up.
                E.cy--;
                // move the cursor to the end of the previous line.
                E.cx = E.row[E.cy].size;
            }
            break;
        case ARROW_RIGHT:
            if (E.cx < E.row[E.cy].size)
                E.cx++;
            else if (E.cy < E.numrows - 1) {
                // move the cursor one line down.
                E.cy++;
                // move the cursor to the beginning.
                E.cx = 0;
            }
            break;
        case ARROW_UP:
            if (E.cy > 0)
                E.cy--;
            break;
        case ARROW_DOWN:
            if (E.cy < E.numrows - 1)
                E.cy++;
            break;
    }

    int rowlen = E.row[E.cy].size;
    if (E.cx > rowlen) E.cx = rowlen;
}

/*** output ***/

void editorScroll() {
    E.rx = 0;
    if (E.cy < E.numrows)
        E.rx = editorRowCxToRx(&E.row[E.cy], E.cx);

    // adjusts E.rowoff depending on where the cursor is.
    if (E.cy < E.rowoff)
        E.rowoff = E.cy;
    else if (E.cy >= E.rowoff + E.screenrows)
        E.rowoff = E.cy - E.screenrows + 1;

    // adjusts E.coloff depending on where the cursor is.
    if (E.rx < E.coloff)
        E.coloff = E.rx;
    else if (E.rx >= E.coloff + E.screencols)
        E.coloff = E.rx - E.screencols + 1;
}

void editorDrawStatusBar(struct abuf *ab) {
    // the escape sequence <esc>[7m switches to inverted colors.
    abAppend(ab, "\x1b[7m", 4);

    // display the file name and the number of lines.
    // status[80] for showing the file name and the number of the file.
    // rstatus[80] for showing the cursor position.
    char status[80], rstatus[80];
    // format the status string.
    int len = snprintf(status, sizeof(status), "%.20s - %d lines. %s",
                       E.filename ? E.filename : "[No Name]", E.numrows,
                       E.dirty ? "(modified)" : "");
    // format the rstatus string.
    int rlen = snprintf(rstatus, sizeof(rstatus), "Ln: %i, Col: %i", E.cy + 1, E.cx + 1);
    // cut the status string if it's longer than the terminal weight.
    if (len > E.screencols) len = E.screencols;
    // don't display the rstatus string if it's longer than
    // the right space of the status bar.
    if (rlen > E.screencols - len - 1) rlen = 0;
    // display the file name and lines number.
    abAppend(ab, status, len);

    // fill the rest space with white spaces.
    for (; len < E.screencols - rlen; len++)
        abAppend(ab, " ", 1);

    // display the cursor position.
    abAppend(ab, rstatus, rlen);
    
    // the escape sequence <esc>[m switches back to normal formatting.
    abAppend(ab, "\x1b[m", 3);
    // move to the next line (for message status).
    abAppend(ab, "\r\n", 2);
}

void editorDrawMessageBar(struct abuf* ab) {
    // erase the line preparing to show the status message.
    abAppend(ab, "\x1b[K", 3);
    // get the status message lenght.
    int msglen = strlen(E.statusmsg);
    // cut if the status message is longer than the terminal wigth is.
    if (msglen > E.screencols) msglen = E.screencols;
    // show it for 5 seconds.
    if (msglen && time(NULL) - E.statusmsg_time < 5)
        abAppend(ab, E.statusmsg, msglen);
}

void editorRefreshScreen() {
    // checks should the program do a scroll.
    editorScroll();

    struct abuf ab = ABUF_INIT;
    // hide the cursor while the screen is refreshing.
    abAppend(&ab, "\x1b[?25l", 6);
    // move the cursor to the top-left corner.
    abAppend(&ab, "\x1b[H", 3);

    // draw the lines.
    editorDrawRows(&ab);
    // draw the status bar.
    editorDrawStatusBar(&ab);
    // draw the status message.
    editorDrawMessageBar(&ab);

    // and then move the cursor to the cx, cy position.
    char buf[32];
    snprintf(buf, sizeof(buf), "\x1b[%d;%dH", (E.cy - E.rowoff) + 1,
                                              (E.rx - E.coloff) + 1);
    abAppend(&ab, buf, strlen(buf));

    // show the cursor after the screen was refreshing.
    abAppend(&ab, "\x1b[?25h", 6);

    write(STDOUT_FILENO, ab.b, ab.len);
    abFree(&ab);
}

void editorSetStatusMessage(const char* fmt, ...) {
    // working with a variadic function.
    va_list ap;
    // start getting the arguments.
    va_start(ap, fmt);
    vsnprintf(E.statusmsg, sizeof(E.statusmsg), fmt, ap);
    va_end(ap);
    // end getting the arguments and assing the current time to a global variable.
    E.statusmsg_time = time(NULL);
}

void editorDrawRows(struct abuf *ab) {
    for (int y = 0; y < E.screenrows; y++) {
        int filerow = y + E.rowoff;
        if (filerow >= E.numrows) {
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
            // pring the text row-by-row.
            int len = E.row[filerow].rsize - E.coloff;
            if (len < 0) len = 0;
            if (len > E.screencols) len = E.screencols;
            abAppend(ab, &E.row[filerow].render[E.coloff], len);
        }

        abAppend(ab, "\x1b[K", 3);
        abAppend(ab, "\r\n", 2);
    }
}
