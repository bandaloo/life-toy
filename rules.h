#ifndef RULES_H_INCLUDED
#define RULES_H_INCLUDED

#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 540
#define FIELD_WIDTH 32
#define FIELD_HEIGHT 18
#define CELL_WIDTH (SCREEN_WIDTH / FIELD_WIDTH)
#define CELL_HEIGHT (SCREEN_HEIGHT / FIELD_HEIGHT)
#define COLOR_AMOUNT 6

#define SAME 0
#define BORN 1
#define DIES 2

#define STEP -1

struct field {
    int board[FIELD_WIDTH][FIELD_HEIGHT];
    struct field *next;
    struct field *prev;
    int index;
};

struct state {
    int paused;
    int clicked;
    int clickx;
    int clicky;
    int cellx;
    int celly;
    int rollbacklen;
    int colorcounter;
};

struct field * set_fields (struct field *first, int n, int chance);

struct field * generate_fields (int n);

extern int cellcolor[];

extern int colorlist[][3];

#endif // RULES_H_INCLUDED
