
// #------------------------------#
// |          includes            |
// #------------------------------#

// external lib
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl2.h"
#include <SDL.h>
#include <SDL_opengl.h>

// c/c++ includes

// my includes
#include "display/mock/imgui_kernel.hpp"
#include "buttons/buttons.h"
#include "speedometer/speedometer.hpp"

// #------------------------------#
// |           macros             |
// #------------------------------#
#define WINDOW_HORIZONTAL_RES 1080
#define WINDOW_VERTICAL_RES 1920

// #------------------------------#
// | global variables definitions |
// #------------------------------#

// #------------------------------#
// | static variables definitions |
// #------------------------------#
// color of background color
const ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

/**
 * @brief Struct for return values from imgui setup
 *
 */
struct Imgui_Context {
    ImGuiIO& io;
    SDL_Window* window;
    SDL_GLContext gl_context;
};

// #------------------------------#
// | static functions declarations|
// #------------------------------#
/**
 * @brief Used for rendering and handling buttons that mocks real buttons
 *
 * @param btn button class
 * @param label label rendered on gui
 */
void handle_button(Button& btn, const char* label);
/**
 * @brief Setup function for imgui. Setup all needed data and returns io,
 * sdl window and sdl gl context
 *
 * @return Imgui_Context* struct with packed data
 */
static Imgui_Context* imgui_setup();
/**
 * @brief renders display of bike computer
 *
 * @param window gl window
 * @param imgui_display_data img to display
 * @param scale integer scale for increasing size
 */
static void imgui_render(SDL_Window* window, Imgui_Display_Data& imgui_display_data, const int scale);

// #------------------------------#
// | global function definitions  |
// #------------------------------#
void imgui_thread_kernel(void* args)
{
    Imgui_Display_Data* imgui_display_data = static_cast<Imgui_Display_Data*>(args);
    auto imgui_context = imgui_setup();
    if (imgui_context == nullptr) {
        return;
    }
    ImGuiIO& io = imgui_context->io;
    SDL_Window* window = imgui_context->window;
    SDL_GLContext gl_context = imgui_context->gl_context;

    // used this to return values from setup, it is not needed anymore
    delete imgui_context;

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;

    // Main loop
    int scale = 3;
    float speed = 20.0f;
    float cadence = 80.0f;
    while (!imgui_display_data->stop)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                imgui_display_data->stop = true;
            if (event.type == SDL_WINDOWEVENT &&
                event.window.event == SDL_WINDOWEVENT_CLOSE &&
                event.window.windowID == SDL_GetWindowID(window))
                {
                    imgui_display_data->stop = true;
                    exit(0);
                }
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Control panel");
            ImGui::Text("Use to interact with device.");
            ImGui::Text("Buttons from the device");
            {
                ImGui::Columns(2, nullptr);
                handle_button(btn0, "Top Left");
                ImGui::NextColumn();
                handle_button(btn1, "Top Right");
                ImGui::NextColumn();
                handle_button(btn3, "Bottom Left");
                ImGui::NextColumn();
                handle_button(btn2, "Bottom Right");
            }
            ImGui::Columns(1);

            ImGui::Text("Set simulate values");
            ImGui::SliderFloat("speed", &speed, 0.0f, 40.0f);

            ImGui::Text("Set window values");
            ImGui::SliderInt("Display size", &scale, 1, 5);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        // If values changes, change simulate speed value
        static float speed_prev = 0.0f;
        if (speed_prev != speed){
            speed_emulate(speed);
            speed_prev = speed;
        }
        imgui_render(window, *imgui_display_data, scale);
    }

    // Cleanup
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
// #------------------------------#
// | static functions definitions |
// #------------------------------#

static Imgui_Context* imgui_setup(void)
{
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return nullptr;
    }

    // From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

    // Setup window
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("Bike computer host sim", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,  WINDOW_VERTICAL_RES, WINDOW_HORIZONTAL_RES, window_flags);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL2_Init();

    return new Imgui_Context{io, window, gl_context};
}

static void imgui_render(SDL_Window* window, Imgui_Display_Data& imgui_display_data, const int scale)
{
    const uint window_width = DISPLAY_WIDTH * scale;
    const uint window_height = DISPLAY_HEIGHT * scale;
    const uint window_offset_x = 10;
    const uint window_offset_y = 10;

    auto io = ImGui::GetIO();

    ImGui::Render();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);

    // Set up the orthographic projection matrix
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, io.DisplaySize.x, io.DisplaySize.y, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    static display::DisplayColor imgui_display_buffer[DISPLAY_PIXEL_COUNT];

    imgui_display_data.mtx_buffer.lock();
    memcpy(imgui_display_buffer, imgui_display_data.imgui_display_buffer, sizeof(display::DisplayColor) * DISPLAY_PIXEL_COUNT);
    imgui_display_data.mtx_buffer.unlock();

    // Render a 360x240 pixel array with RGB colors
    for (int y = 0; y < DISPLAY_HEIGHT; ++y) {
        for (int x = 0; x < DISPLAY_WIDTH; ++x) {
            size_t idx = y * DISPLAY_WIDTH + x;
            assert(idx < DISPLAY_PIXEL_COUNT);
            display::DisplayColor val = (imgui_display_buffer[idx]);

            glColor4f((float)val.r, (float)val.g, (float)val.b, 1.0f);  // RGB color
            // glColor4f(x / (float)window_width, y /(float)window_height, 0.0f, 1.0f);  // RGB color
            glRectf((x*scale) + window_offset_x, (y * scale) + window_offset_y, ( (x +1) * scale) + window_offset_x,  ((y + 1) * scale) + window_offset_y);  // Render a single pixel
        }
    }

    // Restore the previous matrix state
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);


    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
    //glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context where shaders may be bound
    SDL_GL_SwapWindow(window);
}

void handle_button(Button& btn, const char* label)
{
    if(ImGui::Button(label))
    {
        btn.on_call_press();
        sleep_ms(100);
        btn.on_call_release();
    }
}
