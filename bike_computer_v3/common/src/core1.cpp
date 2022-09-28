#include "core1.h"
#include "core_utils.hpp"
#include "common_types.h"
#include "common_utils.hpp"
#include "common_data.hpp"
#include "common_actors.hpp"

#include "buttons/buttons.h"
#include "traces.h"
#include "sd_file.h"
#include "session.hpp"


// display
#include "display/print.h"
#include "gui/structure.hpp"
//veiws
#include "views/view.hpp"
// #include "views/display.h"

#include <string>
#include <iostream>

#include "pico/time.h"

#define FRAME_PER_SECOND 10
#define MINIMAL_TIME_BTN 300

// static variables
static Sensor_Data sensors_data_display = {0};
static Session_Data sessionDataDisplay;

static volatile uint32_t lastBtn1Press = 0;
static volatile uint32_t btnPressedCount = 0;

static volatile uint32_t last_btn2_press = 0;
static volatile uint32_t last_btn2_rel = 0;

static volatile bool change_state = 0;
static volatile bool stop = 0;



// static functions
static void incDisplay(void);
static void incDisplayRel(void);

static void change_state_irq_handler();
static void setup(void);
static int loop(void);

static void get_total_data(float& time, float& dist);
static void set_total_data(float time, float dist);



// global functions
void core1LaunchThread(void)
{
    setup();
    while (loop())
    {
    }
}

void Core1::handle_sig_total_update(const Signal &sig)
{
    float dist, time;
    // read from file dist and time
    get_total_data(time, dist);
    {
        auto payload = sig.get_payload<Sig_Core1_Total_Update*>();

        // update data
        dist += payload->ridden_dist;
        time += payload->ridden_time;
        delete payload;
    }

    {
        auto payload = new Sig_Core0_Set_Total();
        // update data
        payload->ridden_dist_total = dist;
        payload->ridden_time_total = time;
        Signal sig(SIG_CORE0_SET_TOTAL, payload);
        actor_core0.send_signal(sig);
    }

    // write to file
    set_total_data(time, dist);
}


//static functions definitions


//button
static void next_display()
{
    btnPressedCount++;
}

static void btn2_short_press()
{
    printf("change state\n");
    change_state = true;
}

static void btn2_long_press()
{
    stop = true;
}

static void setup(void)
{
    buttons_clear_all();
    // btn1.set_callback(next_display);
    // btn2.set_callback(btn2_short_press);
    // btn2.set_callback_long(btn2_long_press);

    interruptSetupCore1();

    sensors_data_display = sensors_data;


    // setup
    auto gui = Gui::get_gui(&sensors_data_display, &sessionDataDisplay);
    gui->refresh();
    // while (1)
    // {
    //     PRINTF("gui render done\n");
    //     sleep_ms(1000);
    // }

    // Display_init(&sensors_data_display, &sessionDataDisplay);

    // TODO move it to menu
    {
        const std::string config_file_name = "giant_trance.cfg";
        Sd_File config_file(config_file_name);
        auto content = config_file.read_all();

        auto payload = new Sig_Core0_Set_Config();
        payload->file_content = config_file.read_all();
        payload->file_name = config_file_name;
        Signal sig(SIG_CORE0_SET_CONFIG ,payload);
        actor_core0.send_signal(sig);
    }

    // update data on start
    {
        float dist = 0.0f, time = 0.0f;
        get_total_data(time, dist);
        auto payload = new Sig_Core0_Set_Total();
        // update data
        payload->ridden_dist_total = dist;
        payload->ridden_time_total = time;
        Signal sig(SIG_CORE0_SET_TOTAL, payload);
        actor_core0.send_signal(sig);
    }

}

static int loop(void)
{
    absolute_time_t frameStart = get_absolute_time();
    actor_core1.handle_all();
    // frame update
    {
        Gui::get_gui()->handle_buttons();


        // copy data
        mutex_enter_blocking(&sensorDataMutex);
        sensors_data_display = sensors_data;
        if (session_p)
            sessionDataDisplay = *session_p;
        // std::cout << "Core1: \n'"
        //      //<< sensors_data_display.clbs << "'\n'"
        //      << sensors_data_display.cipgsmloc << "'" << std::endl;

        mutex_exit(&sensorDataMutex);

        // if system state has changed execute proper code
        static SystemState last_system_state;
        if(sensors_data_display.current_state != last_system_state)
        {
            last_system_state = sensors_data_display.current_state;
            switch(sensors_data_display.current_state)
            {
                case SystemState::TURNED_ON:
                    // Display_set_main_display_type();
                    break;
                case SystemState::CHARGING:
                    // Display_set_charge_display_type();
                    break;
                case SystemState::ENDED:
                    // TODO ??
                case SystemState::PAUSED:
                case SystemState::RUNNING:
                default:
                    break;
            }
        }

        // if button has been pressed change view
        if(btnPressedCount > 0)
        {
            // TODO next display
            // Display_incDisplayType();
            btnPressedCount = 0;
        }

        // if pause btn was pressed change state
        if(change_state)
        {
            TRACE_DEBUG(2, TRACE_CORE_1, "pause btn pressed %d\n", change_state);
            change_state = false;
            mutex_enter_blocking(&sensorDataMutex);
            switch(sensors_data_display.current_state)
            {
                case SystemState::PAUSED:
                    {
                        // sensors_data_display.current_state = SystemState::AUTOSTART;
                        // session_p->cont();
                        Signal sig(SIG_CORE0_CONTINUE);
                        actor_core0.send_signal(sig);
                        TRACE_DEBUG(2, TRACE_CORE_1, "sending signal continue %d\n", (int) sig.get_sig_id());

                    }
                    break;
                case SystemState::AUTOSTART:
                case SystemState::RUNNING:
                    {
                        // sensors_data_display.current_state = SystemState::PAUSED;
                        // session_p->pause();
                        Signal sig(SIG_CORE0_PAUSE);
                        actor_core0.send_signal(sig);
                        TRACE_DEBUG(2, TRACE_CORE_1, "sending signal pause %d\n", (int) sig.get_sig_id());

                    }
                    break;
                case SystemState::TURNED_ON:
                    {
                        // sensors_data_display.current_state = SystemState::AUTOSTART;
                        // session_p->start(sensors_data_display.current_time);
                        Signal sig(SIG_CORE0_SESION_START);
                        actor_core0.send_signal(sig);
                        TRACE_DEBUG(2, TRACE_CORE_1, "sending signal start %d\n", (int) sig.get_sig_id());
                    }
                    break;
                default:
                    TRACE_ABNORMAL(TRACE_CORE_1, "pause has no effect %d\n", (int) sensors_data_display.current_state);
                    break;
            }
            sensors_data.current_state = sensors_data_display.current_state;
            mutex_exit(&sensorDataMutex);
        }

        // if(last_btn2_rel > last_btn2_press && last_btn2_rel - last_btn2_press > 500)
        // {
        //     // Frame pause_label = {0, DISPLAY_HEIGHT / 4, DISPLAY_WIDTH, DISPLAY_HEIGHT / 2};
        //     // const sFONT* font = 0;
        //     // uint8_t scale;
        //     // auto label = "PRESS";
        //     // auto width_char = pause_label.width / strlen(label);
        //     // getFontSize(width_char, pause_label.height, &font, &scale);
        //     // Paint_Println(pause_label.x, pause_label.y, label, font, {0x0,0xf,0x0}, scale);
        //     // display::display();
        // // }

        if (btn2.is_pressed_long_execute())
        {
            display::clear();
            Frame pause_label = {0, DISPLAY_HEIGHT / 4, DISPLAY_WIDTH, DISPLAY_HEIGHT / 2};
            const sFONT* font = 0;
            uint8_t scale;
            auto label = "SAVING";
            auto width_char = pause_label.width / strlen(label);
            getFontSize(width_char, pause_label.height, &font, &scale);
            Paint_Println(pause_label.x, pause_label.y, label, font, {0x0,0xf,0x0}, scale);
            display::display();
            // save track and wait for restart
            // TODO improve add menu ???

            Signal sig(SIG_CORE0_STOP);
            actor_core0.send_signal(sig);

            mutex_enter_blocking(&sensorDataMutex);
            sessionDataDisplay.end(sensors_data.current_time);

            Sd_File last_save("last_track.csv");
            if(last_save.is_empty())
            {
                last_save.append(sessionDataDisplay.get_header());
            }
            last_save.append(sessionDataDisplay.get_line().c_str());
            mutex_exit(&sensorDataMutex);


            // while(stop)
            {
                //PRINTF("STOPPED\n");
                display::clear();
                Frame pause_label = {0, DISPLAY_HEIGHT / 4, DISPLAY_WIDTH, DISPLAY_HEIGHT / 2};
                const sFONT* font = 0;
                uint8_t scale;
                auto label = "SAVED";
                auto width_char = pause_label.width / strlen(label);
                getFontSize(width_char, pause_label.height, &font, &scale);
                Paint_Println(pause_label.x, pause_label.y, label, font, {0x0,0xf,0x0}, scale);
                display::display();
                sleep_ms(1000);
            }


            // Signal sig_start(SIG_CORE0_SESION_START);
            // actor_core0.send_signal(sig_start);
        }

        mutex_enter_blocking(&sensorDataMutex);
        auto system_sate = sensors_data_display.current_state;
        mutex_exit(&sensorDataMutex);

        // render
        // Display_update(); TODO
        auto gui = Gui::get_gui();
        gui->refresh();

        if(system_sate == SystemState::PAUSED)
        {
            TRACE_DEBUG(2, TRACE_CORE_1, "printing pause label %d\n", change_state);
            Frame pause_label = {0, DISPLAY_HEIGHT / 4, DISPLAY_WIDTH, DISPLAY_HEIGHT / 2};
            const sFONT* font = 0;
            uint8_t scale;
            auto label = "PAUSED";
            auto width_char = pause_label.width / strlen(label);
            getFontSize(width_char, pause_label.height, &font, &scale);
            Paint_Println(pause_label.x, pause_label.y, label, font, {0xf,0x0,0x0}, scale);
        }
        display::display();
    }

    absolute_time_t frameEnd = get_absolute_time();
    auto frameTimeUs = absolute_time_diff_us(frameStart, frameEnd);
    if(fpsToUs(FRAME_PER_SECOND) > frameTimeUs)
    {
        // frame took less time
        int64_t timeToSleep = fpsToUs(FRAME_PER_SECOND) - frameTimeUs;
        sleep_us(timeToSleep);
        TRACE_DEBUG(1, TRACE_CORE_1, "frame took %" PRIi64 " max time is %" PRIi64 " sleeping %" PRIi64 "\n",frameTimeUs, fpsToUs(FRAME_PER_SECOND), timeToSleep);
    }
    else
    {
        int64_t timeToSleep = fpsToUs(FRAME_PER_SECOND) - frameTimeUs;
        TRACE_ABNORMAL(TRACE_CORE_1, "frame took %" PRIi64 " should be %" PRIi64 " delta %" PRIi64 "\n",frameTimeUs, fpsToUs(FRAME_PER_SECOND), timeToSleep);
    }
    return 1;
}

static void set_total_data(float time, float dist)
{
    Sd_File total_stats("total_stats.txt");
    const auto stats = total_stats.read_all();
    std::string new_dist_time = std::to_string(dist) + ';' + std::to_string(time);
    if(new_dist_time.length() < stats.length())
    {
        total_stats.clear();
    }
    total_stats.overwrite(new_dist_time.c_str());
}

static void get_total_data(float& time, float& dist)
{
    Sd_File total_stats("total_stats.txt");
    const auto stats = total_stats.read_all();
    const auto dist_time = split_string(stats, ';');
    dist = 0.0f, time = 0.0f;
    if(dist_time.size() == 2)
    {
        dist = std::atof(dist_time.at(0).c_str());
        time = std::atof(dist_time.at(1).c_str());
    }
}
