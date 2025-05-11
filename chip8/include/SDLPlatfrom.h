#pragma once

#include <SDL3/SDL.h>
#include <unordered_map>
#include <iostream>

class SDL_Platform
{
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;

    std::unordered_map<SDL_Keycode, uint8_t> keymap = {
        {SDLK_1, 0x1}, {SDLK_2, 0x2}, {SDLK_3, 0x3}, {SDLK_4, 0xC},
        {SDLK_Q, 0x4}, {SDLK_W, 0x5}, {SDLK_E, 0x6}, {SDLK_R, 0xD},
        {SDLK_A, 0x7}, {SDLK_S, 0x8}, {SDLK_D, 0x9}, {SDLK_F, 0xE},
        {SDLK_Z, 0xA}, {SDLK_X, 0x0}, {SDLK_C, 0xB}, {SDLK_V, 0xF}
    };

public:
    SDL_Platform(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight)
    {
        if(!SDL_Init(SDL_INIT_VIDEO))
        {
            std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
            return;
        }

        window = SDL_CreateWindow(title, windowWidth, windowHeight, 0);
        if (!window)
        {
            std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
            return;
        }
        renderer = SDL_CreateRenderer(window, "software");
        if (!renderer)
        {
            std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << std::endl;
            return;
        }
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, textureWidth, textureHeight);
        if (!texture)
        {
            std::cerr << "SDL_CreateTexture failed: " << SDL_GetError() << std::endl;
            return;
        }
        SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_NONE);
    }

    ~SDL_Platform()
    {
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    void Update(void const* buffer, int pitch)
    {
        SDL_UpdateTexture(texture, nullptr, buffer, pitch);
        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }

    bool ProcessInput(uint8_t* keys)
    {
        bool quit = false;

        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_EVENT_QUIT:
                quit = true;
                break;
                case SDL_EVENT_KEY_DOWN:
                case SDL_EVENT_KEY_UP:
                {
                    bool isPressed = (event.type == SDL_EVENT_KEY_DOWN);
                    SDL_Keycode code = event.key.key;

                    if (code == SDLK_ESCAPE)
                    {
                        quit = true;
                        break;
                    }

                    auto it = keymap.find(code);
                    if (it != keymap.end())
                    {
                        keys[it->second] = isPressed ? 1 : 0;
                    }
                    break;
                }
                break;
            }
        }
        return quit;
    }

};