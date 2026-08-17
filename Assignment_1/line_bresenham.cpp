#include <graphics.h>
#include <iostream>
#include <cmath>

using namespace std;

int main()
{
    int gd = DETECT, gm;
    char path[] = "";
    initgraph(&gd, &gm, path);

    int x1, y1, x2, y2;

    cout << "Enter x1 y1 x2 y2: ";
    cin >> x1 >> y1 >> x2 >> y2;

    int dx = x2 - x1;
    int dy = y2 - y1;

    int p = 2 * dy - dx;

    int x = x1;
    int y = y1;

    while (x <= x2)
    {
        putpixel(x, y, WHITE);

        if (p < 0)
        {
            p = p + 2 * dy;
        }
        else
        {
            y++;
            p = p + 2 * dy - 2 * dx;
        }

        x++;
    }

    getch();
    closegraph();

    return 0;
}
