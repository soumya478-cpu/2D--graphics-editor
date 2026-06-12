#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define HEIGHT 20
#define WIDTH 60
#define MAX_OBJECTS 100

// Define Object Types
typedef enum {
    LINE,
    RECTANGLE,
    CIRCLE,
    TRIANGLE
} ObjectType;

// Shape coordinates structures
typedef struct {
    int x1, y1;
    int x2, y2;
} LineData;

typedef struct {
    int x1, y1; // Corner 1
    int x2, y2; // Corner 2
} RectData;

typedef struct {
    int cx, cy;
    int r;
} CircleData;

typedef struct {
    int x1, y1;
    int x2, y2;
    int x3, y3;
} TriangleData;

// Object structure containing type and specific data
typedef struct {
    int id; // 1-based unique identifier
    ObjectType type;
    union {
        LineData line;
        RectData rect;
        CircleData circle;
        TriangleData triangle;
    } data;
} Object;

// Global list of graphical objects
Object objects[MAX_OBJECTS];
int object_count = 0;
int next_object_id = 1;

// Drawing Canvas
char canvas[HEIGHT][WIDTH];

// Utility: Clear canvas to underscore '_'
void clear_canvas(char canvas[HEIGHT][WIDTH]) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            canvas[y][x] = '_';
        }
    }
}

// Draw a line using Bresenham's Line Algorithm
void draw_line(char canvas[HEIGHT][WIDTH], int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1);
    int dy = -abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx + dy;
    int e2;
    int x = x1, y = y1;

    while (1) {
        if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
            canvas[y][x] = '*';
        }
        if (x == x2 && y == y2) {
            break;
        }
        e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y += sy;
        }
    }
}

// Draw a rectangle outline
void draw_rectangle(char canvas[HEIGHT][WIDTH], int x1, int y1, int x2, int y2) {
    // Draw horizontal and vertical lines to form the outline
    draw_line(canvas, x1, y1, x2, y1); // top border
    draw_line(canvas, x1, y2, x2, y2); // bottom border
    draw_line(canvas, x1, y1, x1, y2); // left border
    draw_line(canvas, x2, y1, x2, y2); // right border
}

// Draw a circle using Bresenham's Midpoint Circle Algorithm
void draw_circle(char canvas[HEIGHT][WIDTH], int cx, int cy, int r) {
    if (r < 0) return;
    if (r == 0) {
        if (cx >= 0 && cx < WIDTH && cy >= 0 && cy < HEIGHT) {
            canvas[cy][cx] = '*';
        }
        return;
    }
    int x = 0;
    int y = r;
    int d = 3 - 2 * r;

    // Helper macro/function to safely plot symmetric points
    #define PLOT_POINT(px, py) do { \
        if ((px) >= 0 && (px) < WIDTH && (py) >= 0 && (py) < HEIGHT) { \
            canvas[py][px] = '*'; \
        } \
    } while(0)

    PLOT_POINT(cx + x, cy + y);
    PLOT_POINT(cx - x, cy + y);
    PLOT_POINT(cx + x, cy - y);
    PLOT_POINT(cx - x, cy - y);
    PLOT_POINT(cx + y, cy + x);
    PLOT_POINT(cx - y, cy + x);
    PLOT_POINT(cx + y, cy - x);
    PLOT_POINT(cx - y, cy - x);

    while (y >= x) {
        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
        PLOT_POINT(cx + x, cy + y);
        PLOT_POINT(cx - x, cy + y);
        PLOT_POINT(cx + x, cy - y);
        PLOT_POINT(cx - x, cy - y);
        PLOT_POINT(cx + y, cy + x);
        PLOT_POINT(cx - y, cy + x);
        PLOT_POINT(cx + y, cy - x);
        PLOT_POINT(cx - y, cy - x);
    }
    #undef PLOT_POINT
}

// Draw a triangle by connecting three vertices with lines
void draw_triangle(char canvas[HEIGHT][WIDTH], int x1, int y1, int x2, int y2, int x3, int y3) {
    draw_line(canvas, x1, y1, x2, y2);
    draw_line(canvas, x2, y2, x3, y3);
    draw_line(canvas, x3, y3, x1, y1);
}

// Redraw all active objects onto canvas
void redraw_canvas(char canvas[HEIGHT][WIDTH], Object list[], int count) {
    clear_canvas(canvas);
    for (int i = 0; i < count; i++) {
        switch (list[i].type) {
            case LINE:
                draw_line(canvas, list[i].data.line.x1, list[i].data.line.y1,
                                  list[i].data.line.x2, list[i].data.line.y2);
                break;
            case RECTANGLE:
                draw_rectangle(canvas, list[i].data.rect.x1, list[i].data.rect.y1,
                                       list[i].data.rect.x2, list[i].data.rect.y2);
                break;
            case CIRCLE:
                draw_circle(canvas, list[i].data.circle.cx, list[i].data.circle.cy,
                                    list[i].data.circle.r);
                break;
            case TRIANGLE:
                draw_triangle(canvas, list[i].data.triangle.x1, list[i].data.triangle.y1,
                                      list[i].data.triangle.x2, list[i].data.triangle.y2,
                                      list[i].data.triangle.x3, list[i].data.triangle.y3);
                break;
        }
    }
}

// Display canvas to console, with row and column index headings
void display_canvas(char canvas[HEIGHT][WIDTH]) {
    // Print upper header for column tens place
    printf("     ");
    for (int x = 0; x < WIDTH; x++) {
        if (x >= 10) {
            printf("%d", (x / 10) % 10);
        } else {
            printf(" ");
        }
    }
    printf("\n");

    // Print lower header for column ones place
    printf("     ");
    for (int x = 0; x < WIDTH; x++) {
        printf("%d", x % 10);
    }
    printf("\n");

    // Print top canvas boundary border
    printf("    +");
    for (int x = 0; x < WIDTH; x++) printf("-");
    printf("+\n");

    // Print canvas rows with vertical line borders and row numbers
    for (int y = 0; y < HEIGHT; y++) {
        printf(" %2d |", y);
        for (int x = 0; x < WIDTH; x++) {
            putchar(canvas[y][x]);
        }
        printf("|\n");
    }

    // Print bottom canvas boundary border
    printf("    +");
    for (int x = 0; x < WIDTH; x++) printf("-");
    printf("+\n");
}

// Robustly read an integer from stdin in [min, max] range
int read_int(const char *prompt, int min_val, int max_val) {
    char buffer[256];
    int val;
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("\nEOF reached. Exiting graphics editor.\n");
            exit(0);
        }
        // Remove trailing newline
        buffer[strcspn(buffer, "\r\n")] = '\0';

        // Check if string is empty
        if (strlen(buffer) == 0) {
            printf("Error: Empty input. Please enter a number.\n");
            continue;
        }

        // Try reading integer
        char *endptr;
        long parsed = strtol(buffer, &endptr, 10);
        // If no digits parsed or trailing non-space characters exist
        if (endptr == buffer || *endptr != '\0') {
            printf("Error: Invalid integer input. Please try again.\n");
            continue;
        }

        if (parsed >= min_val && parsed <= max_val) {
            val = (int)parsed;
            return val;
        } else {
            printf("Error: Input must be between %d and %d. Please try again.\n", min_val, max_val);
        }
    }
}

// Print object summary description
void get_object_desc(Object obj, char *buffer, int size) {
    switch (obj.type) {
        case LINE:
            snprintf(buffer, size, "Line: (%d, %d) -> (%d, %d)",
                     obj.data.line.x1, obj.data.line.y1,
                     obj.data.line.x2, obj.data.line.y2);
            break;
        case RECTANGLE:
            snprintf(buffer, size, "Rectangle: corners (%d, %d) and (%d, %d)",
                     obj.data.rect.x1, obj.data.rect.y1,
                     obj.data.rect.x2, obj.data.rect.y2);
            break;
        case CIRCLE:
            snprintf(buffer, size, "Circle: center (%d, %d), radius %d",
                     obj.data.circle.cx, obj.data.circle.cy,
                     obj.data.circle.r);
            break;
        case TRIANGLE:
            snprintf(buffer, size, "Triangle: vertices (%d, %d), (%d, %d), (%d, %d)",
                     obj.data.triangle.x1, obj.data.triangle.y1,
                     obj.data.triangle.x2, obj.data.triangle.y2,
                     obj.data.triangle.x3, obj.data.triangle.y3);
            break;
    }
}

// List all active objects
void list_active_objects() {
    if (object_count == 0) {
        printf("No objects currently exist in the picture.\n");
        return;
    }
    printf("\n=== Active Graphical Objects ===\n");
    char desc[256];
    for (int i = 0; i < object_count; i++) {
        get_object_desc(objects[i], desc, sizeof(desc));
        printf("%d. [ID %d] %s\n", i + 1, objects[i].id, desc);
    }
    printf("================================\n");
}

// Add a new object helper
void add_object_wizard() {
    if (object_count >= MAX_OBJECTS) {
        printf("Error: Canvas object list is full (%d limit).\n", MAX_OBJECTS);
        return;
    }

    printf("\n--- Add New Object ---\n");
    printf("1. Line\n");
    printf("2. Rectangle\n");
    printf("3. Circle\n");
    printf("4. Triangle\n");
    int type_choice = read_int("Choose shape type (1-4): ", 1, 4);

    Object obj;
    obj.id = next_object_id++;

    switch (type_choice) {
        case 1: // Line
            obj.type = LINE;
            printf("Enter start coordinates:\n");
            obj.data.line.x1 = read_int("  x1 (column 0-59): ", 0, WIDTH - 1);
            obj.data.line.y1 = read_int("  y1 (row 0-19): ", 0, HEIGHT - 1);
            printf("Enter end coordinates:\n");
            obj.data.line.x2 = read_int("  x2 (column 0-59): ", 0, WIDTH - 1);
            obj.data.line.y2 = read_int("  y2 (row 0-19): ", 0, HEIGHT - 1);
            break;
        case 2: // Rectangle
            obj.type = RECTANGLE;
            printf("Enter corner 1 coordinates:\n");
            obj.data.rect.x1 = read_int("  x1 (column 0-59): ", 0, WIDTH - 1);
            obj.data.rect.y1 = read_int("  y1 (row 0-19): ", 0, HEIGHT - 1);
            printf("Enter opposite corner 2 coordinates:\n");
            obj.data.rect.x2 = read_int("  x2 (column 0-59): ", 0, WIDTH - 1);
            obj.data.rect.y2 = read_int("  y2 (row 0-19): ", 0, HEIGHT - 1);
            break;
        case 3: // Circle
            obj.type = CIRCLE;
            printf("Enter center coordinates:\n");
            obj.data.circle.cx = read_int("  cx (column 0-59): ", 0, WIDTH - 1);
            obj.data.circle.cy = read_int("  cy (row 0-19): ", 0, HEIGHT - 1);
            obj.data.circle.r = read_int("  radius (1-50): ", 1, 50);
            break;
        case 4: // Triangle
            obj.type = TRIANGLE;
            printf("Enter vertex 1 coordinates:\n");
            obj.data.triangle.x1 = read_int("  x1 (column 0-59): ", 0, WIDTH - 1);
            obj.data.triangle.y1 = read_int("  y1 (row 0-19): ", 0, HEIGHT - 1);
            printf("Enter vertex 2 coordinates:\n");
            obj.data.triangle.x2 = read_int("  x2 (column 0-59): ", 0, WIDTH - 1);
            obj.data.triangle.y2 = read_int("  y2 (row 0-19): ", 0, HEIGHT - 1);
            printf("Enter vertex 3 coordinates:\n");
            obj.data.triangle.x3 = read_int("  x3 (column 0-59): ", 0, WIDTH - 1);
            obj.data.triangle.y3 = read_int("  y3 (row 0-19): ", 0, HEIGHT - 1);
            break;
    }

    objects[object_count++] = obj;
    printf("Success: Object added successfully!\n");
}

// Modify an existing object helper
void modify_object_wizard() {
    if (object_count == 0) {
        printf("No objects to modify.\n");
        return;
    }

    list_active_objects();
    int choice = read_int("Enter the list number of the object to modify (1-count): ", 1, object_count);
    int index = choice - 1;

    Object *obj = &objects[index];
    char desc[256];
    get_object_desc(*obj, desc, sizeof(desc));
    printf("Modifying object: %s\n", desc);

    printf("1. Line\n");
    printf("2. Rectangle\n");
    printf("3. Circle\n");
    printf("4. Triangle\n");
    int type_choice = read_int("Choose new shape type (1-4): ", 1, 4);

    switch (type_choice) {
        case 1: // Line
            obj->type = LINE;
            printf("Enter new start coordinates:\n");
            obj->data.line.x1 = read_int("  x1 (column 0-59): ", 0, WIDTH - 1);
            obj->data.line.y1 = read_int("  y1 (row 0-19): ", 0, HEIGHT - 1);
            printf("Enter new end coordinates:\n");
            obj->data.line.x2 = read_int("  x2 (column 0-59): ", 0, WIDTH - 1);
            obj->data.line.y2 = read_int("  y2 (row 0-19): ", 0, HEIGHT - 1);
            break;
        case 2: // Rectangle
            obj->type = RECTANGLE;
            printf("Enter new corner 1 coordinates:\n");
            obj->data.rect.x1 = read_int("  x1 (column 0-59): ", 0, WIDTH - 1);
            obj->data.rect.y1 = read_int("  y1 (row 0-19): ", 0, HEIGHT - 1);
            printf("Enter new opposite corner 2 coordinates:\n");
            obj->data.rect.x2 = read_int("  x2 (column 0-59): ", 0, WIDTH - 1);
            obj->data.rect.y2 = read_int("  y2 (row 0-19): ", 0, HEIGHT - 1);
            break;
        case 3: // Circle
            obj->type = CIRCLE;
            printf("Enter new center coordinates:\n");
            obj->data.circle.cx = read_int("  cx (column 0-59): ", 0, WIDTH - 1);
            obj->data.circle.cy = read_int("  cy (row 0-19): ", 0, HEIGHT - 1);
            obj->data.circle.r = read_int("  radius (1-50): ", 1, 50);
            break;
        case 4: // Triangle
            obj->type = TRIANGLE;
            printf("Enter new vertex 1 coordinates:\n");
            obj->data.triangle.x1 = read_int("  x1 (column 0-59): ", 0, WIDTH - 1);
            obj->data.triangle.y1 = read_int("  y1 (row 0-19): ", 0, HEIGHT - 1);
            printf("Enter new vertex 2 coordinates:\n");
            obj->data.triangle.x2 = read_int("  x2 (column 0-59): ", 0, WIDTH - 1);
            obj->data.triangle.y2 = read_int("  y2 (row 0-19): ", 0, HEIGHT - 1);
            printf("Enter new vertex 3 coordinates:\n");
            obj->data.triangle.x3 = read_int("  x3 (column 0-59): ", 0, WIDTH - 1);
            obj->data.triangle.y3 = read_int("  y3 (row 0-19): ", 0, HEIGHT - 1);
            break;
    }

    printf("Success: Object modified successfully!\n");
}

// Delete an object helper
void delete_object_wizard() {
    if (object_count == 0) {
        printf("No objects to delete.\n");
        return;
    }

    list_active_objects();
    int choice = read_int("Enter the list number of the object to delete (1-count): ", 1, object_count);
    int index = choice - 1;

    // Shift subsequent elements left
    for (int i = index; i < object_count - 1; i++) {
        objects[i] = objects[i + 1];
    }
    object_count--;

    printf("Success: Object deleted successfully!\n");
}

int main() {
    // Initialise canvas
    clear_canvas(canvas);

    printf("===============================================\n");
    printf("      WELCOME TO THE 2D GRAPHICS EDITOR        \n");
    printf("===============================================\n");
    printf("Canvas Resolution: %d columns x %d rows\n", WIDTH, HEIGHT);
    printf("Background Character: '_' | Drawing Character: '*'\n\n");

    while (1) {
        printf("\n------------- Main Menu -------------\n");
        printf("1. Add Object to Picture\n");
        printf("2. Modify Object in Picture\n");
        printf("3. Delete Object from Picture\n");
        printf("4. Display Picture\n");
        printf("5. List Active Objects\n");
        printf("6. Clear All Objects (Reset Canvas)\n");
        printf("7. Exit Editor\n");
        printf("-------------------------------------\n");

        int choice = read_int("Enter menu selection (1-7): ", 1, 7);

        switch (choice) {
            case 1:
                add_object_wizard();
                redraw_canvas(canvas, objects, object_count);
                display_canvas(canvas);
                break;
            case 2:
                modify_object_wizard();
                redraw_canvas(canvas, objects, object_count);
                display_canvas(canvas);
                break;
            case 3:
                delete_object_wizard();
                redraw_canvas(canvas, objects, object_count);
                display_canvas(canvas);
                break;
            case 4:
                redraw_canvas(canvas, objects, object_count);
                display_canvas(canvas);
                break;
            case 5:
                list_active_objects();
                break;
            case 6: {
                char confirm[10];
                printf("Are you sure you want to clear all objects? (y/n): ");
                if (fgets(confirm, sizeof(confirm), stdin) != NULL) {
                    if (confirm[0] == 'y' || confirm[0] == 'Y') {
                        object_count = 0;
                        clear_canvas(canvas);
                        printf("Canvas cleared successfully.\n");
                    } else {
                        printf("Operation cancelled.\n");
                    }
                }
                break;
            }
            case 7:
                printf("\nThank you for using the 2D Graphics Editor. Goodbye!\n");
                return 0;
        }
    }
}
