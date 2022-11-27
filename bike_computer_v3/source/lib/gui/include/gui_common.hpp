#ifndef GUI_COMMON_HPP
#define GUI_COMMON_HPP
// #-------------------------------#
// |           includes            |
// #-------------------------------#
// pico includes

// c/c++ includes
#include <list>

// my includes
#include "buttons/buttons.h"

// #-------------------------------#
// |            macros             |
// #-------------------------------#
#define BTN_NAVIGATE_BACK btn0
#define BTN_NAVIGATE btn1
#define BTN_ACTION btn2
#define BTN_ACTION_SECOND btn3



// #-------------------------------#
// | global types declarations     |
// #-------------------------------#


namespace gui
{
    class View_List;

    class View
    {
    private:
        View_List* next;

    public:
        View(View_List* next = nullptr);
        ~View();

        View_List* get_next_view_list();
        /**
         * @brief render function that renders current view
         *
         */
        virtual void render(void) = 0;

        /**
         * @brief special function that executes when special button is pressed
         * Mainly used to go to the next view
         *
         */
        virtual void action(void);
        virtual void action_long(void);

        virtual void action_second(void);
        virtual void action_second_long(void);

    };

    class View_List
    {
    private:
        std::list<View*> views_list;
        std::_List_iterator<gui::View *> it;
        View_List* back;
    public:
        View_List();
        View_List(View_List* top);

        ~View_List();
        View* get_next_view();
        View* get_prev_view();

        View* get_current_view();

        View_List* get_back();

        void add_view(View* new_view);
    };

};

// void create_emnu()
// {
//     using namespace gui;

//     View main_menu_new_session;
//     View main_menu_session_history;
//     View_List main_menu;
//     main_menu.add_view(&main_menu_new_session);
//     main_menu.add_view(&main_menu_session_history);

// };


// #-------------------------------#
// | global variables declarations |
// #-------------------------------#

// #-------------------------------#
// | global function declarations  |
// #-------------------------------#

#endif
