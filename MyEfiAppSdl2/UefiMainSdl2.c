#include "/home/totto/Code/coder2k/oopetris_pr5/temp/edk2/OvmfPkg/Library/PlatformDebugLibIoPort/DebugLibDetect.h"

#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Uefi.h>

#include <stdbool.h>
#include <stdio.h>

#include "./Color.h"

#include "SDL.h"

// target 60 FPS
#define FPS 60

#define COLOR_PROGRESS_PER_SECOND 10.0

int sdl2_main(void) {

    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);

    int result = SDL_Init(SDL_INIT_VIDEO);

    if (result != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        return 3;
    }


    SDL_Window* window = SDL_CreateWindow(
            "This title is never shown", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_SHOWN
    );

    if (window == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window: %s", SDL_GetError());
        return 4;
    }


    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

    if (renderer == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create renderer: %s", SDL_GetError());
        return 5;
    }

    Uint64 freq = SDL_GetPerformanceFrequency();

    SDL_Event event = {};

    while (true) {
        SDL_PollEvent(&event);
        if (event.type == SDL_QUIT) {
            break;
        }


        Uint64 counter = SDL_GetPerformanceCounter();

        // fmod is slow on this platform, so try to use another method to get the same value, H is not that different in off by one cases
        const double h = fmod(((double) counter / (double) freq) * COLOR_PROGRESS_PER_SECOND, 360.0);

        hsv orig_color = (hsv){
            .h = h,
            .s = 1.0,
            .v = 1.0,
        };
        rgb final_color = hsv2rgb(orig_color);

        SDL_SetRenderDrawColor(
                renderer, //
                (Uint8) (final_color.r * 255.0), (Uint8) (final_color.g * 255.0), (Uint8) (final_color.b * 255.0), 0xFF
        );
        SDL_RenderClear(renderer);

        // flip buffers, write framebuffer to screen, doesn't use vsync
        SDL_RenderPresent(renderer);


        //TODO: use better timing and measure instead fo just using the fixed value
        SDL_Delay(1000 / FPS);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}

const char* EFIAPI bool_string(bool value) {
    return value ? "true" : "false";
}

/***
  Demonstrates basic workings of the main() function by displaying a
  welcoming message.

  Note that the UEFI command line is composed of 16-bit UCS2 wide characters.
  The easiest way to access the command line parameters is to cast Argv as:
      wchar_t **wArgv = (wchar_t **)Argv;

  @param[in]  Argc    Number of argument tokens pointed to by Argv.
  @param[in]  Argv    Array of Argc pointers to command line tokens.

  @retval  0         The application exited normally.
  @retval  Other     An error occurred.
***/
int main(IN int Argc, IN char** Argv) {

    DEBUG((DEBUG_ERROR, "[error] HELLO WORLD.\n"));
    DEBUG((DEBUG_INFO, "[info] HELLO WORLD.\n"));
    DEBUG((DEBUG_VERBOSE, "[verbose] HELLO WORLD.\n"));
    DEBUG((DEBUG_WARN, "[warn] HELLO WORLD.\n"));


    bool plat_detected = PlatformDebugLibIoPortDetect();

    bool debug_print_enabled = DebugPrintEnabled();

    Print(L"Hello from UEFI!: plat_debug: %a debug: %a\r\n", bool_string(plat_detected),
          bool_string(debug_print_enabled));

    // this should happend by some constructor of the lib "UefiBootServicesTableLib"
    // gST = sysTable;
    // gBS = sysTable->BootServices;
    //gImageHandle = imgHandle;

    Print(L"st %p bs %p imgH: %p\r\n", gST, gBS, gImageHandle);
    ASSERT(gST != NULL);
    ASSERT(gBS != NULL);
    ASSERT(gImageHandle != NULL);


    fprintf(stderr, "stderr print\r\n");
    fflush(stderr);
    printf("stdout print: %d\r\n", 42);
    fflush(stdout);

    DEBUG((DEBUG_WARN, "starting sdl2 example\r\n"));
    int result = sdl2_main();
    DEBUG((DEBUG_ERROR, "SDL2 result: %d\r\n", result));

    return result;
}
