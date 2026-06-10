#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ============================================================================
// STRUCTS, ENUMS & ARCHITECTURE DEFINITIONS
// ============================================================================
#define WIDTH 80
#define HEIGHT 24
#define MAX_SHAPES 50

typedef enum { LINE, RECTANGLE, TRIANGLE, CIRCLE } ShapeType;

typedef struct {
    ShapeType type;
    int x1, y1;
    int x2, y2;
    int x3, y3; 
    int radius; 
    char ch;
    int active;
} Shape;

char canvas[HEIGHT][WIDTH];
Shape shapes[MAX_SHAPES];

// ============================================================================
// CORE EDITOR LOGIC FUNCTIONS
// ============================================================================
void init_editor(void) {
    for (int i = 0; i < MAX_SHAPES; i++) {
        shapes[i].active = 0;
    }
}

void plot_pixel(int x, int y, char ch) {
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
        canvas[y][x] = ch;
    }
}

void draw_line(int x1, int y1, int x2, int y2, char ch) {
    int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
    int dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
    int err = dx + dy, e2;
    while (1) {
        plot_pixel(x1, y1, ch);
        if (x1 == x2 && y1 == y2) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x1 += sx; }
        if (e2 <= dx) { err += dx; y1 += sy; }
    }
}

void draw_rectangle(int x1, int y1, int x2, int y2, char ch) {
    draw_line(x1, y1, x2, y1, ch);
    draw_line(x2, y1, x2, y2, ch);
    draw_line(x2, y2, x1, y2, ch);
    draw_line(x1, y2, x1, y1, ch);
}

void draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3, char ch) {
    draw_line(x1, y1, x2, y2, ch);
    draw_line(x2, y2, x3, y3, ch);
    draw_line(x3, y3, x1, y1, ch);
}

void draw_circle(int xc, int yc, int r, char ch) {
    int x = 0, y = r;
    int d = 3 - 2 * r;
    while (y >= x) {
        plot_pixel(xc+x, yc+y, ch); plot_pixel(xc-x, yc+y, ch);
        plot_pixel(xc+x, yc-y, ch); plot_pixel(xc-x, yc-y, ch);
        plot_pixel(xc+y, yc+x, ch); plot_pixel(xc-y, yc+x, ch);
        plot_pixel(xc+y, yc-x, ch); plot_pixel(xc-y, yc-x, ch);
        x++;
        if (d > 0) { y--; d = d + 4 * (x - y) + 10; }
        else d = d + 4 * x + 6;
    }
}

void render_canvas(void) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            canvas[i][j] = ' ';
        }
    }
    for (int i = 0; i < MAX_SHAPES; i++) {
        if (shapes[i].active) {
            switch (shapes[i].type) {
                case LINE:      draw_line(shapes[i].x1, shapes[i].y1, shapes[i].x2, shapes[i].y2, shapes[i].ch); break;
                case RECTANGLE: draw_rectangle(shapes[i].x1, shapes[i].y1, shapes[i].x2, shapes[i].y2, shapes[i].ch); break;
                case TRIANGLE:  draw_triangle(shapes[i].x1, shapes[i].y1, shapes[i].x2, shapes[i].y2, shapes[i].x3, shapes[i].y3, shapes[i].ch); break;
                case CIRCLE:    draw_circle(shapes[i].x1, shapes[i].y1, shapes[i].radius, shapes[i].ch); break;
            }
        }
    }
}

void display_canvas(void) {
    for (int j = 0; j < WIDTH + 2; j++) printf("_");
    printf("\n");
    for (int i = 0; i < HEIGHT; i++) {
        printf("|");
        for (int j = 0; j < WIDTH; j++) printf("%c", canvas[i][j]);
        printf("|\n");
    }
    for (int j = 0; j < WIDTH + 2; j++) printf("_");
    printf("\n");
}

int add_shape(Shape s) {
    for (int i = 0; i < MAX_SHAPES; i++) {
        if (!shapes[i].active) { shapes[i] = s; shapes[i].active = 1; return i; }
    }
    return -1;
}

int delete_shape(int index) {
    if (index >= 0 && index < MAX_SHAPES && shapes[index].active) { shapes[index].active = 0; return 1; }
    return 0;
}

void modify_shape(int index, Shape s) {
    if (index >= 0 && index < MAX_SHAPES && shapes[index].active) { shapes[index] = s; shapes[index].active = 1; }
}

void print_shapes_list(void) {
    int count = 0;
    for (int i = 0; i < MAX_SHAPES; i++) {
        if (shapes[i].active) {
            const char* types[] = {"Line", "Rectangle", "Triangle", "Circle"};
            printf("ID [%d]: %s (Drawing character: '%c')\n", i, types[shapes[i].type], shapes[i].ch);
            count++;
        }
    }
    if (count == 0) printf("(No objects created yet)\n");
}

// ============================================================================
// INTERACTIVE CONSOLE MENU & ENTRY POINT
// ============================================================================
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

Shape prompt_shape_details() {
    Shape s;
    int type_choice;
    printf("Select Shape Type:\n1. Line\n2. Rectangle\n3. Triangle\n4. Circle\nEnter choice (1-4): ");
    scanf("%d", &type_choice);
    s.type = (ShapeType)(type_choice - 1);

    if (s.type == LINE) {
        printf("Enter Start X (0-79) & Y (0-23): "); scanf("%d %d", &s.x1, &s.y1);
        printf("Enter End X (0-79) & Y (0-23): "); scanf("%d %d", &s.x2, &s.y2);
    } else if (s.type == RECTANGLE) {
        printf("Enter Top-Left X (0-79) & Y (0-23): "); scanf("%d %d", &s.x1, &s.y1);
        printf("Enter Bottom-Right X (0-79) & Y (0-23): "); scanf("%d %d", &s.x2, &s.y2);
    } else if (s.type == TRIANGLE) {
        printf("Point 1 X & Y: "); scanf("%d %d", &s.x1, &s.y1);
        printf("Point 2 X & Y: "); scanf("%d %d", &s.x2, &s.y2);
        printf("Point 3 X & Y: "); scanf("%d %d", &s.x3, &s.y3);
    } else if (s.type == CIRCLE) {
        printf("Enter Center X (0-79) & Y (0-23): "); scanf("%d %d", &s.x1, &s.y1);
        printf("Enter Radius: "); scanf("%d", &s.radius);
    }
    printf("Enter drawing character (e.g., * or #): ");
    clear_input_buffer();
    scanf("%c", &s.ch);
    s.active = 1;
    return s;
}

int main() {
    int choice, id;
    init_editor();

    while (1) {
        render_canvas();
        display_canvas();
        printf("\n--- 2D Object-Based Graphics Editor ---\n");
        printf("1. Add Object (Line, Rect, Triangle, Circle)\n");
        printf("2. Delete Object\n");
        printf("3. Modify Object\n");
        printf("4. List All Current Objects\n");
        printf("5. Exit\n");
        printf("Enter menu choice: ");
        if (scanf("%d", &choice) != 1) break;

        if (choice == 1) {
            Shape new_shape = prompt_shape_details();
            add_shape(new_shape);
        } else if (choice == 2) {
            printf("\n--- Current Objects Available ---\n");
            print_shapes_list();
            printf("Enter Object ID to delete: ");
            scanf("%d", &id);
            if (!delete_shape(id)) printf("Invalid ID selected.\n");
        } else if (choice == 3) {
            printf("\n--- Current Objects Available ---\n");
            print_shapes_list();
            printf("Enter Object ID to modify: ");
            scanf("%d", &id);
            Shape updated = prompt_shape_details();
            modify_shape(id, updated);
        } else if (choice == 4) {
            printf("\n--- Current Objects List ---\n");
            print_shapes_list();
            printf("\nPress Enter to return to menu...");
            clear_input_buffer(); getchar();
        } else if (choice == 5) {
            printf("Goodbye!\n");
            break;
        }
    }
    return 0;
}
