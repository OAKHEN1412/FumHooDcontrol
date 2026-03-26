#ifndef BUTTON_NAV_H
#define BUTTON_NAV_H

#include <Arduino.h>

// Pin settings
#ifndef BTN_LEFT
#define BTN_LEFT  2
#endif

#ifndef BTN_RIGHT
#define BTN_RIGHT 3
#endif

#ifndef BTN_DOWN
#define BTN_DOWN  4
#endif

// Total buttons
#define TOTAL_BUTTONS 7

// Button layout map
// -1 means no button in that grid
static int buttonMap[3][3] = {
    {0, 1, 2},
    {3, 4, 5},
    {-1, 6, -1}
};

static int currentIndex = 0;

// ===== Function Declarations =====
void nav_init();
void nav_update();
void moveLeft();
void moveRight();
void moveDown();
int findNext(int index, int dx, int dy);
void highlightButton(int idx);

// ===== Function Implementations =====
void nav_init() {
    pinMode(BTN_LEFT, INPUT_PULLUP);
    pinMode(BTN_RIGHT, INPUT_PULLUP);
    pinMode(BTN_DOWN, INPUT_PULLUP);

    highlightButton(currentIndex);
}

void nav_update() {
    if (!digitalRead(BTN_LEFT)) {
        moveLeft();
        delay(200);
    }

    if (!digitalRead(BTN_RIGHT)) {
        moveRight();
        delay(200);
    }

    if (!digitalRead(BTN_DOWN)) {
        moveDown();
        delay(200);
    }
}

void moveLeft() {
    int next = findNext(currentIndex, -1, 0);
    if (next != -1) {
        currentIndex = next;
        highlightButton(currentIndex);
    }
}

void moveRight() {
    int next = findNext(currentIndex, 1, 0);
    if (next != -1) {
        currentIndex = next;
        highlightButton(currentIndex);
    }
}

void moveDown() {
    int next = findNext(currentIndex, 0, 1);
    if (next != -1) {
        currentIndex = next;
        highlightButton(currentIndex);
    }
}

int findNext(int index, int dx, int dy) {
    int x, y;

    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            if (buttonMap[row][col] == index) {
                y = row;
                x = col;

                int nx = x + dx;
                int ny = y + dy;

                if (nx >= 0 && nx < 3 && ny >= 0 && ny < 3) {
                    if (buttonMap[ny][nx] != -1) {
                        return buttonMap[ny][nx];
                    }
                }
            }
        }
    }
    return -1;
}

// You edit this to link with LVGL or your display system
void highlightButton(int idx) {
    Serial.print("Selected Button Index = ");
    Serial.println(idx);
}

#endif
