#include <iostream>
#include <graphics.h>
#include <cmath>

using namespace std;

int main()
{
    int gd = DETECT, gm;
    initgraph(&gd, &gm, "");

    float x1, y1, x2, y2;
    float dx, dy, x, y;
    int length;

    cout << "Enter x1 y1 x2 y2: ";
    cin >> x1 >> y1 >> x2 >> y2;

    dx = x2 - x1;
    dy = y2 - y1;

    if (abs(dx) > abs(dy))
        length = abs(dx);
    else
        length = abs(dy);

    dx = dx / length;
    dy = dy / length;

    x = x1;
    y = y1;

    for (int i = 0; i <= length; i++)
    {
        putpixel(round(x), round(y), WHITE);
        x += dx;
        y += dy;
    }

    getch();
    closegraph();

    return 0;
}
