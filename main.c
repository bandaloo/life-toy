#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#include "rules.h"

int cellcolor[] = {255, 110, 0};
int colorlist[COLOR_AMOUNT][3] = {
    {255, 110, 0}, 
    {99, 223, 255}, 
    {249, 234, 67}, 
    {249, 67, 67}, 
    {114, 247, 66}, 
    {220, 98, 247}
}; 

struct state currstate;
int movemode = 1;

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

void render_placement(SDL_Renderer *renderer, struct state *state) {
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    if (state->paused) {
        SDL_Rect rect = {state->cellx * CELL_WIDTH, state->celly * CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT};
        SDL_RenderDrawRect(renderer, &rect);
    }
}

int clamp(int n, int l) {
    if (n < 0) return 0;
    if (n > l) return l;
    return n;
}

int main(int argc, char *argv[]) {
    currstate.paused = 0;
    currstate.clicked = 0;
    currstate.colorcounter = 0;
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
                case SDLK_c:
                    currstate.colorcounter++;
                    currstate.colorcounter %= COLOR_AMOUNT;
                    break;
                case SDLK_h:
                    currstate.cellx--;
                    currstate.cellx = clamp(currstate.cellx, FIELD_WIDTH - 1);
                    movemode = 1;
                    break;
                case SDLK_j:
                    currstate.celly++;
                    currstate.celly = clamp(currstate.celly, FIELD_HEIGHT - 1);
                    movemode = 1;
                    break;
                case SDLK_k:
                    currstate.celly--;
                    currstate.celly = clamp(currstate.celly, FIELD_HEIGHT - 1);
                    movemode = 1;
                    break;
                case SDLK_l:
                    currstate.cellx++;
                    currstate.cellx = clamp(currstate.cellx, FIELD_WIDTH - 1);
                    movemode = 1;
                    break;
                case SDLK_SPACE:
                    if (currstate.paused) currstate.clicked = 1;
                    break;
                default: break;
                }
            } else if (e.type == SDL_MOUSEMOTION) {
                SDL_GetMouseState(&(currstate.clickx), &(currstate.clicky));
                movemode = 0;
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
            if (movemode == 0) {
                currstate.cellx = currstate.clickx / CELL_WIDTH;
                currstate.celly = currstate.clicky / CELL_HEIGHT;
            } else {
            }
            if (currstate.clicked) {
                currstate.clicked = 0;
                currfield->board[currstate.cellx][currstate.celly] = !currfield->board[currstate.cellx][currstate.celly];
            }
        }
        SDL_RenderClear(renderer);
        render_field(renderer, currfield, colorlist[currstate.colorcounter][0], colorlist[currstate.colorcounter][1], colorlist[currstate.colorcounter][2], animtime, prevtime);
        render_placement(renderer, &currstate);
        //roundedBoxRGBA(renderer, 0, 0, 100, 100, 20, 0, 100, 0, 255);
        //aacircleRGBA (renderer, 200, 200, 100, 200, 0, 0, 255);
        SDL_RenderPresent(renderer);
    }
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}
