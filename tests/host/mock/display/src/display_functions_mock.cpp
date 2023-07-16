
// #------------------------------#
// |          includes            |
// #------------------------------#
// pico includes
#include "display/display_config.hpp"
#include "pico/time.h"

// external lib
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl2.h"
#include <SDL.h>
#include <SDL_opengl.h>

// c/c++ includes
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <mutex>
#include <thread>


// my includes
#include "display_functions_mock.hpp"
#include "traces.h"
#include "display/driver.hpp"
#include "hw_functions.hpp"
#include "speedometer/speedometer.hpp"

// #------------------------------#
// |           macros             |
// #------------------------------#

// #------------------------------#
// | global variables definitions |
// #------------------------------#

// #------------------------------#
// | static variables definitions |
// #------------------------------#

/**
Holds data for imgui. Main program loads here data.
*/
struct Imgui_Display_Data{
    std::mutex mtx_buffer;
    display::DisplayColor imgui_display_buffer[DISPLAY_PIXEL_COUNT];
};
static Imgui_Display_Data imgui_display_data;
static std::thread* imgui_thread;


// #------------------------------#
// | static functions declarations|
// #------------------------------#
void imgui_thread_kernel(void);

// #------------------------------#
// | global function definitions  |
// #------------------------------#
// functions from display driver
void display_init(void)
{
    // PRINT("display_init");
    // do nothing
    memset(imgui_display_data.imgui_display_buffer, 0, sizeof(imgui_display_data.imgui_display_buffer));
    Imgui_Display::start();
}

void display_display(uint8_t display_buffer[DISPLAY_BUFFER_SIZE])
{
    (void)display_buffer;
    // PRINT("display_display");
    std::unique_lock<std::mutex> lock(imgui_display_data.mtx_buffer);
    // lock.lock();

    // imgui_display_data.mtx_buffer.lock();

    for(size_t idx = 0; idx < DISPLAY_PIXEL_COUNT; idx+=1)
    {
        struct pixel2
        {
            uint8_t rg;
            uint8_t br;
            uint8_t gb;

        };
        pixel2 *buffer_pixels = (pixel2 *)display_buffer;

        const uint_fast32_t index_pixels = idx >> 1;
        pixel2 *const pixel_p = &buffer_pixels[index_pixels];
        uint8_t r = 0;
        uint8_t g = 0;
        uint8_t b = 0;
        if (idx & 1)
        {
            r = pixel_p->br & 0x0f;
            g = pixel_p->gb & 0xf0;
            b = pixel_p->gb & 0x0f;
        }
        else
        {
            r = pixel_p->rg & 0xf0;
            g = pixel_p->rg & 0x0f;
            b = pixel_p->br & 0xf0;
        }
        imgui_display_data.imgui_display_buffer[idx] = {r,g,b};
    }
    // for(size_t i = 0; i < DISPLAY_BUFFER_SIZE; i+=3)
    // {
    //     assert(i/3 + 1 < DISPLAY_PIXEL_COUNT);
    //     assert(i/3 < DISPLAY_PIXEL_COUNT);

    //     // load 2 pixels
    //     uint8_t* pixels = &display_buffer[i];
    //     imgui_display_data.imgui_display_buffer[i/3] =
    //         display::DisplayColor{(uint8_t)(pixels[0] & 0xf0),
    //                               (uint8_t)(pixels[0] & 0x0f),
    //                               (uint8_t)(pixels[1] & 0xf0)};
    //     imgui_display_data.imgui_display_buffer[i/3 + 1] =
    //         display::DisplayColor{(uint8_t)(pixels[1] & 0x0f),
    //                               (uint8_t)(pixels[2] & 0xf0),
    //                               (uint8_t)(pixels[2] & 0x0f)};

    // }
    // imgui_display_data.mtx_buffer.unlock();

    // copy display_buffer to imgui data
}

void Imgui_Display::start(void)
{
    ::imgui_thread = new std::thread(imgui_thread_kernel);
}

void Imgui_Display::stop(void)
{
    // TODO
    ::imgui_thread->join();
}

// #------------------------------#
// | static functions definitions |
// #------------------------------#

void imgui_thread_kernel(void)
{
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return;
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
    SDL_Window* window = SDL_CreateWindow("Bike computer host sim", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1920, 1080, window_flags);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL2_Init();

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != nullptr);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    bool done = false;
    int scale = 3;
    float speed = 20.0f;
    float cadence = 80.0f;
    display::DisplayColor imgui_display_buffer[DISPLAY_PIXEL_COUNT] = {0};
    while (!done)
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT &&
                event.window.event == SDL_WINDOWEVENT_CLOSE &&
                event.window.windowID == SDL_GetWindowID(window))
                {
                    done = true;
                    exit(0);
                }
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderInt("Display size", &scale, 1, 3);
            ImGui::SliderFloat("speed", &speed, 0.0f, 40.0f);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }
        static float speed_prev = 0.0f;
        if (speed_prev != speed){
            speed_emulate(speed);
            speed_prev = speed;
        }
        const uint window_width = DISPLAY_WIDTH * scale;
        const uint window_height = DISPLAY_HEIGHT * scale;
        const uint window_offset_x = 10;
        const uint window_offset_y = 10;

        // ImGui::SetNextWindowSize(ImVec2(window_width, window_height), ImGuiCond_FirstUseEver);
        // ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
        // ImGui::Begin("Pixel Array Window", nullptr);

        // // Render a 10x10 pixel array with RGB colors
        // for (int y = 0; y < window_height; ++y) {
        //     for (int x = 0; x < window_width; ++x) {
        //         ImU32 color = IM_COL32(255 * x / window_width, 255 * y / window_height, 0, 255);  // RGB color
        //         ImVec2 pos = ImVec2(window_offset_x + x, window_offset_y + y);
        //         ImGui::GetWindowDrawList()->AddRectFilled(pos, ImVec2(pos.x + 1.0f, pos.y + 1.0f), color);
        //     }
        // }

        // ImGui::End();
        // Rendering
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

    // Cleanup
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
