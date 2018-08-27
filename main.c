#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#include "rules.h"

int cellcolor[] = {255, 110, 0};

void render_field(SDL_Renderer *renderer, struct field *field, int r, int g, int b, int animtime, int prevtime) {
    SDL_Rect rect = {0, 0, CELL_WIDTH, CELL_HEIGHT};
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    int currtime = SDL_GetTicks();
    float animationscalar = (float) (currtime - prevtime) / (float) animtime;
    if (animationscalar > 1.0) animationscalar = 1;

    int groww = CELL_WIDTH * animationscalar;
    int growh = CELL_HEIGHT * animationscalar;

    int shrinkw = CELL_WIDTH - groww;
    int shrinkh = CELL_WIDTH - growh;

    SDL_Rect srect = {0, 0, shrinkw, shrinkh};
    SDL_Rect grect = {0, 0, groww, growh};

    for (int i = 0; i < FIELD_WIDTH; i++) {
        rect.x = i * CELL_HEIGHT;
        srect.x = i * CELL_HEIGHT + groww / 2;
        grect.x = i * CELL_HEIGHT + shrinkw / 2;
        for (int j = 0; j < FIELD_HEIGHT; j++) {
            rect.y = j * CELL_HEIGHT;
            srect.y = j * CELL_HEIGHT + growh / 2;
            grect.y = j * CELL_HEIGHT + shrinkh / 2;
            if (field->board[i][j]) {
                (!(field->prev->board[i][j])) ? SDL_RenderFillRect(renderer, &grect) : SDL_RenderFillRect(renderer, &rect);
            } else if (!(field->board[i][j]) && field->prev->board[i][j]) {
                SDL_RenderFillRect(renderer, &srect);
            }
        }
    }
}

struct state currstate;

void render_placement(SDL_Renderer *renderer, struct state *state) {
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    if (state->paused) {
        SDL_Rect rect = {state->cellx * CELL_WIDTH, state->celly * CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT};
        SDL_RenderDrawRect(renderer, &rect);
    }
}

int main(int argc, char *argv[])
{
    currstate.paused = 0;
    currstate.clicked = 0;
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        return EXIT_FAILURE;
    }
    SDL_Window *window = SDL_CreateWindow("life", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        return EXIT_FAILURE;
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (window == NULL) {
        return EXIT_FAILURE;
    }

    struct field *currfield = generate_fields(10);
    set_fields(currfield, 1, 0);

    int prevtime = SDL_GetTicks();

    int steptime = 300;
    int animtime = 200;

    SDL_Event e;
    int quit = 0;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            } else if (e.type == SDL_KEYDOWN) {
                switch(e.key.keysym.sym) {
                case SDLK_RETURN:
                    currstate.paused = !(currstate.paused);
                    break;
                case SDLK_LEFT:
                    if (currstate.paused) currfield = currfield->prev;
                    break;
                default: break;
                }
            } else if (e.type == SDL_MOUSEMOTION) {
                SDL_GetMouseState(&(currstate.clickx), &(currstate.clicky));
            } else if (e.type == SDL_MOUSEBUTTONDOWN && currstate.paused) {
                //SDL_GetMouseState(&clickx, &clicky);
                currstate.clicked = 1;
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        if (!(currstate.paused)) {
            int currtime = SDL_GetTicks();
            if (currtime - prevtime >= steptime) {
                prevtime = currtime;
                currfield = set_fields(currfield, 1, STEP);
                printf("%d\n", currfield->index);
            }
        }
        else {
            SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
            currstate.cellx = currstate.clickx / CELL_WIDTH;
            currstate.celly = currstate.clicky / CELL_HEIGHT;
            if (currstate.clicked) {
                currstate.clicked = 0;
                currfield->board[currstate.cellx][currstate.celly] = !currfield->board[currstate.cellx][currstate.celly];
            }
        }
        SDL_RenderClear(renderer);
        render_field(renderer, currfield, cellcolor[0], cellcolor[1], cellcolor[2], animtime, prevtime);
        render_placement(renderer, &currstate);
        //roundedBoxRGBA(renderer, 0, 0, 100, 100, 20, 0, 100, 0, 255);
        //aacircleRGBA (renderer, 200, 200, 100, 200, 0, 0, 255);
        SDL_RenderPresent(renderer);
    }
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}
