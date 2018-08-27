#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "rules.h"

int xaround[] = {1, 1, 0, -1, -1, -1, 0, 1};
int yaround[] = {0, 1, 1, 1, 0, -1, -1, -1};

int rulelist[] = {DIES, DIES, SAME, BORN, DIES, DIES, DIES, DIES, DIES};

int wrap_num(int i, int l) {
    if (i == -1) i = l - 1;
    else if (i == l) i = 0;
    return i;
}

int is_alive(int i, int j, struct field *field, int wrap) {
    if (wrap) {
        i = wrap_num(i, FIELD_WIDTH);
        j = wrap_num(j, FIELD_HEIGHT);
    }
    return i >= 0 && i < FIELD_WIDTH && j >= 0 && j < FIELD_HEIGHT && field->board[i][j];
}

struct field * set_fields (struct field *first, int n, int chance) {
    srand(time(0));
    int roll;

    struct field *curr = first;
    for (int m = 0; m < n; m++) {
        for (int i = 0; i < FIELD_WIDTH; i++) {
            for (int j = 0; j < FIELD_HEIGHT; j++) {
                if (chance >= 0) {
                    roll = rand() % 100 + 1;
                    curr->board[i][j] = (roll <= chance) ? 1 : 0;
                } else {
                    curr->next->board[i][j] = curr->board[i][j];
                    int count = 0;
                    for (int b = 0; b < 8; b++) {
                        if (is_alive(i + xaround[b], j + yaround[b], curr, 1)) count++;
                    }
                    if (rulelist[count] == DIES) curr->next->board[i][j] = 0;
                    else if (rulelist[count] == BORN) curr->next->board[i][j] = 1;
                }
            }
        }
        //printf("cleared board %d\n", m);
        //printf("index %d\n", curr->next->index);
        curr = curr->next;
    }
    return curr;
}

struct field * generate_fields (int n) {
    struct field *last;
    struct field *curr;
    struct field *first;
    for (int i = 0; i < n; i++) {
        curr = (struct field *) malloc(sizeof(struct field));
        if (i == 0) {
            first = curr;
        } else {
            last->next = curr;
            curr->prev = last;
        }
        curr->index = i;
        last = curr;
    }
    // connect list back to beginning
    last->next = first;
    first->prev = last;
    set_fields(first, n, 0);
    return first;
}
