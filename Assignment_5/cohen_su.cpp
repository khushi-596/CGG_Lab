#include<iostream>
#include<graphics.h>
#include<cmath>
#include<algorithm>
#include<limits>
using namespace std;

const int INSIDE = 0; //0000
const int LEFT   = 1; //0001
const int RIGHT  = 2; //0010
const int BOTTOM = 4; //0100
const int TOP    = 8; //1000

double xmin, ymin, xmax, ymax;

double readDouble(const string &prompt) {
    double value;
    cout << prompt;
    while (!(cin >> value)) {
        cin.clear();                                           
        cin.ignore(numeric_limits<streamsize>::max(), '\n');    
        cout << "Invalid number, try again: ";
    }
    return value;
}

void DDA(double x1, double y1, double x2, double y2, int color) {
    double dx = x2 - x1;
    double dy = y2 - y1;

    int steps = (int)round(max(fabs(dx), fabs(dy)));
    if (steps == 0) {
        putpixel((int)round(x1), (int)round(y1), color);
        return;
    }

    double xIncrement = dx / steps;
    double yIncrement = dy / steps;
    double x = x1, y = y1;
    for (int i = 0; i <= steps; i++) {
        putpixel((int)round(x), (int)round(y), color);
        x += xIncrement;
        y += yIncrement;
    }
}

int computeCode(double x, double y) {
    int code = INSIDE;
    if (x < xmin) code |= LEFT;
    else if (x > xmax) code |= RIGHT;
    if (y < ymin) code |= BOTTOM;
    else if (y > ymax) code |= TOP;
    return code;
}

// Cohen-Sutherland clipping.
bool cohenSutherland(double &x1, double &y1, double &x2, double &y2) {
    int code1 = computeCode(x1, y1);
    int code2 = computeCode(x2, y2);
    int guard = 0;

    while (true) {
        if (code1 == 0 && code2 == 0) {
            return true;                 // both points inside - accept
        } else if (code1 & code2) {
            return false;                // both points share an outside side - reject
        } else {
            if (++guard > 100) return false; 

            double x = 0, y = 0;
            int codeOut = code1 ? code1 : code2;

            if (codeOut & TOP) {
                x = x1 + (x2 - x1) * (ymax - y1) / (y2 - y1);
                y = ymax;
            } else if (codeOut & BOTTOM) {
                x = x1 + (x2 - x1) * (ymin - y1) / (y2 - y1);
                y = ymin;
            } else if (codeOut & RIGHT) {
                y = y1 + (y2 - y1) * (xmax - x1) / (x2 - x1);
                x = xmax;
            } else { // LEFT
                y = y1 + (y2 - y1) * (xmin - x1) / (x2 - x1);
                x = xmin;
            }

            if (codeOut == code1) {
                x1 = x; y1 = y;
                code1 = computeCode(x1, y1);
            } else {
                x2 = x; y2 = y;
                code2 = computeCode(x2, y2);
            }
        }
    }
}

int main() {
    cout << "Enter clipping window coordinates:\n";
    xmin = readDouble("Enter xmin: ");
    ymin = readDouble("Enter ymin: ");
    xmax = readDouble("Enter xmax: ");
    ymax = readDouble("Enter ymax: ");

    if (xmin > xmax) { swap(xmin, xmax); cout << "(xmin/xmax were reversed, so I swapped them)\n"; }
    if (ymin > ymax) { swap(ymin, ymax); cout << "(ymin/ymax were reversed, so I swapped them)\n"; }

    if (xmin == xmax || ymin == ymax) {
        cout << "clipping window has zero width or height; "
                "clipped results will collapse onto a line/point.\n";
    }

    cout << "\nEnter point (x1, y1): ";
    double x1 = readDouble("x1: ");
    double y1 = readDouble("y1: ");
    cout << "Enter point (x2, y2): ";
    double x2 = readDouble("x2: ");
    double y2 = readDouble("y2: ");

    int gd = DETECT, gm;
    char path[] = "";
    initgraph(&gd, &gm, path);

    if (graphresult() != grOk) {
        cout << "Graphics init failed: " << grapherrormsg(graphresult()) << "\n";
        return 1;
    }

    if (xmax > getmaxx() || ymax > getmaxy() || xmin < 0 || ymin < 0) {
        cout << "part of the clipping window falls outside the visible "
                "screen (" << getmaxx() << " x " << getmaxy() << ").\n";
    }

    DDA(xmin, ymin, xmax, ymin, WHITE);
    DDA(xmax, ymin, xmax, ymax, WHITE);
    DDA(xmax, ymax, xmin, ymax, WHITE);
    DDA(xmin, ymax, xmin, ymin, WHITE);

    DDA(x1, y1, x2, y2, RED);

    double cx1 = x1, cy1 = y1, cx2 = x2, cy2 = y2;
    bool accepted = cohenSutherland(cx1, cy1, cx2, cy2);

    if (accepted) {
        cout << "\nLine accepted after clipping.\n";
        cout << "Clipped line: (" << cx1 << ", " << cy1 << ") -> ("
             << cx2 << ", " << cy2 << ")\n";
        DDA(cx1, cy1, cx2, cy2, GREEN);
    } else {
        cout << "\nLine completely outside. Line rejected.\n";
    }

    getch();
    closegraph();
    return 0;
}
