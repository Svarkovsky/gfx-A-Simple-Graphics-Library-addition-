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


#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <unistd.h>

#include <string.h>

#include "gfx.h"

/*  gfx_open creates several X11 objects, and stores them in globals
    for use by the other functions in the library.     */
// Создает несколько объектов X11 и сохраняет их в глобальных переменных для использования другими функциями в библиотеке.

static Display *gfx_display = 0;
static Window gfx_window;
static GC gfx_gc;
static Colormap gfx_colormap;
static int gfx_fast_color_mode = 0;

/* These values are saved by gfx_wait then retrieved later by gfx_xpos and gfx_ypos. */
// Эти значения сохраняются с помощью gfx_wait, а затем извлекаются с помощью gfx_xpos и gfx_ypos.
static int saved_xpos = 0;
static int saved_ypos = 0;

/* Open a new graphics window. */
// Открытие нового графического окна.
void gfx_open(int width, int height, const char *title)
{
    gfx_display = XOpenDisplay(0);
    if (!gfx_display)
    {
        fprintf(stderr, "gfx_open: unable to open the graphics window.\n");
        exit(1);
    }
    Visual *visual = DefaultVisual(gfx_display, 0);
    if (visual && visual->class == TrueColor)
    {
        gfx_fast_color_mode = 1;
    }
    else
    {
        gfx_fast_color_mode = 0;
    }
    int blackColor = BlackPixel(gfx_display, DefaultScreen(gfx_display));
    int whiteColor = WhitePixel(gfx_display, DefaultScreen(gfx_display));
    gfx_window = XCreateSimpleWindow(gfx_display, DefaultRootWindow(gfx_display), 0, 0, width, height, 0, blackColor, blackColor); // создать окно
    XSetWindowAttributes attr;
    attr.backing_store = Always;
    XChangeWindowAttributes(gfx_display, gfx_window, CWBackingStore, &attr);
    XStoreName(gfx_display, gfx_window, title);
    XSelectInput(gfx_display, gfx_window, StructureNotifyMask | KeyPressMask | ButtonPressMask); // Выбрать тип события, который будет обрабатываться XSelectInput().
    XMapWindow(gfx_display, gfx_window); // Показать окно с помощью функции XMapWindow().
    gfx_gc = XCreateGC(gfx_display, gfx_window, 0, 0); // XCreateGC - создание нового контекста вывода графики
    gfx_colormap = DefaultColormap(gfx_display, 0);
    XSetForeground(gfx_display, gfx_gc, whiteColor); // XSetForeground - установка цвета вывода примитивов графического контекста - http://www.asvcorp.ru/tech/linux/xwinprg/functions/XSetForeground.html
    // Wait for the MapNotify event
    for (;;)
    {
        XEvent e;
        XNextEvent(gfx_display, &e);
        if (e.type == MapNotify)
        {
            break;
        }
    }
}

/* Draw a single point at (x,y) */
// Нарисовать одну точку в (х, у)
void gfx_point(int x, int y)
{
    XDrawPoint(gfx_display, gfx_window, gfx_gc, x, y);
}

/* Draw a line from (x1,y1) to (x2,y2) */
void gfx_line(int x1, int y1, int x2, int y2)
{
    XDrawLine(gfx_display, gfx_window, gfx_gc, x1, y1, x2, y2);
}

/* Draw a string */
void gfx_string(int x, int y, char *cc)
{
    XDrawString(gfx_display, gfx_window, gfx_gc, x, y, cc, strlen(cc));
}

/* Draw one circle */
void gfx_circle(int x1, int y1, int width, int height)
{
    // width, height - ширина и высота образующего эллипса (круга); x1 и y1 - в данном случае - это центр окружности
    int angle1 = 0 * 64;
    int angle2 = 360 * 64;
    x1 = x1 - (width / 2);
    y1 = y1 - (height / 2);
    XDrawArc(gfx_display, gfx_window, gfx_gc, x1, y1, width, height, angle1, angle2);
} // http://www.asvcorp.ru/tech/linux/xwinprg/functions/XDrawArc.html

/* Draw one fill circle */
void gfx_fill_circle(int x1, int y1, int width, int height)
{
    // width, height - ширина и высота образующего эллипса (круга); x1 и y1 - в данном случае - это центр окружности
    int angle1 = 0 * 64;
    int angle2 = 360 * 64;
    x1 = x1 - (width / 2);
    y1 = y1 - (height / 2);
    XFillArc(gfx_display, gfx_window, gfx_gc, x1, y1, width, height, angle1, angle2);
} // http://www.asvcorp.ru/tech/linux/xwinprg/functions/XFillArc.html

/* Draw one rectangle */
void gfx_rectangle(int x1, int y1, int width, int height)
{
    XDrawRectangle(gfx_display, gfx_window, gfx_gc, x1, y1, width, height);    // http://www.asvcorp.ru/tech/linux/xwinprg/functions/XDrawRectangle.html
}

/* Draw one fill rectangle */
void gfx_fill_rectangle(int x1, int y1, int width, int height)
{
    XFillRectangle(gfx_display, gfx_window, gfx_gc, x1, y1, width, height);    // http://www.asvcorp.ru/tech/linux/xwinprg/functions/XFillRectangle.html
}

/* Change the current drawing color. */
// Изменить текущий цвет рисунка.
void gfx_color(int r, int g, int b)
{
    XColor color;
    if (gfx_fast_color_mode)
    {
        // Если это дисплей с истинным цветом, мы можем просто выбрать цвет напрямую.
        /* If this is a truecolor display, we can just pick the color directly. */
        color.pixel = ((b & 0xff) | ((g & 0xff) << 8) | ((r & 0xff) << 16));
    }
    else
    {
        // В противном случае мы должны выделить его из цветовой карты дисплея.
        /* Otherwise, we have to allocate it from the colormap of the display. */
        color.pixel = 0;
        color.red = r << 8;
        color.green = g << 8;
        color.blue = b << 8;
        XAllocColor(gfx_display, gfx_colormap, &color);
    }
    XSetForeground(gfx_display, gfx_gc, color.pixel);
}

/* Clear the graphics window to the background color. */
// Очистите графическое окно до цвета фона.
void gfx_clear()
{
    XClearWindow(gfx_display, gfx_window);
}

/* Change the current background color. */
// Изменить текущий цвет фона.
void gfx_clear_color(int r, int g, int b)
{
    XColor color;
    color.pixel = 0;
    color.red = r << 8;
    color.green = g << 8;
    color.blue = b << 8;
    XAllocColor(gfx_display, gfx_colormap, &color);
    XSetWindowAttributes attr;
    attr.background_pixel = color.pixel;
    XChangeWindowAttributes(gfx_display, gfx_window, CWBackPixel, &attr);
}

/* Check to see if an event is waiting. */
int gfx_event_waiting() // Ожидание любого события
{
    XEvent event;
    gfx_flush();
    while (1)
    {
        if (XCheckMaskEvent(gfx_display, -1, &event))
        {
            if (event.type == KeyPress)
            {
                XPutBackEvent(gfx_display, &event);
                return 1;
            }
            else if (event.type == ButtonPress)
            {
                XPutBackEvent(gfx_display, &event);
                return 1;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            return 0;
        }
    }
}

/* Wait for the user to press a key or mouse button. */
// Подождите, пока пользователь нажмет клавишу или кнопку мыши.
char gfx_wait()
{
    XEvent event;
    gfx_flush();
    while (1)
    {
        XNextEvent(gfx_display, &event);
        if (event.type == KeyPress)
        {
            saved_xpos = event.xkey.x;
            saved_ypos = event.xkey.y;
            return XLookupKeysym(&event.xkey, 0);
        }
        else if (event.type == ButtonPress)
        {
            saved_xpos = event.xkey.x;
            saved_ypos = event.xkey.y;
            return event.xbutton.button;
        }
    }
}

/* Return the X and Y coordinates of the last event. */
// Вернуть координаты X и Y последнего события.
int gfx_xpos()
{
    return saved_xpos;
}

int gfx_ypos()
{
    return saved_ypos;
}

/* Flush all previous output to the window. */
// Сбросить весь предыдущий вывод.
void gfx_flush()
{
    XFlush(gfx_display);
}

/* XGetPixel() function returns the specified pixel from the named image. */
int GetPix(int x, int y)
{
    // http://www.asvcorp.ru/tech/linux/xwinprg/functions/XGetImage.html
    XColor color;
    XImage *image;
    /*  Для всего дисплея.
        image = XGetImage (gfx_display, RootWindow (gfx_display, DefaultScreen (gfx_display)), x, y, 1, 1, AllPlanes, XYPixmap);   */
    image = XGetImage(gfx_display, gfx_window, x, y, 1, 1, AllPlanes, XYPixmap); // Только для нашего окна.
    color.pixel = XGetPixel(image, 0, 0);										 // XGetPixel - получить значение цвета точки из локального графического образа
    XFree(image); // XFree - освобождение памяти от объектов данных
    //XQueryColor(gfx_display, DefaultColormap(gfx_display, DefaultScreen(gfx_display)), &color); // XQueryColor - получить значения RGB из ячейки палитры.
    unsigned long red_mask = image->red_mask;
    unsigned long green_mask = image->green_mask;
    unsigned long blue_mask = image->blue_mask;
    unsigned char blue = (color.pixel & blue_mask);
    unsigned char green = (color.pixel & green_mask) >> 8;
    unsigned char red = (color.pixel & red_mask) >> 16;
    //printf("Red = %3hhu Green = %3hhu Blue = %3hhu -- RGB int = %3lu  \n", red, green, blue, (red << 16) + (green << 8) + blue);
    return ((red << 16) + (green << 8) + blue);
}

/* Read keys */
int gfx_xreadkeys()
{
    XEvent event;
    XNextEvent(gfx_display, &event);
    KeySym sym = XLookupKeysym(&event.xkey, 0);
    if (event.type == KeyPress) // keyboard events
    {
        printf("KeyCode: %2x KeySym: %4lx - (%s) \n", event.xkey.keycode, sym, XKeysymToString(sym));
        return sym; //
    }
    else
    {
        return -1;
    }
}

/* With control of the number of events read key   */
int gfx_m_xreadkeys()
{
    // XPending () возвращает количество событий, которые были получены от X-сервера, но не были удалены из очереди событий.
    if (XPending(gfx_display) > 0)
    {
        XEvent event;
        XNextEvent(gfx_display, &event);
        KeySym sym = XLookupKeysym(&event.xkey, 0);
        if (event.type == KeyPress) // keyboard events
        {
            printf("KeyCode: %2x KeySym: %4lx - (%s) \n", event.xkey.keycode, sym, XKeysymToString(sym));
            return sym; //
        }				// KeyRelease - отжата  KeyPress - нажата
    }
    else
    {
        return -1;
    }
}

/* Moving window */
// Перемещение окна
int gfx_move_win_l(int x, int y, int distance, int delay, int step)
{
    /* moving the window to the left. */
    for (int i = 0; i < distance; i++)
    {
        x -= step;
        XMoveWindow(gfx_display, gfx_window, x, y);
        XFlush(gfx_display);
        usleep(delay);
    }
    return x;
}

int gfx_move_win_d(int x, int y, int distance, int delay, int step)
{
    /* moving the window to the down. */
    for (int i = 0; i < distance; i++)
    {
        y += step;
        XMoveWindow(gfx_display, gfx_window, x, y);
        XFlush(gfx_display);
        usleep(delay);
    }
    return y;
}

int gfx_move_win_r(int x, int y, int distance, int delay, int step)
{
    /* moving the window to the right. */
    for (int i = 0; i < distance; i++)
    {
        x += step;
        XMoveWindow(gfx_display, gfx_window, x, y);
        XFlush(gfx_display);
        usleep(delay);
    }
    return x;
}

int gfx_move_win_u(int x, int y, int distance, int delay, int step)
{
    /* moving the window to the up. */
    for (int i = 0; i < distance; i++)
    {
        y -= step;
        XMoveWindow(gfx_display, gfx_window, x, y);
        XFlush(gfx_display);
        usleep(delay);
    }
    return y;
}
