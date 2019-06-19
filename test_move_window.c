#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "gfx.h"
// cc test_move_window.c gfx.c -o test_move_window -I/usr/X11R6/include -L/usr/X11R6/lib -lX11 -lm

#define ysize 600
#define xsize 600
#define Xsize xsize / 2        // условный центр
#define Ysize ysize / 2        // 300
#define A ysize / 60           // 10
#define B ysize - (ysize / 60) // 590
#define D ysize / 60           // 10
#define C ysize - (ysize / 60) // 590
#define N xsize / 10           // 60

void info(void)
{
#define couleur(param) printf("\033[%sm", param)
    couleur("32");
    printf("Ivan Svarkovsky (c) 2019 \n");
    couleur("0");
}

void cell_division(int n) // деление на клетки
{
    for (int i = 0; i <= C; i = (i + n)) //
    {
        gfx_line(A + i, D, A + i, C); // вертикальная левая
        gfx_line(A, D + i, B, D + i); // горизонтальная верхняя
    }
}

int main()
{
    info();
    gfx_open(xsize, ysize, "Graphics Program");

    gfx_color(30, 30, 30); // Grey   Red =  30 Green =  30 Blue =  30 -- RGB int = 1973790
    cell_division(A);

    // -- //
    int Delay = 30000; // ms
    int step = 1;
    int x = Xsize / 3;
    int y = Ysize / 3;
    // A - distance
    y = gfx_move_win_d(x, y, A * 2, Delay, step); // y
    x = gfx_move_win_l(x, y, A * 2, Delay, step); // x
    y = gfx_move_win_u(x, y, A * 2, Delay, step); // y
    x = gfx_move_win_r(x, y, A * 2, Delay, step); // x
    // -- //
    
    gfx_color(255, 0, 0);                               // Red
    gfx_rectangle(Xsize, Ysize, 200, 200);

    gfx_color(0, 0, 255); // Blue
    gfx_rectangle(100, 100, 100, 100);

    gfx_color(102, 255, 102);
    gfx_rectangle(0, 0, 100, 100);

    gfx_wait();
    return 0;
}
