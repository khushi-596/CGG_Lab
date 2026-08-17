#include <iostream>
#include <cmath>
#include <algorithm>
#include <SDL2/SDL.h>
#include <graphics.h>
using namespace std;

void drawline_dda(float x1, float y1, float x2, float y2, int colour = RED) {
    float delta_x, delta_y, length;
    float dx, dy;
    delta_x = x2 - x1;
    delta_y = y2 - y1;
    length = max(abs(delta_x), abs(delta_y));
    if (length == 0) {
        putpixel((int)x1, (int)y1, colour);
        return;
    }
    dx = delta_x / length;
    dy = delta_y / length;
    int i = 0;
    while (i <= length) {
        putpixel((int)x1, (int)y1, colour);
        x1 = x1 + dx;
        y1 = y1 + dy;
        i++;
    }
}

void drawObject(double **A, int n, int colour) {
    for (int i = 0; i < n; i++) {
        int next = (i + 1) % n;
        drawline_dda(A[i][0], A[i][1], A[next][0], A[next][1], colour);
    }
}

void printMatrix(double **A, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < 3; j++) {
            cout << A[i][j] << " ";
        }
        cout << endl;
    }
}

void copyMatrix(double **source, double **destination, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < 3; j++) {
            destination[i][j] = source[i][j];
        }
    }
}

void translate(double **A, int n, int tx, int ty, double **R) {
    double T[3][3] = {
        {1, 0, 0},
        {0, 1, 0},
        {(double)tx, (double)ty, 1}
    };
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < 3; j++) {
            R[i][j] = 0;
            for (int k = 0; k < 3; k++) {
                R[i][j] = R[i][j] + A[i][k] * T[k][j];
            }
        }
    }
    cout << "\nAfter Translation:\n";
    printMatrix(R, n);
}

void scaling(double **A, int n, int sx, int sy, double **R) {
    double S[3][3] = {
        {(double)sx, 0, 0},
        {0, (double)sy, 0},
        {0, 0, 1}
    };
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < 3; j++) {
            R[i][j] = 0;
            for (int k = 0; k < 3; k++) {
                R[i][j] = R[i][j] + A[i][k] * S[k][j];
            }
        }
    }
    cout << "\nAfter Scaling:\n";
    printMatrix(R, n);
}

void rotation(double **A, int n, double **R, double theta, int choice) {
    double rad = theta * 3.14159 / 180.0;
    double Y[3][3];
    if (choice == 1) {
        // Clockwise Rotation
        Y[0][0] = cos(rad);
        Y[0][1] = sin(rad);
        Y[0][2] = 0;

        Y[1][0] = -sin(rad);
        Y[1][1] = cos(rad);
        Y[1][2] = 0;

        Y[2][0] = 0;
        Y[2][1] = 0;
        Y[2][2] = 1;
    }
    else {
        // Anticlockwise Rotation
        Y[0][0] = cos(rad);
        Y[0][1] = -sin(rad);
        Y[0][2] = 0;

        Y[1][0] = sin(rad);
        Y[1][1] = cos(rad);
        Y[1][2] = 0;

        Y[2][0] = 0;
        Y[2][1] = 0;
        Y[2][2] = 1;
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < 3; j++) {
            R[i][j] = 0;
            for (int k = 0; k < 3; k++) {
                R[i][j] = R[i][j] + A[i][k] * Y[k][j];
            }
        }
    }
    cout << "\nAfter Rotation:\n";
    printMatrix(R, n);
}

void rotationAnimation(double **A, int n, double theta, int choice) {
    double **R = new double*[n];
    for (int i = 0; i < n; i++) {
        R[i] = new double[3];
    }
    double centerX = 0;
    double centerY = 0;
    for (int i = 0; i < n; i++) {
        centerX += A[i][0];
        centerY += A[i][1];
    }
    centerX = centerX / n;
    centerY = centerY / n;
    
    cleardevice();
    setcolor(BLACK);
    drawObject(A, n, BLACK);
    delay(1000);
    double rad = theta * 3.14159 / 180.0;
    for (int i = 0; i < n; i++) {
        double x = A[i][0] - centerX;
        double y = A[i][1] - centerY;
        if (choice == 1) {
            // Clockwise
            R[i][0] = x * cos(rad) + y * sin(rad);
            R[i][1] = -x * sin(rad) + y * cos(rad);
        }
        else {
            // Anticlockwise
            R[i][0] = x * cos(rad) - y * sin(rad);
            R[i][1] = x * sin(rad) + y * cos(rad);
        }
        R[i][0] = R[i][0] + centerX;
        R[i][1] = R[i][1] + centerY;
        R[i][2] = 1;
    }
    setcolor(RED);
    drawObject(R, n, RED);
    for (int i = 0; i < n; i++) {
        delete[] R[i];
    }
    delete[] R;
}

void waitForClose() {
    SDL_Event event;
    bool running = true;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }
        SDL_Delay(10);
    }
}

int main() {
    int n;
    cout << "Enter number of points: ";
    cin >> n;
    double **A = new double*[n];
    for (int i = 0; i < n; i++) {
        A[i] = new double[3];
    }

    cout << "\nEnter coordinates of each point:\n";
    for (int i = 0; i < n; i++) {
        cout << "Point " << i + 1 << " (x y): ";
        cin >> A[i][0] >> A[i][1];
        A[i][2] = 1;
    }

    double **R = new double*[n];
    double **S = new double*[n];
    double **T = new double*[n];
    double **current = new double*[n];
    for (int i = 0; i < n; i++) {
        R[i] = new double[3];
        S[i] = new double[3];
        T[i] = new double[3];
        current[i] = new double[3];
    }

    int gd = DETECT;
    int gm;
    initgraph(&gd, &gm, NULL);
    setbkcolor(WHITE);
    cleardevice();
    setcolor(BLACK);
    drawObject(A, n, BLACK);

    int choice;
    cout << "\n\n--- 2D TRANSFORMATION ---";
    cout << "\n1. Translation";
    cout << "\n2. Scaling";
    cout << "\n3. Rotation";
    cout << "\n4. Composite Transformation";
    cout << "\n5. Exit";
    cout << "\nEnter your choice: ";
    cin >> choice;

    if (choice == 1) {
        int tx, ty;
        cout << "\nEnter Translation tx and ty: ";
        cin >> tx >> ty;
        translate(A, n, tx, ty, R);
        drawObject(R, n, RED);
    }

    else if (choice == 2) {
        int sx, sy;
        cout << "\nEnter Scaling Factors sx and sy: ";
        cin >> sx >> sy;
        scaling(A, n, sx, sy, R);
        drawObject(R, n, RED);
    }

    else if(choice == 3) {
        int theta;
        int rotation_choice;
        cout << "\nEnter theta: ";
        cin >> theta;
        cout << "\nEnter 1 for Clockwise";
        cout << "\nEnter 2 for Anticlockwise";
        cout << "\nEnter choice: ";
        cin >> rotation_choice;
        if(rotation_choice != 1 && rotation_choice != 2) {
            cout << "\nInvalid rotation choice!";
        }
        else {
            rotationAnimation(A, n, theta, rotation_choice);
        }
    }

    else if (choice == 4) {
        int tx, ty;
        int sx, sy;
        int theta;
        int rotation_choice;

        cout << "\nEnter Translation tx and ty: ";
        cin >> tx >> ty;
        cout << "\nEnter Scaling Factors sx and sy: ";
        cin >> sx >> sy;
        cout << "\nEnter theta: ";
        cin >> theta;
        cout << "\nEnter 1 for Clockwise";
        cout << "\nEnter 2 for Anticlockwise";
        cout << "\nEnter choice: ";
        cin >> rotation_choice;

        int sequence[3];
        cout << "\n";
        cout << "\n   COMPOSITE TRANSFORMATION";

        cout << "\n\nTransformation Options:";
        cout << "\n1. Translation";
        cout << "\n2. Scaling";
        cout << "\n3. Rotation";

        cout << "\n\nEnter sequence: ";
        cin >> sequence[0] >> sequence[1] >> sequence[2];

        if (sequence[0] < 1 || sequence[0] > 3 ||
            sequence[1] < 1 || sequence[1] > 3 ||
            sequence[2] < 1 || sequence[2] > 3 ||
            sequence[0] == sequence[1] ||
            sequence[1] == sequence[2] ||
            sequence[0] == sequence[2]) {
            cout << "\nInvalid transformation sequence!";
            cout << "\nPlease enter 1, 2 and 3 exactly once.";
            getch();
            closegraph();
            return 0;
        }

        copyMatrix(A, current, n);
        cout << "\n\nTransformation Process";
        for (int i = 0; i < 3; i++) {
            if (sequence[i] == 1) {
                cout << "\n\nApplying Translation...";
                translate(current, n, tx, ty, R);
                copyMatrix(R, current, n);
            }
            else if (sequence[i] == 2) {
                cout << "\n\nApplying Scaling...";
                scaling(current, n, sx, sy, S);
                copyMatrix(S, current, n);
            }
            else if (sequence[i] == 3) {
                cout << "\n\nApplying Rotation...";
                rotation(current, n, T, theta, rotation_choice);
                copyMatrix(T, current, n);
            }
        }

        cout << "\nFinal Composite Transformation";
        cout << "\n\nFinal Coordinates:\n";
        printMatrix(current, n);

        // Draw directly using the transformed coordinates (no fixed-point shift)
        setcolor(RED);
        drawObject(current, n, RED);
    }

    else if (choice == 5) {
        cout << "\nExiting...";
    }

    else {
        cout << "\nInvalid choice!";
    }
    waitForClose();
    closegraph();

    for (int i = 0; i < n; i++) {
        delete[] A[i];
        delete[] R[i];
        delete[] S[i];
        delete[] T[i];
        delete[] current[i];
    }
    delete[] A;
    delete[] R;
    delete[] S;
    delete[] T;
    delete[] current;
    return 0;
}
