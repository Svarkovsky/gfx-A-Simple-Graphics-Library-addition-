/*
    A simple graphics library for CSE 20211 by Douglas Thain
    For complete documentation, see:
    http://www.nd.edu/~dthain/courses/cse20211/fall2011/gfx
    Version 3, 11/07/2012 - Now much faster at changing colors rapidly.
    Version 2, 9/23/2011 - Fixes a bug that could result in jerky animation.
*/

/*
	Copyright (c) 2019 Ivan Svarkovsky
	06/19/2019 - Expanded and supplemented
*/

#ifndef GFX_H
#define GFX_H

/* Open a new graphics window. */
void gfx_open(int width, int height, const char *title);

/* Draw a point at (x,y) */
void gfx_point(int x, int y);

/* Draw a line from (x1,y1) to (x2,y2) */
void gfx_line(int x1, int y1, int x2, int y2);

/* Draw a string */
void gfx_string(int x, int y, char *cc);

/* Draw one circle */
void gfx_circle(int x1, int y1, int width, int height);

/* Draw one fill circle */
void gfx_fill_circle(int x1, int y1, int width, int height);

/* Draw one rectangle */
void gfx_rectangle(int x1, int y1, int width, int height);

/* Draw one fill rectangle */
void gfx_fill_rectangle(int x1, int y1, int width, int height);

/* Change the current drawing color. */
void gfx_color(int red, int green, int blue);

/* Clear the graphics window to the background color. */
void gfx_clear();

/* Change the current background color. */
void gfx_clear_color(int red, int green, int blue);

/* Wait for the user to press a key or mouse button. */
char gfx_wait();

/* Return the X and Y coordinates of the last event. */
int gfx_xpos();
int gfx_ypos();

/* Return the X and Y dimensions of the window. */
int gfx_xsize();
int gfx_ysize();

/* Check to see if an event is waiting. */
int gfx_event_waiting(); // Waiting for any event

/* Flush all previous output to the window. */
void gfx_flush();

/* XGetPixel() function returns the specified pixel from the named image. */
int GetPix(int x, int y);

/* Read keys */
int gfx_xreadkeys();

/* With control of the number of events read key   */
int gfx_m_xreadkeys();

/* Moving window */
int gfx_move_win_l(int x, int y, int distance, int delay, int step);
int gfx_move_win_r(int x, int y, int distance, int delay, int step);
int gfx_move_win_u(int x, int y, int distance, int delay, int step);
int gfx_move_win_d(int x, int y, int distance, int delay, int step);

#endif
