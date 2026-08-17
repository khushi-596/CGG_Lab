#include<iostream>
#include<graphics.h>
using namespace std;

void Bresenham(int xc, int yc, int R)
{
    int x = 0;
    int y = R;
    int delta = 2 * (1 - R);
    int p;

    while (y >= x)
    {
        // Plot all 8 symmetric points
        putpixel(xc + x, yc + y, WHITE);
        putpixel(xc - x, yc + y, WHITE);
        putpixel(xc + x, yc - y, WHITE);
        putpixel(xc - x, yc - y, WHITE);

        putpixel(xc + y, yc + x, WHITE);
        putpixel(xc - y, yc + x, WHITE);
        putpixel(xc + y, yc - x, WHITE);
        putpixel(xc - y, yc - x, WHITE);

        if (delta < 0)
        {
            p = 2 * delta + 2 * y - 1;

            if (p <= 0)
            {
                x = x + 1;
                delta = delta + 2 * x + 1;
            }
            else
            {
                x = x + 1;
                y = y - 1;
                delta = delta + 2 * x - 2 * y + 2;
            }
        }
        else if (delta > 0)
        {
            p = 2 * delta - 2 * x - 1;

            if (p <= 0)
            {
                x = x + 1;
                y = y - 1;
                delta = delta + 2 * x - 2 * y + 2;
            }
            else
            {
                y = y - 1;
                delta = delta - 2 * y + 1;
            }
        }
        else
        {
            x = x + 1;
            y = y - 1;
            delta = delta + 2 * x - 2 * y + 2;
        }
    }
}

int main()
{
    int gd = DETECT, gm;
    initgraph(&gd, &gm, (char*)"");

    int xc, yc, r;

    cout << "Enter centre (x y): ";
    cin >> xc >> yc;

    cout << "Enter radius: ";
    cin >> r;

    Bresenham(xc, yc, r);

    getch();
    closegraph();
    return 0;
}
