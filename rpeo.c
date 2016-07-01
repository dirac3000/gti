/*
 * gti - a git launcher
 *
 * Copyright 2012 by Richard Wossal <richard@r-wos.org>
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear in
 * supporting documentation.  No representations are made about the
 * suitability of this software for any purpose.  It is provided "as
 * is" without express or implied warranty.
 */

#if defined (_WIN32) && !defined(WIN32)
#define WIN32
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(WIN32) && !defined(__CYGWIN__)
#include <process.h>
#include <io.h>

// usleep() doesn't exist on MSVC, instead use Sleep() from Win32 API
#define usleep(a) Sleep((a) / 1000)

// exec*() on MSVC makes the parent process exit; that means that gti.exe will finish as git is starting,
// which causes cmd.exe to print its prompt over git's output (because it sees that the child process has
// finished). The solution is to use synchronous spawn*(): it will make gti.exe to wait until git finishes.
#define execv(a, b) do { i = _spawnv(_P_WAIT, (a), (b)); if (i != -1) return i; } while(0)
#define execvp(a, b) do { i = _spawnvp(_P_WAIT, (a), (b)); if (i != -1) return i; } while(0)

#else
#include <unistd.h>
#endif

#ifndef WIN32
#include <sys/ioctl.h>
#else
#include <windows.h>
#endif

// SunOS defines winsize in termios.h
#if defined(__sun) && defined(__SVR4)
#include <sys/termios.h>
#endif


#define REPO_NAME "repo"

#define xstr(s) str(s)
#define str(s) #s
#define ROBOT_HEIGHT 7
#define ROBOT_HEIGHT_STR xstr(ROBOT_HEIGHT)

#ifndef ROBOT_SPEED
#define ROBOT_SPEED 500
#endif

int  term_width(void);
void init_space(void);
void open_term();
void move_to_top(void);
void line_at(int start_x, const char *s);
void draw_robot(int x);
void clear_robot(int x);

int TERM_WIDTH;
FILE *TERM_FH;
int SLEEP_DELAY;

int main(int argc, char **argv)
{
    int i;
    (void) argc;

    open_term();
    TERM_WIDTH = term_width();
    SLEEP_DELAY = 2000000 / (TERM_WIDTH + ROBOT_SPEED);

    init_space();
    for (i = -20; i < TERM_WIDTH; i++) {
        draw_robot(i);
        usleep(SLEEP_DELAY);
        clear_robot(i);
    }
    move_to_top();
    fflush(TERM_FH);
    execvp(REPO_NAME, argv);

    /* error in exec if we land here */
    perror(REPO_NAME);
    return 1;
}

void init_space(void)
{
    int i;
    /* ROBOT_HEIGHT +1, do to not remove the PS1 line */
    for (i = 0; i < ROBOT_HEIGHT + 1; i++)
        fputs("\n", TERM_FH);
#ifdef WIN32
    fflush(TERM_FH);
#endif
}

#ifndef WIN32

void open_term()
{
    TERM_FH = fopen("/dev/tty", "w");
    if (!TERM_FH)
        TERM_FH = stdout;
}

int term_width(void)
{
    struct winsize w;
    ioctl(fileno(TERM_FH), TIOCGWINSZ, &w);
    return w.ws_col;
}

void move_to_top(void)
{
    fprintf(TERM_FH, "\033[" ROBOT_HEIGHT_STR "A");
}

void move_to_x(int x)
{
    fprintf(TERM_FH, "\033[%dC", x);
}

#else

HANDLE con;

void open_term()
{
    TERM_FH = fopen("CONOUT$", "w+");
    con = (HANDLE)_get_osfhandle(fileno(TERM_FH));
}

int term_width(void)
{
    CONSOLE_SCREEN_BUFFER_INFO ci;
    GetConsoleScreenBufferInfo(con, &ci);
    return ci.dwSize.X;
}

void move_to_top(void)
{
    CONSOLE_SCREEN_BUFFER_INFO ci;
    GetConsoleScreenBufferInfo(con, &ci);
    ci.dwCursorPosition.X = 0;
    ci.dwCursorPosition.Y -= ROBOT_HEIGHT;
    SetConsoleCursorPosition(con, ci.dwCursorPosition);
}

void move_to_x(int x)
{
    CONSOLE_SCREEN_BUFFER_INFO ci;
    GetConsoleScreenBufferInfo(con, &ci);
    ci.dwCursorPosition.X = x;
    SetConsoleCursorPosition(con, ci.dwCursorPosition);
}

#endif

void line_at(int start_x, const char *s)
{
    int x;
    size_t i;
    if (start_x > 1)
        move_to_x(start_x);
    for (x = start_x, i = 0; i < strlen(s); x++, i++) {
        if (x > 0 && x < TERM_WIDTH)
            fputc(s[i], TERM_FH);
    }
#ifdef WIN32
    /*
     * It seems Windows wraps on whe cursor when it's about to overflow,
     * rather than after it has overflown (unless the overflowing character
     * is a newline), as other systems seems to do.
     */
    if (x < TERM_WIDTH)
#endif
    fputc('\n', TERM_FH);

#ifdef WIN32
    fflush(TERM_FH);
#endif
}

void draw_robot(int x)
{
    move_to_top();
    line_at(x, " ,-----.");
    line_at(x, ",       .");
    line_at(x, "|___* *__|");
    line_at(x, "|        |");
    line_at(x, "| /|     |");
    line_at(x, "|/ |     |");
    line_at(x, "/()|_____|");
}

void clear_robot(int x)
{
    int i;

    move_to_top();
    for (i = 0; i < ROBOT_HEIGHT; i++)
        line_at(x, "  ");
}

