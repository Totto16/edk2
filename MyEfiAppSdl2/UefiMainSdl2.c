#include "/home/totto/Code/coder2k/oopetris_pr5/temp/edk2/OvmfPkg/Library/PlatformDebugLibIoPort/DebugLibDetect.h"

#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Uefi.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "./Color.h"

#include "SDL.h"

// target 60 FPS
// -> unlimited => 0
#define FPS 60

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

static bool rand_bool(void) {
    return (rand() & 0x01) != 0;
}

[[maybe_unused]] void displayFPS(SDL_Renderer* renderer, double fps) {
    //TODO: display it on the top corner
    SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "FPS: %.2f", fps);
}

typedef void* (*Sdl2RenderExampleModeInitData)();

typedef void (*Sdl2RenderExampleModeResetData)(void*);

typedef bool (*Sdl2RenderExampleModeRender)(SDL_Renderer* renderer, void* data);

typedef struct {
    void* data;
    Sdl2RenderExampleModeInitData init_data;
    Sdl2RenderExampleModeResetData reset_data;
    Sdl2RenderExampleModeRender render;
} Sdl2RenderExampleMode;

#define RECT_WIDTH_EXAMPLE1 200
#define RECT_HEIGHT_EXAMPLE1 100

#define MOVEMENT_DX_EXAMPLE1 20
#define MOVEMENT_DY_EXAMPLE1 20

typedef struct {
    SDL_Rect rect;
    int dx;
    int dy;
    Uint64 freq;
    Uint64 start_counter;
} Sdl2RenderExample1Data;

void Sdl2RenderExample1_reset_data(void* _data) {
    Sdl2RenderExample1Data* data = (Sdl2RenderExample1Data*) _data;


    data->rect = (SDL_Rect){ (SCREEN_WIDTH - RECT_HEIGHT_EXAMPLE1) / 2, (SCREEN_HEIGHT - RECT_HEIGHT_EXAMPLE1) / 2,
                             RECT_WIDTH_EXAMPLE1, RECT_HEIGHT_EXAMPLE1 };


    data->dx = rand_bool() ? -MOVEMENT_DX_EXAMPLE1 : MOVEMENT_DX_EXAMPLE1;
    data->dy = rand_bool() ? -MOVEMENT_DY_EXAMPLE1 : MOVEMENT_DY_EXAMPLE1;

    data->freq = SDL_GetPerformanceFrequency();
    data->start_counter = SDL_GetPerformanceCounter();
}


void* Sdl2RenderExample1_init_data(void) {

    Sdl2RenderExample1Data* data = SDL_malloc(sizeof(Sdl2RenderExample1Data));

    if (data == NULL) {
        return NULL;
    }

    Sdl2RenderExample1_reset_data(data);

    return data;
}

bool Sdl2RenderExample1_render(SDL_Renderer* renderer, void* _data) {

    Sdl2RenderExample1Data* data = (Sdl2RenderExample1Data*) _data;

    Uint64 raw_counter = SDL_GetPerformanceCounter();

    Uint64 counter = raw_counter - data->start_counter;

    const double h = fmod((((double) counter) / (double) data->freq) * COLOR_PROGRESS_PER_SECOND, 360.0);

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


    data->rect.x += data->dx;
    data->rect.y += data->dy;

    // Bounce horizontally
    if (data->rect.x <= 0 || data->rect.x + data->rect.w >= SCREEN_WIDTH) {
        data->dx = -data->dx;
    }

    // Bounce vertically
    if (data->rect.y <= 0 || data->rect.y + data->rect.h >= SCREEN_HEIGHT) {
        data->dy = -data->dy;
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

    SDL_RenderFillRect(renderer, &data->rect);

    return false;
}

//TODO
/* int b() {
    // SDL_SetRenderDrawColor(renderer, 255 / 4, (255 / 4) * 2, (255 / 4) * 3, 0xFF);

    // SDL_RenderFillRect(renderer, &rect);
} */

#define MODES_SIZE 1
static Sdl2RenderExampleMode g_modes[] = {
    (Sdl2RenderExampleMode){
                            .data = NULL,
                            .init_data = Sdl2RenderExample1_init_data,
                            .reset_data = Sdl2RenderExample1_reset_data,
                            .render = Sdl2RenderExample1_render,
                            }
};

SDL_COMPILE_TIME_ASSERT(g_modes, SDL_arraysize(g_modes) == MODES_SIZE);

static uint8_t g_current_mode_idx = 0;

Sdl2RenderExampleMode* setup_mode(uint8_t idx) {

    ASSERT(idx >= 0 && idx < MODES_SIZE);

    Sdl2RenderExampleMode* mode = &(g_modes[idx]);

    ASSERT(mode->data == NULL);

    ASSERT(mode->init_data != NULL);
    ASSERT(mode->render != NULL);
    // reset_data might be NULL

    mode->data = mode->init_data();

    ASSERT(mode->data != NULL);

    g_current_mode_idx = idx;

    return mode;
}

void reset_mode(Sdl2RenderExampleMode* mode) {

    ASSERT(mode->data != NULL);

    SDL_free(mode->data);

    mode->data = NULL;
}

bool render_mode(Sdl2RenderExampleMode* mode, SDL_Renderer* renderer) {
    ASSERT(mode->data != NULL);

    return mode->render(renderer, mode->data);
}

bool reset_mode_data(Sdl2RenderExampleMode* mode) {

    if (mode->reset_data == NULL) {
        return false;
    }

    mode->reset_data(mode->data);
    return true;
}


int sdl2_main(void) {

    srand(time(NULL));

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

    Sdl2RenderExampleMode* current_mode = setup_mode(0);

    SDL_Event event = {};

    bool quit = false;

    while (!quit) {
        while (SDL_PollEvent(&event) != 0) {

            switch (event.type) {
                case SDL_QUIT:
                    SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "Quitting");
                    quit = true;
                    break;
                case SDL_MOUSEMOTION:
                    // SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "SDL_MOUSEMOTION event: %d", event.type);
                    break;
                case SDL_MOUSEWHEEL:
                    //SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "SDL_MOUSEWHEEL event: %d", event.type);
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    // SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "SDL_MOUSEBUTTONDOWN event: %d", event.type);
                    break;
                case SDL_MOUSEBUTTONUP:
                    // SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "SDL_MOUSEBUTTONUP event: %d", event.type);
                    break;
                case SDL_KEYDOWN:
                    SDL_KeyboardEvent key_event = event.key;
                    //  SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "SDL_KEYDOWN event: %d", key_event.keysym.sym);
                    if (key_event.keysym.sym >= '0' && key_event.keysym.sym <= '9') {
                        uint8_t mode_idx = key_event.keysym.sym - '0';

                        if (mode_idx >= 0 && mode_idx < MODES_SIZE && g_current_mode_idx != mode_idx) {
                            reset_mode(current_mode);
                            current_mode = setup_mode(mode_idx);
                        }
                    } else if (key_event.keysym.sym == 'r') {
                        reset_mode_data(current_mode);
                    } else if (key_event.keysym.sym == 27) {
                        //ESC
                        SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "ESC: Quitting");
                        quit = true;
                    }
                    break;
                case SDL_KEYUP:
                    //  SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "SDL_KEYUP event: %d", event.type);
                    break;
                case SDL_TEXTINPUT:
                    //  SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "SDL_TEXTINPUT event: %d", event.type);
                    break;
                default:
                    // SDL_LogVerbose(SDL_LOG_CATEGORY_APPLICATION, "Unkown SDL event: %d", event.type);
                    break;
            }
        }

        bool render_quit = render_mode(current_mode, renderer);
        if (render_quit) {
            quit = true;
        }


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

    reset_mode(current_mode);

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
