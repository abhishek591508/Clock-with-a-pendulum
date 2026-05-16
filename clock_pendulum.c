/*
 * Computer Graphics Mini Project - Analog Clock with Pendulum
 * For Code::Blocks (WinBGIM): link with  libbgi  and GDI32 libs
 *
 * Concepts used:
 *   - Bresenham's line algorithm (hands, ticks, pendulum)
 *   - Midpoint circle algorithm (clock face)
 *   - 2D rotation (hand angles)
 */

#include <graphics.h>
#include <conio.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <windows.h>   /* GetTickCount for real wall-clock time */

#define PI          3.14159265
#define GRAVITY     9.81          /* m/s^2 */
#define PX_TO_M     0.01          /* 1 pixel = 1 cm (rod length in metres) */
#define PEND_AMP    25.0          /* max swing in degrees (small-angle) */

/* Simple pendulum: angle = A * cos(omega * t),  omega = sqrt(g/L) */
double pendulum_angle(int rod_len_px, double t_sec)
{
    double L = rod_len_px * PX_TO_M;
    double omega = sqrt(GRAVITY / L);
    return PEND_AMP * cos(omega * t_sec);
}

/* ---------- Bresenham Line ---------- */
void bresenham_line(int x0, int y0, int x1, int y1, int col)
{
    int dx = abs(x1 - x0);
    int sx = (x0 < x1) ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx + dy;
    int e2;

    setcolor(col);
    while (1) {
        putpixel(x0, y0, col);
        if (x0 == x1 && y0 == y1)
            break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

/* ---------- Midpoint Circle (outline) ---------- */
void plot_circle_points(int xc, int yc, int x, int y, int col)
{
    putpixel(xc + x, yc + y, col);
    putpixel(xc - x, yc + y, col);
    putpixel(xc + x, yc - y, col);
    putpixel(xc - x, yc - y, col);
    putpixel(xc + y, yc + x, col);
    putpixel(xc - y, yc + x, col);
    putpixel(xc + y, yc - x, col);
    putpixel(xc - y, yc - x, col);
}

void midpoint_circle(int xc, int yc, int r, int col)
{
    int x = 0, y = r, p = 1 - r;

    setcolor(col);
    while (x <= y) {
        plot_circle_points(xc, yc, x, y, col);
        x++;
        if (p < 0)
            p += 2 * x + 1;
        else {
            y--;
            p += 2 * (x - y) + 1;
        }
    }
}

/* Point on circle at angle (degrees, 0 = 12 o'clock, clockwise) */
void angle_to_point(int cx, int cy, int len, double deg, int *px, int *py)
{
    double rad = (deg - 90.0) * PI / 180.0;
    *px = cx + (int)(len * cos(rad));
    *py = cy + (int)(len * sin(rad));
}

void draw_hand(int cx, int cy, int len, double deg, int col, int thick)
{
    int x, y, i;
    angle_to_point(cx, cy, len, deg, &x, &y);
    setlinestyle(SOLID_LINE, thick, 0);
    bresenham_line(cx, cy, x, y, col);
    /* small thickness effect by drawing parallel offset (simple) */
    for (i = 1; i < thick; i++) {
        bresenham_line(cx + i, cy, x + i, y, col);
        bresenham_line(cx, cy + i, x, y + i, col);
    }
}

void draw_ticks(int cx, int cy, int r)
{
    int i, x1, y1, x2, y2;
    int inner, outer;

    for (i = 0; i < 12; i++) {
        inner = r - 18;
        outer = r - 5;
        angle_to_point(cx, cy, inner, i * 30.0, &x1, &y1);
        angle_to_point(cx, cy, outer, i * 30.0, &x2, &y2);
        bresenham_line(x1, y1, x2, y2, WHITE);
    }
}

void draw_numbers(int cx, int cy, int r)
{
  char num[3];
  int x, y, i;
  for (i = 1; i <= 12; i++) {
    angle_to_point(cx, cy, r - 35, i * 30.0, &x, &y);
    sprintf(num, "%d", i);
    settextjustify(CENTER_TEXT, CENTER_TEXT);
    setcolor(YELLOW);
    outtextxy(x - 4, y - 4, num);
  }
}

void draw_pendulum(int pivot_x, int pivot_y, int rod_len, double angle_deg)
{
    int bob_x, bob_y;
    double rad = angle_deg * PI / 180.0;

    bob_x = pivot_x + (int)(rod_len * sin(rad));
    bob_y = pivot_y + (int)(rod_len * cos(rad));

    setlinestyle(SOLID_LINE, 2, 0);
    bresenham_line(pivot_x, pivot_y, bob_x, bob_y, LIGHTGRAY);
    setfillstyle(SOLID_FILL, RED);
    fillellipse(bob_x - 8, bob_y - 8, 16, 16);
    setcolor(DARKGRAY);
    circle(pivot_x, pivot_y, 4);
}

int main(void)
{
    int gd = DETECT, gm;
    int cx, cy, radius = 120;
    int pivot_x, pivot_y, pend_len = 90;
    double pend_angle, t_sec;
    DWORD pend_start = 0;
    time_t t;
    struct tm *now;
    int h, m, s;
    float hour_ang, min_ang, sec_ang;

    initgraph(&gd, &gm, "");

    cx = getmaxx() / 2;
    cy = getmaxy() / 2 - 40;
    pivot_x = cx;
    pivot_y = cy + radius + 25;

    setbkcolor(BLACK);
    cleardevice();

    while (!kbhit()) {
        t = time(NULL);
        now = localtime(&t);
        h = now->tm_hour % 12;
        m = now->tm_min;
        s = now->tm_sec;

        hour_ang = (h * 30.0) + (m * 0.5);
        min_ang  = (m * 6.0) + (s * 0.1);
        sec_ang  = s * 6.0;

        /* real wall-clock time so speed stays correct (not tied to frame count) */
        if (pend_start == 0)
            pend_start = GetTickCount();
        t_sec = (GetTickCount() - pend_start) / 1000.0;
        pend_angle = pendulum_angle(pend_len, t_sec);

        cleardevice();

        /* clock body */
        midpoint_circle(cx, cy, radius, WHITE);
        setfillstyle(SOLID_FILL, DARKGRAY);
        floodfill(cx, cy, WHITE);

        draw_ticks(cx, cy, radius);
        draw_numbers(cx, cy, radius);

        /* hands (back to front) */
        draw_hand(cx, cy, radius - 55, hour_ang, CYAN, 3);
        draw_hand(cx, cy, radius - 30, min_ang, GREEN, 2);
        draw_hand(cx, cy, radius - 15, sec_ang, RED, 1);

        /* center pin */
        setfillstyle(SOLID_FILL, WHITE);
        fillellipse(cx - 4, cy - 4, 8, 8);

        draw_pendulum(pivot_x, pivot_y, pend_len, pend_angle);

        setcolor(WHITE);
        settextjustify(CENTER_TEXT, TOP_TEXT);
        outtextxy(cx, 10, "Analog Clock with Pendulum - CG Project");
        outtextxy(cx, getmaxy() - 25, "Press any key to exit");

        delay(50);
    }

    closegraph();
    return 0;
}
