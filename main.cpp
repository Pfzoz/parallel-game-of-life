#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <iostream>
#include <vector>
#include <omp.h>

int SCREEN_WIDTH = 1000, SCREEN_HEIGHT = 1000;
int GAME_WIDTH = 300, GAME_HEIGHT = 300;
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
    Uint64 start = SDL_GetTicks64();
    #pragma omp parallel for
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
    Uint64 end = SDL_GetTicks64();
    std::cout << "Took: " << end - start << "ms" << std::endl;
    return new_board;
}

int main()
{
    if (!init_SDL()) {
        std::cout << "Could not initialize SDL. SDL_Error:" << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Conway's Game of Life - PAUSADO (Use Espaço e Mouse)", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Surface *surface = SDL_GetWindowSurface(window);

    std::vector<std::vector<bool>> game_board(GAME_HEIGHT, std::vector<bool>(GAME_WIDTH, false));

    SDL_Event event;
    bool quit = false;

    Uint32 last_update = SDL_GetTicks();
    Uint32 delay = 250;
    draw_board(surface, game_board);
    SDL_UpdateWindowSurface(window);
    bool running = false;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    int x, y;
                    SDL_GetMouseState(&x, &y);
                    std::cout << "X: " << x / 10 << " Y: " << y / 10 << std::endl;
                    if (x > 0 && x < SCREEN_WIDTH && y > 0 && y < SCREEN_HEIGHT) {
                        game_board[y / CELL_HEIGHT][x / CELL_WIDTH] = !game_board[y / CELL_HEIGHT][x / CELL_WIDTH];
                    }
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_SPACE) {
                        std::cout << "SPACE!" << std::endl;
                        running = !running;
                        SDL_SetWindowTitle(window, !running ? "Conway's Game of Life - PAUSADO (Use Espaço e Mouse)" : "Conway's Game of Life  (Use Espaço e Mouse)");
                    }
                    if (event.key.keysym.sym == SDLK_DOWN) {
                        delay -= delay > 0 ? 10 : 0;
                        std::cout << "Delay: " << delay << std::endl;
                    }
                    if (event.key.keysym.sym == SDLK_UP) {
                        delay += delay < 250 ? 10 : 0;
                        std::cout << "Delay: " << delay << std::endl;
                    }
                    if (event.key.keysym.sym == SDLK_r) {
                        game_board = std::vector<std::vector<bool>>(GAME_HEIGHT, std::vector<bool>(GAME_WIDTH, false));
                    }
                    break;
            }
        }

        Uint32 current_time = SDL_GetTicks();

        if (current_time - last_update >= delay && running) {
            game_board = change_board(game_board);
            last_update = current_time;
        }

        draw_board(surface, game_board);
        SDL_UpdateWindowSurface(window);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
