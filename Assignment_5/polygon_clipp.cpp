#include <graphics.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <stdexcept>

using namespace std;

// Colors used for the polygon, fill, intersections, and clipping window.
const int BOUNDARY_COLOR = BLACK;
const int BACKGROUND_COLOR = WHITE;
const int FILL_COLOR = BLUE;
const int CLIP_COLOR = GREEN;
const int INTERSECTION_COLOR = RED;
const int CLIP_WINDOW_COLOR = LIGHTRED;

// Integer screen coordinate.
struct Point {
    int x;
    int y;
};

// Double-precision coordinate used during clipping calculations.
struct DPoint {
    double x;
    double y;
};

// Checks whether a point is inside the graphics screen.
bool insideScreen(int x, int y) {
    return x >= 0 && x < getmaxx() && y >= 0 && y < getmaxy();
}

// Draws a line using Bresenham's line algorithm.
void bresenhamLine(int x1, int y1, int x2, int y2, int color) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);

    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;

    int err = dx - dy;

    while (true) {
        if (insideScreen(x1, y1))
            putpixel(x1, y1, color);

        if (x1 == x2 && y1 == y2)
            break;

        int e2 = 2 * err;

        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }

        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

// Draws all edges of the polygon.
void drawPolygon(const vector<Point>& p, int color) {
    int n = p.size();

    for (int i = 0; i < n; i++) {
        int j = (i + 1) % n;

        bresenhamLine(p[i].x, p[i].y, p[j].x, p[j].y, color);
    }
}

// Determines the orientation of three points.
long long orientation(int ax, int ay, int bx, int by, int cx, int cy) {
    return (long long)(bx - ax) * (cy - ay) - (long long)(by - ay) * (cx - ax);
}

// Checks whether a point lies on a line segment.
bool onSegment(int ax, int ay, int bx, int by, int px, int py) {
    return px >= min(ax, bx) &&
           px <= max(ax, bx) &&
           py >= min(ay, by) &&
           py <= max(ay, by);
}

// Checks whether two line segments intersect.
bool segmentsIntersect(Point a, Point b, Point c, Point d) {
    long long o1 = orientation(a.x, a.y, b.x, b.y, c.x, c.y);

    long long o2 = orientation(a.x, a.y, b.x, b.y, d.x, d.y);

    long long o3 = orientation(c.x, c.y, d.x, d.y, a.x, a.y);

    long long o4 = orientation(c.x, c.y, d.x, d.y, b.x, b.y);

    if (((o1 > 0 && o2 < 0) ||
         (o1 < 0 && o2 > 0)) &&
        ((o3 > 0 && o4 < 0) ||
         (o3 < 0 && o4 > 0))) {
        return true;
    }

    if (o1 == 0 && onSegment(a.x, a.y, b.x, b.y, c.x, c.y))
        return true;

    if (o2 == 0 && onSegment(a.x, a.y, b.x, b.y, d.x, d.y))
        return true;

    if (o3 == 0 && onSegment(c.x, c.y, d.x, d.y, a.x, a.y))
        return true;

    if (o4 == 0 && onSegment(c.x, c.y, d.x, d.y, b.x, b.y))
        return true;

    return false;
}

// Calculates the intersection point of two line segments.
bool getIntersection(Point a, Point b, Point c, Point d, DPoint& result) {
    double x1 = a.x;
    double y1 = a.y;
    double x2 = b.x;
    double y2 = b.y;

    double x3 = c.x;
    double y3 = c.y;
    double x4 = d.x;
    double y4 = d.y;

    double denominator = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);

    if (fabs(denominator) < 1e-9)
        return false;

    result.x = ((x1 * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3 * y4 - y3 * x4)) / denominator;

    result.y =
        ((x1 * y2 - y1 * x2) * (y3 - y4) -
         (y1 - y2) * (x3 * y4 - y3 * x4))
        / denominator;

    return true;
}

// Finds non-adjacent polygon-edge intersections for self-intersection detection.
vector<DPoint> findAllIntersections(const vector<Point>& poly) {
    vector<DPoint> intersections;
    int n = poly.size();

    for (int i = 0; i < n; i++) {
        int i2 = (i + 1) % n;

        for (int j = i + 1; j < n; j++) {
            int j2 = (j + 1) % n;

            if (j == i + 1)
                continue;

            if (i == 0 && j == n - 1)
                continue;

            Point a = poly[i];
            Point b = poly[i2];
            Point c = poly[j];
            Point d = poly[j2];

            if (segmentsIntersect(a, b, c, d)) {
                DPoint intersection;

                if (getIntersection(a, b, c, d, intersection)) {
                    intersections.push_back(intersection);
                }
            }
        }
    }

    return intersections;
}

// Fills the polygon using the scan-line filling algorithm.
void scanLineFill(const vector<Point>& polygon, int fillColor) {
    int n = polygon.size();

    if (n < 3)
        return;

    int minY = getmaxy();
    int maxY = 0;

    for (int i = 0; i < n; i++) {
        minY = min(minY, polygon[i].y);
        maxY = max(maxY, polygon[i].y);
    }

    minY = max(0, minY);
    maxY = min(getmaxy() - 1, maxY);

    for (int y = minY; y <= maxY; y++) {
        vector<double> intersections;

        for (int i = 0; i < n; i++) {
            Point p1 = polygon[i];
            Point p2 = polygon[(i + 1) % n];

            if (p1.y == p2.y)
                continue;

            if (y >= min(p1.y, p2.y) && y < max(p1.y, p2.y)) {
                double x =
                    p1.x +
                    (double)(y - p1.y) *
                    (p2.x - p1.x) /
                    (double)(p2.y - p1.y);

                intersections.push_back(x);
            }
        }

        sort(intersections.begin(), intersections.end());

        for (size_t i = 0; i + 1 < intersections.size(); i += 2) {
            int xStart = (int)ceil(intersections[i]);
            int xEnd = (int)floor(intersections[i + 1]);

            xStart = max(0, xStart);
            xEnd = min(getmaxx() - 1, xEnd);

            for (int x = xStart; x <= xEnd; x++)
                putpixel(x, y, fillColor);
        }
    }
}

struct ClipWindow {
    double xmin;
    double ymin;
    double xmax;
    double ymax;
};

struct CircleWindow {
    double cx;
    double cy;
    double radius;
};

const int INSIDE = 0;
const int LEFT = 1;
const int RIGHT = 2;
const int BOTTOM = 4;
const int TOP = 8;

// Generates the Cohen-Sutherland region code.
int getCode(double x, double y, const ClipWindow& window) {
    int code = INSIDE;

    if (x < window.xmin)
        code |= LEFT;
    else if (x > window.xmax)
        code |= RIGHT;

    if (y < window.ymin)
        code |= BOTTOM;
    else if (y > window.ymax)
        code |= TOP;

    return code;
}

// Clips a line segment against a rectangular window using Cohen-Sutherland.
bool cohenSutherlandClip(DPoint p1, DPoint p2, const ClipWindow& window, DPoint& clipped1, DPoint& clipped2) {
    double x1 = p1.x;
    double y1 = p1.y;
    double x2 = p2.x;
    double y2 = p2.y;

    int code1 = getCode(x1, y1, window);
    int code2 = getCode(x2, y2, window);

    bool accept = false;

    while (true) {
        if ((code1 | code2) == 0) {
            accept = true;
            break;
        }

        if (code1 & code2)
            break;

        int codeOut =
            (code1 != 0) ? code1 : code2;

        double x = 0;
        double y = 0;

        if (codeOut & TOP) {
            if (fabs(y2 - y1) < 1e-9)
                break;

            x = x1 +
                (x2 - x1) *
                (window.ymax - y1) /
                (y2 - y1);

            y = window.ymax;
        }
        else if (codeOut & BOTTOM) {
            if (fabs(y2 - y1) < 1e-9)
                break;

            x = x1 +
                (x2 - x1) *
                (window.ymin - y1) /
                (y2 - y1);

            y = window.ymin;
        }
        else if (codeOut & RIGHT) {
            if (fabs(x2 - x1) < 1e-9)
                break;

            y = y1 +
                (y2 - y1) *
                (window.xmax - x1) /
                (x2 - x1);

            x = window.xmax;
        }
        else if (codeOut & LEFT) {
            if (fabs(x2 - x1) < 1e-9)
                break;

            y = y1 +
                (y2 - y1) *
                (window.xmin - x1) /
                (x2 - x1);

            x = window.xmin;
        }

        if (codeOut == code1) {
            x1 = x;
            y1 = y;
            code1 = getCode(x1, y1, window);
        }
        else {
            x2 = x;
            y2 = y;
            code2 = getCode(x2, y2, window);
        }
    }

    if (accept) {
        clipped1 = {x1, y1};
        clipped2 = {x2, y2};
    }
    return accept;
}

bool insideClipWindow(double x, double y, const ClipWindow& window) {
    return x >= window.xmin &&
           x <= window.xmax &&
           y >= window.ymin &&
           y <= window.ymax;
}

// Draws the rectangular clipping window.
void drawClipWindow(const ClipWindow& window, int color) {
    bresenhamLine(
        (int)round(window.xmin),
        (int)round(window.ymin),
        (int)round(window.xmax),
        (int)round(window.ymin),
        color);

    bresenhamLine(
        (int)round(window.xmax),
        (int)round(window.ymin),
        (int)round(window.xmax),
        (int)round(window.ymax),
        color);

    bresenhamLine(
        (int)round(window.xmax),
        (int)round(window.ymax),
        (int)round(window.xmin),
        (int)round(window.ymax),
        color);

    bresenhamLine(
        (int)round(window.xmin),
        (int)round(window.ymax),
        (int)round(window.xmin),
        (int)round(window.ymin),
        color);
}

// Finds polygon-edge intersections with the rectangular clipping window.
vector<DPoint> findClipIntersections(const vector<Point>& polygon, const ClipWindow& window) {
    vector<DPoint> intersections;
    int n = polygon.size();

    for (int i = 0; i < n; i++) {
        Point p1 = polygon[i];
        Point p2 = polygon[(i + 1) % n];

        DPoint a = {(double)p1.x, (double)p1.y};
        DPoint b = {(double)p2.x, (double)p2.y};

        DPoint clipped1;
        DPoint clipped2;

        if (!cohenSutherlandClip(
                a, b, window,
                clipped1, clipped2))
            continue;

        if (fabs(clipped1.x - a.x) > 1e-6 ||
            fabs(clipped1.y - a.y) > 1e-6) {
            intersections.push_back(clipped1);
        }

        if (fabs(clipped2.x - b.x) > 1e-6 ||
            fabs(clipped2.y - b.y) > 1e-6) {
            intersections.push_back(clipped2);
        }
    }
    return intersections;
}

// Draws intersection markers for the intermediate clipping view.
void drawIntersectionPoints(const vector<DPoint>& intersections) {
    for (const auto& p : intersections) {
        int x = (int)round(p.x);
        int y = (int)round(p.y);

        bresenhamLine(
            x - 4, y,
            x + 4, y,
            INTERSECTION_COLOR);

        bresenhamLine(
            x, y - 4,
            x, y + 4,
            INTERSECTION_COLOR);
    }
}

// Draws only polygon-edge portions inside the rectangular window.
void drawClippedPolygon(const vector<Point>& polygon, const ClipWindow& window) {
    int n = polygon.size();

    for (int i = 0; i < n; i++) {
        Point p1 = polygon[i];
        Point p2 = polygon[(i + 1) % n];

        DPoint a = {(double)p1.x, (double)p1.y};
        DPoint b = {(double)p2.x, (double)p2.y};

        DPoint clipped1;
        DPoint clipped2;

        if (cohenSutherlandClip(
                a, b, window,
                clipped1, clipped2)) {

            // Draw ONLY the portion of the polygon edge
            // that lies inside the rectangular clipping window.
            bresenhamLine(
                (int)round(clipped1.x),
                (int)round(clipped1.y),
                (int)round(clipped2.x),
                (int)round(clipped2.y),
                BOUNDARY_COLOR);
        }
    }
}

// Fills the intersection of the polygon and rectangular window.
void fillClippedArea(const vector<Point>& polygon, const ClipWindow& window) {
    int minY = max(0, (int)ceil(window.ymin));

    int maxY = min(getmaxy() - 1, (int)floor(window.ymax));

    int minX = max(0, (int)ceil(window.xmin));

    int maxX = min(getmaxx() - 1, (int)floor(window.xmax));

    for (int y = minY; y <= maxY; y++) {
        vector<double> intersections;

        for (int i = 0; i < (int)polygon.size(); i++) {
            Point p1 = polygon[i];
            Point p2 = polygon[(i + 1) % polygon.size()];

            if (p1.y == p2.y)
                continue;

            if (y >= min(p1.y, p2.y) && y < max(p1.y, p2.y)) {
                double x =
                    p1.x +
                    (double)(y - p1.y) *
                    (p2.x - p1.x) /
                    (double)(p2.y - p1.y);

                intersections.push_back(x);
            }
        }

        sort(intersections.begin(), intersections.end());

        for (size_t i = 0; i + 1 < intersections.size(); i += 2) {
            int x1 = max(minX, (int)ceil(intersections[i]));

            int x2 = min(maxX, (int)floor(intersections[i + 1]));

            if (x1 <= x2) {
                for (int x = x1; x <= x2; x++) {
                    putpixel(x, y, CLIP_COLOR);
                }
            }
        }
    }
}

// Checks whether a point lies inside the circular window.
bool insideCircle(double x, double y, const CircleWindow& window) {
    double dx = x - window.cx;
    double dy = y - window.cy;

    return dx * dx + dy * dy <=
           window.radius * window.radius;
}

// Draws the circular clipping window.
void drawCircleWindow(const CircleWindow& window, int color) {
    setcolor(color);

    circle(
        (int)round(window.cx),
        (int)round(window.cy),
        (int)round(window.radius));
}

// Clips a polygon edge using edge-circle intersection.
bool circularLineClip(DPoint p1, DPoint p2, const CircleWindow& window, DPoint& clipped1, DPoint& clipped2) {
    double dx = p2.x - p1.x;
    double dy = p2.y - p1.y;

    double fx = p1.x - window.cx;
    double fy = p1.y - window.cy;

    // Quadratic coefficient for the line-circle intersection equation.
    double A = dx * dx + dy * dy;

    if (fabs(A) < 1e-9) {
        if (insideCircle(p1.x, p1.y, window)) {
            clipped1 = p1;
            clipped2 = p2;
            return true;
        }

        return false;
    }

    // Linear coefficient for the line-circle intersection equation.
    double B = 2.0 * (fx * dx + fy * dy);

    double C =
        fx * fx +
        fy * fy -
        window.radius * window.radius;

    // Discriminant determines whether the edge intersects the circle.
    double D = B * B - 4.0 * A * C;

    bool inside1 =
        insideCircle(p1.x, p1.y, window);

    bool inside2 =
        insideCircle(p2.x, p2.y, window);

    if (inside1 && inside2) {
        clipped1 = p1;
        clipped2 = p2;
        return true;
    }

    if (D < 0)
        return false;

    double sqrtD = sqrt(max(0.0, D));

    // First intersection parameter along the polygon edge.
    double t1 =
        (-B - sqrtD) / (2.0 * A);

    // Second intersection parameter along the polygon edge.
    double t2 =
        (-B + sqrtD) / (2.0 * A);

    if (t1 > t2)
        swap(t1, t2);

    if (t2 < 0.0 || t1 > 1.0)
        return false;

    double start =
        inside1 ? 0.0 : t1;

    double end =
        inside2 ? 1.0 : t2;

    start = max(0.0, start);
    end = min(1.0, end);

    if (start > end)
        return false;

    clipped1.x = p1.x + start * dx;
    clipped1.y = p1.y + start * dy;

    clipped2.x = p1.x + end * dx;
    clipped2.y = p1.y + end * dy;

    return true;
}

// Finds intersections between polygon edges and the circle.
vector<DPoint> findCircleIntersections(const vector<Point>& polygon, const CircleWindow& window) {
    vector<DPoint> intersections;
    int n = polygon.size();

    for (int i = 0; i < n; i++) {
        Point p1 = polygon[i];
        Point p2 = polygon[(i + 1) % n];

        double x1 = p1.x;
        double y1 = p1.y;
        double x2 = p2.x;
        double y2 = p2.y;

        double dx = x2 - x1;
        double dy = y2 - y1;

        double fx = x1 - window.cx;
        double fy = y1 - window.cy;

        double A = dx * dx + dy * dy;

        if (fabs(A) < 1e-9)
            continue;

        double B =
            2.0 * (fx * dx + fy * dy);

        double C =
            fx * fx +
            fy * fy -
            window.radius * window.radius;

        double D = B * B - 4.0 * A * C;

        if (D < -1e-9)
            continue;

        double sqrtD =
            sqrt(max(0.0, D));

        double t1 =
            (-B - sqrtD) / (2.0 * A);

        double t2 =
            (-B + sqrtD) / (2.0 * A);

        if (t1 >= 0.0 && t1 <= 1.0) {
            intersections.push_back({
                x1 + t1 * dx,
                y1 + t1 * dy
            });
        }

        if (t2 >= 0.0 && t2 <= 1.0 &&
            fabs(t2 - t1) > 1e-6) {
            intersections.push_back({
                x1 + t2 * dx,
                y1 + t2 * dy
            });
        }
    }

    return intersections;
}

// Draws polygon-edge portions that lie inside the circle.
void drawCircularClippedPolygon(const vector<Point>& polygon, const CircleWindow& window) {
    int n = polygon.size();

    for (int i = 0; i < n; i++) {
        Point p1 = polygon[i];
        Point p2 = polygon[(i + 1) % n];

        DPoint a = {(double)p1.x, (double)p1.y};
        DPoint b = {(double)p2.x, (double)p2.y};

        DPoint clipped1;
        DPoint clipped2;

        if (circularLineClip(
                a, b, window,
                clipped1, clipped2)) {
            bresenhamLine(
                (int)round(clipped1.x),
                (int)round(clipped1.y),
                (int)round(clipped2.x),
                (int)round(clipped2.y),
                CLIP_COLOR);
        }
    }
}

// Fills the intersection of the polygon and circular window.
void fillCircularClippedArea(const vector<Point>& polygon, const CircleWindow& circleWindow) {
    double cx = circleWindow.cx;
    double cy = circleWindow.cy;
    double radius = circleWindow.radius;
    if (polygon.size() < 3)
        return;

    int minY = max(0, (int)ceil(cy - radius));
    int maxY = min(getmaxy() - 1, (int)floor(cy + radius));

    for (int y = minY; y <= maxY; y++) {
        // Find circle's X range at this Y
        double dy = y - cy;
        double value = radius * radius - dy * dy;

        if (value < 0)
            continue;

        double dx = sqrt(value);

        double circleLeft  = cx - dx;
        double circleRight = cx + dx;

        // Find polygon intersections with scan line
        vector<double> intersections;

        for (int i = 0; i < (int)polygon.size(); i++) {
            Point p1 = polygon[i];
            Point p2 = polygon[(i + 1) % polygon.size()];

            // Ignore horizontal edges
            if (p1.y == p2.y)
                continue;

            // Half-open rule prevents duplicate vertices
            if (y >= min(p1.y, p2.y) && y < max(p1.y, p2.y)) {
                double x =
                    p1.x +
                    (double)(y - p1.y) *
                    (p2.x - p1.x) /
                    (double)(p2.y - p1.y);

                intersections.push_back(x);
            }
        }

        sort(intersections.begin(), intersections.end());

        // Fill polygon intervals
        for (int i = 0; i + 1 < (int)intersections.size(); i += 2) {
            double polyLeft  = intersections[i];
            double polyRight = intersections[i + 1];

            // Intersection of polygon span and circle span
            double left  = max(polyLeft, circleLeft);
            double right = min(polyRight, circleRight);

            int x1 = max(0, (int)ceil(left));
            int x2 = min(getmaxx() - 1, (int)floor(right));

            if (x1 <= x2) {
                for (int x = x1; x <= x2; x++)
                    putpixel(x, y, CLIP_COLOR);
            }
        }
    }
}

void drawVisibleCircleBoundary(const CircleWindow& window) {
    int cx = (int)round(window.cx);
    int cy = (int)round(window.cy);
    int r = (int)round(window.radius);

    for (int angle = 0; angle < 360; angle++) {
        double rad = angle * M_PI / 180.0;

        int x = (int)round(cx + r * cos(rad));

        int y = (int)round(cy + r * sin(rad));

        bool visible = false;

        if (insideScreen(x + 1, y) &&
            getpixel(x + 1, y) == CLIP_COLOR)
            visible = true;

        if (insideScreen(x - 1, y) &&
            getpixel(x - 1, y) == CLIP_COLOR)
            visible = true;

        if (insideScreen(x, y + 1) &&
            getpixel(x, y + 1) == CLIP_COLOR)
            visible = true;

        if (insideScreen(x, y - 1) &&
            getpixel(x, y - 1) == CLIP_COLOR)
            visible = true;

        if (visible)
            putpixel(x, y, CLIP_WINDOW_COLOR);
    }
}

// Reads and validates one polygon vertex.
Point getPolygonPoint(int pointNumber) {
    Point p;

    while (true) {
        cout << "Point "
             << pointNumber
             << " (X Y): ";

        cin >> p.x >> p.y;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');

            cout << "\nInvalid input.\n";
            continue;
        }

        if (!insideScreen(p.x, p.y)) {
            cout << "\nPoint is outside "
                 << "the graphics window.\n";
            continue;
        }

        return p;
    }
}

// Reads a valid integer input.
int getIntegerInput(string message) {
    int value;

    while (true) {
        cout << message;
        cin >> value;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');

            cout << "\nInvalid input.\n";
            continue;
        }

        return value;
    }
}

// Reads a valid decimal input.
double getDoubleInput(string message) {
    double value;

    while (true) {
        cout << message;
        cin >> value;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');

            cout << "\nInvalid input.\n";
            continue;
        }
        return value;
    }
}

int main() {
    bool graphInitialized = false;

    try {
        int gd = DETECT;
        int gm;

        char path[] = "";

        initgraph(&gd, &gm, path);
        graphInitialized = true;
        setbkcolor(BACKGROUND_COLOR);
        cleardevice();
        cout << "POLYGON FILL & CLIPPING\n";

        cout << "\n1. Convex Polygon";
        cout << "\n2. Concave Polygon";
        cout << "\n3. Self-Intersecting Polygon";

        int choice =
            getIntegerInput("\n\nEnter choice: ");

        if (choice < 1 || choice > 3)
            throw runtime_error(
                "Invalid polygon choice.");

        int n = getIntegerInput("\nEnter number of points: ");

        if (n < 3)
            throw runtime_error(
                "A polygon must have at least 3 points.");

        vector<Point> polygon(n);

        cout << "\nEnter polygon points:\n";

        for (int i = 0; i < n; i++) {
            polygon[i] = getPolygonPoint(i + 1);
        }

        cleardevice();

        drawPolygon(polygon, BOUNDARY_COLOR);

        vector<DPoint> intersections =
            findAllIntersections(polygon);

        if (!intersections.empty()) {
            cout << "\nSelf-intersection detected.";
            cout << "\nUsing Scan-Line Fill.";

            scanLineFill(
                polygon,
                FILL_COLOR);
        }
        else {
            cout << "\nPolygon accepted.";
            cout << "\nUsing Scan-Line Fill.";

            scanLineFill(
                polygon,
                FILL_COLOR);
        }

        drawPolygon(polygon, BOUNDARY_COLOR);

        cout << "\n\nPolygon filled.";
        cout << "\nPress Enter to continue...";

        cin.ignore(10000, '\n');
        cin.get();
        cout << "\nCLIPPING METHOD";

        cout << "\n\n1. Rectangular Clipping";
        cout << "\n2. Circular Clipping";

        int clippingChoice =
            getIntegerInput(
                "\n\nEnter choice: ");

        if (clippingChoice < 1 || clippingChoice > 2) {
            throw runtime_error(
                "Invalid clipping choice.");
        }

        if (clippingChoice == 1) {
            ClipWindow clipWindow;

            cout << "\n\nEnter rectangular clipping window:";

            clipWindow.xmin =
                getDoubleInput(
                    "\nMinimum X: ");

            clipWindow.ymin =
                getDoubleInput(
                    "Minimum Y: ");

            clipWindow.xmax =
                getDoubleInput(
                    "Maximum X: ");

            clipWindow.ymax =
                getDoubleInput(
                    "Maximum Y: ");

            if (clipWindow.xmin >= clipWindow.xmax ||
                clipWindow.ymin >= clipWindow.ymax) {
                throw runtime_error(
                    "Invalid clipping window.");
            }

            cleardevice();

            drawPolygon(
                polygon,
                BOUNDARY_COLOR);

            drawClipWindow(
                clipWindow,
                CLIP_WINDOW_COLOR);

            vector<DPoint> clipIntersections =
                findClipIntersections(
                    polygon,
                    clipWindow);

            drawIntersectionPoints(
                clipIntersections);

            cout << "\n\nIntersections: "
                 << clipIntersections.size();

            for (size_t i = 0; i < clipIntersections.size(); i++) {
                cout << "\n"
                     << i + 1
                     << ": ("
                     << clipIntersections[i].x
                     << ", "
                     << clipIntersections[i].y
                     << ")";
            }

            cout << "\n\nRED marks show intersections.";
            cout << "\nPress Enter to clip...";

            cin.ignore(10000, '\n');
            cin.get();

            cleardevice();

            fillClippedArea(
                polygon,
                clipWindow);

            drawClippedPolygon(
                polygon,
                clipWindow);

            drawClipWindow(
                clipWindow,
                CLIP_WINDOW_COLOR);

            cout << "\n\nCohen-Sutherland clipping completed.";
        }
        else {
            CircleWindow circleWindow;

            cout << "\n\nEnter circular clipping window:";

            circleWindow.cx =
                getDoubleInput(
                    "\nCentre X: ");

            circleWindow.cy =
                getDoubleInput(
                    "Centre Y: ");

            circleWindow.radius =
                getDoubleInput(
                    "Radius: ");

            if (circleWindow.radius <= 0)
                throw runtime_error(
                    "Radius must be greater than zero.");

            if (!insideScreen(
                    (int)round(circleWindow.cx),
                    (int)round(circleWindow.cy))) {
                throw runtime_error(
                    "Circle centre is outside "
                    "the graphics window.");
            }

            cleardevice();

            // Fill the intersection of the polygon and circle.
            fillCircularClippedArea(
                polygon,
                circleWindow);

            {
                int n = polygon.size();

                for (int i = 0; i < n; i++) {
                    Point p1 = polygon[i];
                    Point p2 = polygon[(i + 1) % n];

                    DPoint a = {(double)p1.x, (double)p1.y};
                    DPoint b = {(double)p2.x, (double)p2.y};

                    DPoint clipped1;
                    DPoint clipped2;

                    if (circularLineClip(
                            a, b, circleWindow,
                            clipped1, clipped2)) {

                        // Draw ONLY the polygon-edge portion inside
                        // the circular clipping window.
                        bresenhamLine(
                            (int)round(clipped1.x),
                            (int)round(clipped1.y),
                            (int)round(clipped2.x),
                            (int)round(clipped2.y),
                            BOUNDARY_COLOR);
                    }
                }
            }

            // Draw the circular clipping boundary.
            drawCircleWindow(
                circleWindow,
                CLIP_WINDOW_COLOR);

            vector<DPoint> circleIntersections =
                findCircleIntersections(
                    polygon,
                    circleWindow);

            cout << "\n\nPolygon-circle intersections: "
                 << circleIntersections.size();

            for (size_t i = 0; i < circleIntersections.size(); i++) {
                cout << "\n"
                     << i + 1
                     << ": ("
                     << circleIntersections[i].x
                     << ", "
                     << circleIntersections[i].y
                     << ")";
            }

            cout << "\n\nCircular clipping completed.";
        }

        cout << "\n\nPress Enter to exit...";

        cin.ignore(10000, '\n');
        cin.get();

        closegraph();

        return 0;
    }
    catch (const exception& e) {
        cout << "\n\nERROR: "
             << e.what()
             << endl;

        cout << "\nPress Enter to exit...";

        cin.ignore(10000, '\n');
        cin.get();

        if (graphInitialized)
            closegraph();

        return 1;
    }
}

