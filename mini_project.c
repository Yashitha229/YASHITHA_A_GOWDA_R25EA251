#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define ROWS 25
#define COLS 60
#define MAX_OBJECTS 50

#define EMPTY '_'
#define DRAW '*'

typedef enum {
    LINE = 1,
    RECTANGLE,
    CIRCLE,
    TRIANGLE
} ObjectType;

typedef struct {
    int id;
    ObjectType type;

    int x1, y1;
    int x2, y2;
    int x3, y3;
    int radius;

    int active;
} Object;

char canvas[ROWS][COLS];
Object objects[MAX_OBJECTS];
int objectCount = 0;
int nextId = 1;

/* ---------- Canvas Functions ---------- */

void clearCanvas() {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            canvas[i][j] = EMPTY;
        }
    }
}

void plotPoint(int x, int y) {
    if (x >= 0 && x < COLS && y >= 0 && y < ROWS) {
        canvas[y][x] = DRAW;
    }
}

void displayCanvas() {
    printf("\n");
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            printf("%c", canvas[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

/* ---------- Drawing Functions ---------- */

void drawLine(int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);

    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;

    int err = dx - dy;

    while (1) {
        plotPoint(x1, y1);

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

void drawRectangle(int x1, int y1, int x2, int y2) {
    drawLine(x1, y1, x2, y1);
    drawLine(x2, y1, x2, y2);
    drawLine(x2, y2, x1, y2);
    drawLine(x1, y2, x1, y1);
}

// Fixed to Midpoint Circle Algorithm for a solid, gap-free border
void drawCircle(int cx, int cy, int radius) {
    int x = radius;
    int y = 0;
    int error = 0;

    while (x >= y) {
        plotPoint(cx + x, cy + y);
        plotPoint(cx + y, cy + x);
        plotPoint(cx - y, cy + x);
        plotPoint(cx - x, cy + y);
        plotPoint(cx - x, cy - y);
        plotPoint(cx - y, cy - x);
        plotPoint(cx + y, cy - x);
        plotPoint(cx + x, cy - y);

        if (error <= 0) {
            y += 1;
            error += 2 * y + 1;
        }
        if (error > 0) {
            x -= 1;
            error -= 2 * x + 1;
        }
    }
}

void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3) {
    drawLine(x1, y1, x2, y2);
    drawLine(x2, y2, x3, y3);
    drawLine(x3, y3, x1, y1);
}

/* ---------- Object Handling ---------- */

void drawObject(Object obj) {
    if (!obj.active)
        return;

    switch (obj.type) {
        case LINE:
            drawLine(obj.x1, obj.y1, obj.x2, obj.y2);
            break;
        case RECTANGLE:
            drawRectangle(obj.x1, obj.y1, obj.x2, obj.y2);
            break;
        case CIRCLE:
            drawCircle(obj.x1, obj.y1, obj.radius);
            break;
        case TRIANGLE:
            drawTriangle(obj.x1, obj.y1, obj.x2, obj.y2, obj.x3, obj.y3);
            break;
    }
}

void redrawAllObjects() {
    clearCanvas();
    for (int i = 0; i < objectCount; i++) {
        if (objects[i].active) {
            drawObject(objects[i]);
        }
    }
}

void listObjects() {
    printf("\nObjects in picture:\n");
    for (int i = 0; i < objectCount; i++) {
        if (objects[i].active) {
            printf("ID: %d | Type: ", objects[i].id);
            switch (objects[i].type) {
                case LINE:      printf("Line\n"); break;
                case RECTANGLE: printf("Rectangle\n"); break;
                case CIRCLE:    printf("Circle\n"); break;
                case TRIANGLE:  printf("Triangle\n"); break;
            }
        }
    }
}

int findObjectIndexById(int id) {
    for (int i = 0; i < objectCount; i++) {
        if (objects[i].id == id && objects[i].active) {
            return i;
        }
    }
    return -1;
}

/* ---------- Menu Operations ---------- */

void addObject() {
    if (objectCount >= MAX_OBJECTS) {
        printf("Object limit reached!\n");
        return;
    }

    Object obj;
    obj.id = nextId++;
    obj.active = 1;

    printf("\nChoose object type:\n");
    printf("1. Line\n");
    printf("2. Rectangle\n");
    printf("3. Circle\n");
    printf("4. Triangle\n");
    printf("Enter choice: ");
    
    int typeChoice;
    scanf("%d", &typeChoice);
    if (typeChoice < 1 || typeChoice > 4) {
        printf("Invalid object type!\n");
        return;
    }
    obj.type = (ObjectType)typeChoice;

    switch (obj.type) {
        case LINE:
            printf("Enter x1 y1 x2 y2: ");
            scanf("%d %d %d %d", &obj.x1, &obj.y1, &obj.x2, &obj.y2);
            break;

        case RECTANGLE:
            printf("Enter top-left x1 y1 and bottom-right x2 y2: ");
            scanf("%d %d %d %d", &obj.x1, &obj.y1, &obj.x2, &obj.y2);
            break;

        case CIRCLE:
            printf("Enter center x y and radius: ");
            scanf("%d %d %d", &obj.x1, &obj.y1, &obj.radius);
            break;

        case TRIANGLE:
            printf("Enter x1 y1 x2 y2 x3 y3: ");
            scanf("%d %d %d %d %d %d", &obj.x1, &obj.y1, &obj.x2, &obj.y2, &obj.x3, &obj.y3);
            break;
    }

    objects[objectCount++] = obj;
    redrawAllObjects();
    printf("Object added successfully with ID %d.\n", obj.id);
}

void deleteObject() {
    int id;
    listObjects();

    printf("\nEnter object ID to delete: ");
    scanf("%d", &id);

    int index = findObjectIndexById(id);
    if (index == -1) {
        printf("Object not found!\n");
        return;
    }

    objects[index].active = 0;
    redrawAllObjects();
    printf("Object deleted successfully.\n");
}

void modifyObject() {
    int id;
    listObjects();

    printf("\nEnter object ID to modify: ");
    scanf("%d", &id);

    int index = findObjectIndexById(id);
    if (index == -1) {
        printf("Object not found!\n");
        return;
    }

    printf("\nEnter new details for this object.\n");
    switch (objects[index].type) {
        case LINE:
            printf("Enter new x1 y1 x2 y2: ");
            scanf("%d %d %d %d", &objects[index].x1, &objects[index].y1, &objects[index].x2, &objects[index].y2);
            break;

        case RECTANGLE:
            printf("Enter new top-left x1 y1 and bottom-right x2 y2: ");
            scanf("%d %d %d %d", &objects[index].x1, &objects[index].y1, &objects[index].x2, &objects[index].y2);
            break;

        case CIRCLE:
            printf("Enter new center x y and radius: ");
            scanf("%d %d %d", &objects[index].x1, &objects[index].y1, &objects[index].radius);
            break;

        case TRIANGLE:
            printf("Enter new x1 y1 x2 y2 x3 y3: ");
            scanf("%d %d %d %d %d %d", &objects[index].x1, &objects[index].y1, &objects[index].x2, &objects[index].y2, &objects[index].x3, &objects[index].y3);
            break;
    }

    redrawAllObjects();
    printf("Object modified successfully.\n");
}

/* ---------- Main Function ---------- */

int main() {
    int choice;
    clearCanvas();

    do {
        printf("\n===== 2D Graphics Editor =====\n");
        printf("1. Add Object\n");
        printf("2. Delete Object\n");
        printf("3. Modify Object\n");
        printf("4. Display Picture\n");
        printf("5. List Objects\n");
        printf("6. Clear Picture\n");
        printf("0. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: addObject(); break;
            case 2: deleteObject(); break;
            case 3: modifyObject(); break;
            case 4: displayCanvas(); break;
            case 5: listObjects(); break;
            case 6:
                for (int i = 0; i < objectCount; i++) {
                    objects[i].active = 0;
                }
                clearCanvas();
                printf("Picture cleared successfully.\n");
                break;
            case 0: printf("Exiting program...\n"); break;
            default: printf("Invalid choice! Try again.\n");
        }
    } while (choice != 0);

    return 0;
}
