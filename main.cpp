#include <SDL2/SDL_locale.h>
#include <SDL2/SDL.h>
#include <locale.h>
#include <stdio.h>

bool isSdlError()
{
    const char *error = SDL_GetError();
    return (*error != '\0');
}

static SDL_Window *m_window = NULL;
static SDL_Renderer *m_gRenderer = NULL;
static SDL_Texture  *m_tBuffer = NULL;

int ScaleWidth = 800;
int ScaleHeight = 600;

#define TEXTURE_TARGET_ORDER_VALID

void clearBuffer()
{
    SDL_SetRenderDrawColor(m_gRenderer, 0, 0, 0, 255);
    SDL_RenderClear(m_gRenderer);
}

void freeSDL()
{
    if(m_tBuffer)
        SDL_DestroyTexture(m_tBuffer);
    m_tBuffer = nullptr;

    if(m_gRenderer)
        SDL_DestroyRenderer(m_gRenderer);
    m_gRenderer = NULL;

    if(m_window)
        SDL_DestroyWindow(m_window);
    m_window = NULL;

    SDL_Quit();
}

extern "C"
int main(int argc, char**argv)
{
    int res;
    bool works = true;
    SDL_Event m_event;
    float val = 0.f;
    float sign = +1.f;

#if defined(__ANDROID__) || (defined(__APPLE__) && (defined(TARGET_IPHONE_SIMULATOR) || defined(TARGET_OS_IPHONE)))
    // Restrict the landscape orientation only
    SDL_SetHint(SDL_HINT_ORIENTATIONS, "LandscapeLeft LandscapeRight");
#endif

#if defined(__ANDROID__)
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 6);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
#endif

    Uint32 sdlInitFlags = 0;
    // Prepare flags for SDL initialization
#ifndef __EMSCRIPTEN__
    sdlInitFlags |= SDL_INIT_TIMER;
#endif
    sdlInitFlags |= SDL_INIT_AUDIO;
    sdlInitFlags |= SDL_INIT_VIDEO;
    sdlInitFlags |= SDL_INIT_EVENTS;
    sdlInitFlags |= SDL_INIT_JOYSTICK;
    sdlInitFlags |= SDL_INIT_HAPTIC;
    sdlInitFlags |= SDL_INIT_GAMECONTROLLER;

    // Initialize SDL
    res = (SDL_Init(sdlInitFlags) < 0);

    // Workaround: https://discourse.libsdl.org/t/26995
    setlocale(LC_NUMERIC, "C");

    const char *error = SDL_GetError();
    if(*error != '\0')
    {
        printf("Error while SDL Initialization: %s", error);
        fflush(stdout);
    }
    SDL_ClearError();

    SDL_GL_ResetAttributes();

    m_window = SDL_CreateWindow("kokokok",
                         SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED,
                         800, 600,
                         SDL_WINDOW_RESIZABLE |
                         SDL_WINDOW_HIDDEN |
                         SDL_WINDOW_ALLOW_HIGHDPI);

    if(m_window == nullptr)
    {
        printf("Unable to create window!");
        fflush(stdout);
        SDL_ClearError();
        freeSDL();
        return 1;
    }

    if(isSdlError())
    {
        printf("Unable to create window!");
        fflush(stdout);
        SDL_ClearError();
        freeSDL();
        return 1;
    }



    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

#if defined(__ANDROID__) // Use a full-screen on Android mode by default
    SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
#endif

    SDL_ShowWindow(m_window);

    m_gRenderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

    if(!m_gRenderer)
    {
        printf("Unable to create renderer!");
        fflush(stdout);
        freeSDL();
        return 1;
    }

    m_tBuffer = SDL_CreateTexture(m_gRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 800, 600);
    if(!m_tBuffer)
    {
        SDL_Log("Unable to create texture render buffer!");
        freeSDL();
        return 2;
    }

    // Clean-up from a possible start-up junk
    clearBuffer();
    SDL_SetRenderTarget(m_gRenderer, m_tBuffer);
    SDL_SetRenderDrawBlendMode(m_gRenderer, SDL_BLENDMODE_BLEND);
    clearBuffer();

#ifdef TEXTURE_TARGET_ORDER_VALID
    SDL_SetRenderTarget(m_gRenderer, NULL);
#endif

    while(works)
    {
        while(SDL_PollEvent(&m_event))
        {
            switch(m_event.type)
            {
            case SDL_QUIT:
                works = false;
                break;
            case SDL_KEYDOWN:
                break;
            case SDL_KEYUP:

                switch(m_event.key.keysym.scancode)
                {
                case SDL_SCANCODE_ESCAPE:
                case SDL_SCANCODE_AC_BACK:
                    works = false;
                    break;
                default:
                    break;
                }

                break;
        #ifdef __ANDROID__
            case SDL_RENDER_DEVICE_RESET:
                SDL_Log("Android: Render Device Reset");
                break;
            case SDL_APP_WILLENTERBACKGROUND:
                SDL_Log("Android: Entering background");
                break;
            case SDL_APP_DIDENTERFOREGROUND:
                SDL_Log("Android: Resumed foreground");
                break;
        #endif
            }
        }



        val += 0.01f * sign;
        if(val > 1.0f)
        {
            val = 1.0f;
            sign = -1.f;
        }
        else if(val < 0.0f)
        {
            val = 0.0f;
            sign = +1.f;
        }

#ifdef TEXTURE_TARGET_ORDER_VALID
        // Draw two squares on the texture
        SDL_SetRenderTarget(m_gRenderer, m_tBuffer);
#endif

        SDL_SetRenderDrawColor(m_gRenderer, 0, 0, 0, 255);
        SDL_RenderClear(m_gRenderer);

        SDL_Rect aRect = {50,
                          50,
                          200, 200};
        SDL_SetRenderDrawColor(m_gRenderer,
                               static_cast<unsigned char>(255.f * val),
                               static_cast<unsigned char>(255.f * 0.5f),
                               static_cast<unsigned char>(255.f * 0.5f),
                               static_cast<unsigned char>(255.f * 0.9f)
                              );
        SDL_RenderFillRect(m_gRenderer, &aRect);

        aRect = {300,
                 50,
                 200, 200};
                SDL_SetRenderDrawColor(m_gRenderer,
                                       static_cast<unsigned char>(255.f * (1.f - val)),
                                       static_cast<unsigned char>(255.f * 0.5f),
                                       static_cast<unsigned char>(255.f * 0.5f),
                                       static_cast<unsigned char>(255.f * 0.9f)
                                      );
                SDL_RenderFillRect(m_gRenderer, &aRect);

        int w, h, off_x, off_y, wDst, hDst;
        float scale_x, scale_y;

        SDL_SetRenderTarget(m_gRenderer, nullptr);

        // Get the size of surface where to draw the scene
        SDL_GetRendererOutputSize(m_gRenderer, &w, &h);
        // Calculate the size difference factor
        scale_x = float(w) / ScaleWidth;
        scale_y = float(h) / ScaleHeight;

        wDst = w;
        hDst = h;

        // Keep aspect ratio
        if(scale_x > scale_y) // Width more than height
        {
            wDst = int(scale_y * ScaleWidth);
            hDst = int(scale_y * ScaleHeight);
        }
        else if(scale_x < scale_y) // Height more than width
        {
            hDst = int(scale_x * ScaleHeight);
            wDst = int(scale_x * ScaleWidth);
        }

        // Align the rendering scene to the center of screen
        off_x = (w - wDst) / 2;
        off_y = (h - hDst) / 2;

        SDL_SetRenderDrawColor(m_gRenderer, 0, 0, 0, 255);
        SDL_RenderClear(m_gRenderer);

        SDL_Rect destRect = {off_x, off_y, wDst, hDst};
        SDL_Rect sourceRect = {0, 0, ScaleWidth, ScaleHeight};

        // Draw the texture on the screen
        SDL_SetTextureColorMod(m_tBuffer, 255, 255, 255);
        SDL_SetTextureAlphaMod(m_tBuffer, 255);
        SDL_RenderCopyEx(m_gRenderer, m_tBuffer, &sourceRect, &destRect, 0.0, nullptr, SDL_FLIP_NONE);



        // Draw the square on the screen itself
        aRect = {10,
                  10,
                  50, 50};
        SDL_SetRenderDrawColor(m_gRenderer,
                               static_cast<unsigned char>(255.f * 0.5f),
                               static_cast<unsigned char>(255.f * val),
                               static_cast<unsigned char>(255.f * 0.5f),
                               static_cast<unsigned char>(255.f * 0.9f)
        );
        SDL_RenderFillRect(m_gRenderer, &aRect);

#ifndef TEXTURE_TARGET_ORDER_VALID
        // Return back the texture renderer
        SDL_SetRenderTarget(m_gRenderer, m_tBuffer);
#endif

        // Refresh the screen
        SDL_RenderPresent(m_gRenderer);

        // Wait
        SDL_Delay(15);
    }

    freeSDL();

    return 0;
}
