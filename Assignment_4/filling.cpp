#include <graphics.h>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <stack>
using namespace std;

int n, x[100], y[100];
int fillColor, boundaryColor;
int seedX, seedY;

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
            if (p < 0) { p += 2 * dy; }
            else { y += sy; p += 2 * dy - 2 * dx; }
            x += sx;
        }
    } else {
        int p = 2 * dx - dy;
        for (int k = 0; k <= dy; k++) {
            putpixel(x, y, getcolor());
            if (p < 0) { p += 2 * dx; }
            else { x += sx; p += 2 * dx - 2 * dy; }
            y += sy;
        }
    }
}

void drawPolygon()
{
    setcolor(boundaryColor);
    for (int i = 0; i < n; i++) {
        drawLine(x[i], y[i], x[(i + 1) % n], y[(i + 1) % n]);
    }
}

// ---------- even-odd rule: is (px, py) mathematically inside the polygon? ----------
bool isInsidePolygon(int px, int py)
{
    bool inside = false;
    for (int i = 0, j = n - 1; i < n; j = i++) {
        int xi = x[i], yi = y[i];
        int xj = x[j], yj = y[j];
        bool crosses = ((yi > py) != (yj > py));
        if (crosses) {
            double xIntersect = xi + (double)(py - yi) * (xj - xi) / (yj - yi);
            if (px < xIntersect)
                inside = !inside;
        }
    }
    return inside;
}

// ---------- one iterative boundary-fill spread from a single seed ----------
void boundaryFillFrom(int startX, int startY)
{
    stack<pair<int,int>> st;
    st.push({startX, startY});

    while (!st.empty()) {
        pair<int,int> p = st.top();
        st.pop();

        int px = p.first, py = p.second;

        if (px < 0 || py < 0 || px > getmaxx() || py > getmaxy())
            continue;

        int currentColor = getpixel(px, py);

        // stop at boundary or already-filled pixels
        if (currentColor == boundaryColor || currentColor == fillColor)
            continue;

        putpixel(px, py, fillColor);
        delay(1);

        st.push({px + 1, py});
        st.push({px - 1, py});
        st.push({px, py + 1});
        st.push({px, py - 1});
    }
}

// ---------- boundary fill that also catches disconnected sub-polygons ----------
void boundaryFillAllRegions(int seedX, int seedY)
{
    // Step 1: normal boundary fill from the given seed
    boundaryFillFrom(seedX, seedY);

    // Step 2: compute bounding box of the whole polygon
    int xMin = x[0], xMax = x[0];
    int yMin = y[0], yMax = y[0];
    for (int i = 0; i < n; i++) {
        xMin = min(xMin, x[i]);
        xMax = max(xMax, x[i]);
        yMin = min(yMin, y[i]);
        yMax = max(yMax, y[i]);
    }
    xMin = max(xMin, 0);
    yMin = max(yMin, 0);
    xMax = min(xMax, getmaxx());
    yMax = min(yMax, getmaxy());

    // Step 3: scan for any pixel the odd-even rule says is "inside"
    // but that boundary fill did NOT reach (a disconnected sub-polygon)
    for (int py = yMin; py <= yMax; py++) {
        for (int px = xMin; px <= xMax; px++) {
            if (isInsidePolygon(px, py)) {
                int c = getpixel(px, py);
                if (c != fillColor && c != boundaryColor) {
                    // found an unfilled interior pixel in a different sub-region
                    // use it as a NEW seed and spread from there too
                    boundaryFillFrom(px, py);
                }
            }
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
    cout << "Enter seed pixel coordinates (must be inside at least one sub-region): ";
    cin >> seedX >> seedY;

    if (!isInsidePolygon(seedX, seedY)) {
        cout << "Error: seed point (" << seedX << ", " << seedY
             << ") is OUTSIDE the polygon (or on its edge). Fill aborted.\n";
        return 1;
    }

    initgraph(&gd, &gm, (char*)"");
    cleardevice();

    drawPolygon();
    delay(500);

    boundaryFillAllRegions(seedX, seedY);

    getch();
    closegraph();
    return 0;
}
