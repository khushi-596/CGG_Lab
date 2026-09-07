#include <graphics.h>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>
using namespace std;

int n, x[100], y[100];
int fillColor, boundaryColor;

// ---------- Bresenham's line drawing algorithm ----------
void drawLine(int x1, int y1, int x2, int y2)
{
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x2 > x1) ? 1 : -1;
    int sy = (y2 > y1) ? 1 : -1;
    int x = x1, y = y1;
    if (dx >= dy) {
        int p = 2 * dy - dx;
        for (int k = 0; k <= dx; k++) {
            putpixel(x, y, getcolor());
            if (p < 0) {
                p += 2 * dy;
            } else {
                y += sy;
                p += 2 * dy - 2 * dx;
            }
            x += sx;
        }
    } else {
        int p = 2 * dx - dy;
        for (int k = 0; k <= dy; k++) {
            putpixel(x, y, getcolor());
            if (p < 0) {
                p += 2 * dx;
            } else {
                x += sx;
                p += 2 * dx - 2 * dy;
            }
            y += sy;
        }
    }
}

// ---------- draw polygon outline (this is our fill boundary) ----------
void drawPolygon()
{
    setcolor(boundaryColor);
    for (int i = 0; i < n; i++) {
        drawLine(x[i], y[i], x[(i + 1) % n], y[(i + 1) % n]);
    }
}


void fillPolygonScanline()
{
    int xMin = x[0], xMax = x[0];
    int yMin = y[0], yMax = y[0];
    for (int i = 0; i < n; i++) {
        xMin = min(xMin, x[i]);
        xMax = max(xMax, x[i]);
        yMin = min(yMin, y[i]);
        yMax = max(yMax, y[i]);
    }
    xMin = max(xMin, 0);
    xMax = min(xMax, getmaxx());
    yMin = max(yMin, 0);
    yMax = min(yMax, getmaxy());

    setcolor(fillColor);
    for (int py = yMin; py <= yMax; py++) {
        vector<double> xIntersections;
        for (int i = 0, j = n - 1; i < n; j = i++) {
            int yi = y[i], yj = y[j];
            if (yi == yj)
                continue;
            int yLow = min(yi, yj);
            int yHigh = max(yi, yj);
            if (py >= yLow && py < yHigh) {
                double t = (double)(py - yi) / (double)(yj - yi);
                double xIntersect = x[i] + t * (x[j] - x[i]);
                xIntersections.push_back(xIntersect);
            }
        }
        sort(xIntersections.begin(), xIntersections.end());
        for (size_t k = 0; k + 1 < xIntersections.size(); k += 2) {
            int xStart = (int)round(xIntersections[k]);
            int xEnd = (int)round(xIntersections[k + 1]);

            // clamp to screen bounds
            xStart = max(xStart, xMin);
            xEnd = min(xEnd, xMax);

            for (int px = xStart; px <= xEnd; px++)
                putpixel(px, py, fillColor);
        }
    }
}

int main()
{
    int gd = DETECT, gm;

    cout << "Enter no. of vertices in polygon: ";
    cin >> n;

    cout << "Enter coordinates x, y for each vertex:\n";
    for (int i = 0; i < n; i++) {
        cout << "Vertex " << i + 1 << ": ";
        cin >> x[i] >> y[i];
    }

    cout << "Enter fill color (0-15): ";
    cin >> fillColor;

    cout << "Enter boundary color (0-15): ";
    cin >> boundaryColor;

    initgraph(&gd, &gm, (char*)"");
    cleardevice();

    drawPolygon(); 
    delay(500);    

    fillPolygonScanline(); 

    drawPolygon();

    getch();
    closegraph();
    return 0;
}
