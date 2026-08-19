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
// -> unlimited => 0
#define FPS 60

#define MOVEMENT_DX 20
#define MOVEMENT_DY 20

#define NANOSECONDS(x) ((x) * 1000000000ULL)

#define COLOR_PROGRESS_PER_SECOND 36.0

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720


#include <UEfiTimeSupport.h>

// C++ like functions

#ifdef __cplusplus
#error "TODO"
#else

#include <errno.h>

static uint64_t std_chrono_steady_clock_now(void) {
    return 0;
    struct timespec ts;
    int res = clock_gettime(CLOCK_MONOTONIC, &ts);
    ASSERT(res == 0);

    uint64_t nanoseconds = (uint64_t) ts.tv_sec * NANOSECONDS(1) + ts.tv_nsec;

    return nanoseconds;
}

static bool helper_sleep_nanoseconds(uint64_t nano_seconds) {
    int result = 0;
    struct timespec remaining = {};
    struct timespec current = (struct timespec){
        .tv_sec = nano_seconds / NANOSECONDS(1),
        .tv_nsec = nano_seconds % NANOSECONDS(1),
    };

    do { // NOLINT(cppcoreguidelines-avoid-do-while)
        result = nanosleep(&current, &remaining);

        if (result == 0) {
            return true;
        }

        if (errno != EINTR) {
            return false;
        }

        current = remaining;
    } while (true);
}

#endif


static uint64_t get_sleep_time(uint64_t target_framerate) {
    if (target_framerate == 0) {
        return 0;
    }
    return NANOSECONDS(1) / target_framerate;
}

[[maybe_unused]] void displayFPS(SDL_Renderer* renderer, double fps) {
    //TODO: display it on the top corner
    SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "FPS: %.2f", fps);
}

int sdl2_main(void) {

    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);

    int result = SDL_Init(SDL_INIT_VIDEO);

    if (result != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        return 3;
    }

    SDL_Window* window = SDL_CreateWindow(
            "This title is never shown", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT,
            SDL_WINDOW_SHOWN
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

    const Uint64 freq = SDL_GetPerformanceFrequency();

    const uint64_t target_framerate = FPS;

    const uint64_t sleep_time = get_sleep_time(target_framerate);

    uint64_t start_execution_time = std_chrono_steady_clock_now();

#if !defined(NDEBUG)
    uint64_t start_time = SDL_GetPerformanceCounter();
    uint64_t frame_counter = 0;
    const uint64_t update_time = freq / 2; //0.5 s;
    const double count_per_s = (double) freq;
#endif

    const int RECT_WIDTH = 200;
    const int RECT_HEIGHT = 100;

    SDL_Rect rect = { (SCREEN_WIDTH - RECT_WIDTH) / 2, (SCREEN_HEIGHT - RECT_HEIGHT) / 2, RECT_WIDTH, RECT_HEIGHT };

    int dx = MOVEMENT_DX;
    int dy = MOVEMENT_DY;

    SDL_Event event = {};

    while (true) {
        SDL_PollEvent(&event);
        if (event.type == SDL_QUIT) {
            break;
        }


        Uint64 counter = SDL_GetPerformanceCounter();

        const double h = fmod((((double) counter) / (double) freq) * COLOR_PROGRESS_PER_SECOND, 360.0);

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


        rect.x += dx;
        rect.y += dy;

        // Bounce horizontally
        if (rect.x <= 0 || rect.x + rect.w >= SCREEN_WIDTH) {
            dx = -dx;
        }

        // Bounce vertically
        if (rect.y <= 0 || rect.y + rect.h >= SCREEN_HEIGHT) {
            dy = -dy;
        }
        hsv rect_orig_color = (hsv){
            .h = fmod(h + 180.0, 360.0),
            .s = 1.0,
            .v = 1.0,
        };
        rgb final_rect_color = hsv2rgb(rect_orig_color);
        SDL_SetRenderDrawColor(
                renderer, (Uint8) (final_rect_color.r * 255.0), (Uint8) (final_rect_color.g * 255.0),
                (Uint8) (final_rect_color.b * 255.0), 0xFF
        );

        SDL_RenderFillRect(renderer, &rect);
        // SDL_SetRenderDrawColor(renderer, 255 / 4, (255 / 4) * 2, (255 / 4) * 3, 0xFF);

        // SDL_RenderFillRect(renderer, &rect);


#if !defined(NDEBUG)
        frame_counter++;

        const Uint64 current_time = SDL_GetPerformanceCounter();

        if (current_time - start_time >= update_time) {
            const double elapsed = (double) (current_time - start_time) / count_per_s;

            displayFPS(renderer, (double) (frame_counter) / elapsed);

            start_time = current_time;
            frame_counter = 0;
        }
#endif

        SDL_RenderPresent(renderer);

        if (target_framerate != 0) {

            const uint64_t now = std_chrono_steady_clock_now();
            const uint64_t runtime = (now - start_execution_time);

            if (runtime < sleep_time) {
                //TODO(totto): use SDL_DelayNS in sdl >= 3.0
                bool sleep = helper_sleep_nanoseconds(sleep_time - runtime);
                ASSERT(sleep);
                start_execution_time = std_chrono_steady_clock_now();
            } else {
                start_execution_time = now;
            }
        }
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
