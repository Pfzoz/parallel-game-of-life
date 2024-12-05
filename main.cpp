#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <iostream>
#include <vector>

int SCREEN_WIDTH = 1000, SCREEN_HEIGHT = 1000;
int GAME_WIDTH = 100, GAME_HEIGHT = 100;
int CELL_WIDTH = SCREEN_WIDTH/GAME_WIDTH, CELL_HEIGHT = SCREEN_HEIGHT/GAME_HEIGHT;


bool init_SDL()
{
    return !(SDL_Init(SDL_INIT_VIDEO) < 0);
}

void draw_board(SDL_Surface *surface, std::vector<std::vector<bool>> game_board)
{
    for (int i = 0; i < game_board.size(); i++) {
        for (int j = 0; j < game_board[0].size(); j++) {
            SDL_Rect rect;
            rect.h = CELL_HEIGHT;
            rect.w = CELL_WIDTH;
            rect.x = j*CELL_HEIGHT;
            rect.y = i*CELL_WIDTH;
            Uint8 color = 0x00;
            if (game_board[i][j]) {
                color = 0xFF;
            }
            SDL_FillRect(surface, &rect, SDL_MapRGB(surface->format, color, color, color));
        }
    }
}

int get_live_neighbors(std::vector<std::vector<bool>> &game_board, int line, int column)
{
    int live = 0;
    if (line > 0) {
        live += game_board[line-1][column];
        if (column > 0) {
            live += game_board[line-1][column-1];
        }
        if (column < game_board[0].size() - 1) {
            live += game_board[line-1][column+1];
        }
    }
    if (column > 0) {
        live += game_board[line][column-1];
    }
    if (column < game_board[0].size() - 1) {
        live += game_board[line][column+1];
    }
    if (line < game_board.size() - 1) {
        live += game_board[line+1][column];
        if (column > 0) {
            live += game_board[line+1][column-1];
        }
        if (column < game_board[0].size() - 1) {
            live += game_board[line+1][column+1];
        }
    }
    return live;
}


std::vector<std::vector<bool>> change_board(std::vector<std::vector<bool>> &game_board)
{
    std::vector<std::vector<bool>> new_board(game_board);
    for (int i = 0; i < game_board.size(); i++) {
        for (int j = 0; j < game_board[0].size(); j++) {
            int live = get_live_neighbors(game_board, i, j);
            if (live < 2 || live > 3) {
                new_board[i][j] = false;
            } else if (game_board[i][j] == false && live == 3) {
                new_board[i][j] = true;
            }
        }
    }
    return new_board;
}

int main()
{
    if (!init_SDL()) {
        std::cout << "Could not initialize SDL. SDL_Error:" << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Conway's Game of Life", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Surface *surface = SDL_GetWindowSurface(window);

    SDL_Rect rect = {200, 200, 450, 300};

    SDL_FillRect(surface, &rect, SDL_MapRGB(surface->format, 0xFF, 0xFF, 0xFF));

    std::vector<std::vector<bool>> game_board(GAME_HEIGHT, std::vector<bool>(GAME_WIDTH, false));
    game_board[GAME_HEIGHT / 2][GAME_WIDTH / 2] = true;
    game_board[(GAME_HEIGHT / 2) - 1][GAME_WIDTH / 2] = true;
    game_board[(GAME_HEIGHT / 2) + 1][GAME_WIDTH / 2] = true;
    game_board[GAME_HEIGHT / 2 + 1][GAME_WIDTH / 2 + 1] = true;
    game_board[GAME_HEIGHT / 2][GAME_WIDTH / 2 - 1] = true;



    SDL_Event event;
    bool quit = false;

    Uint32 last_update = SDL_GetTicks();
    const Uint32 delay = 250;
    draw_board(surface, game_board);
    SDL_UpdateWindowSurface(window);
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
            }
        }

        Uint32 current_time = SDL_GetTicks();

        if (current_time - last_update >= delay) {
            last_update = current_time;
            draw_board(surface, game_board);
            game_board = change_board(game_board);
            SDL_UpdateWindowSurface(window);
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}