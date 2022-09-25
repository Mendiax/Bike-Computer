#include <views/window.h>

#include <algorithm>
#include "views/display.h"


#include <views/screenfunc/plot.h>
#include <views/screenfunc/lastval.h>
#include <views/screenfunc/label.h>
#include <views/screenfunc/val.h>

// Display _Display;

// typedef void (*view_new_func)(void);

// // views
// void view0(void){}
// void view1(void){}
// void view2(void){}
// void view3(void){}
// void view4(void){}
// void view5(void){}



// extern void view_charge(void);
// extern void view_main(void);

// // view3, ,  view4
// view_new_func views_all[] = {view0, view1, view2, view4, view5};
// #define DISPLAY_TYPES_LENGTH (sizeof(views_all) / sizeof(view_new_func))
// void Display_init(Sensor_Data *data, Session_Data *session)
// {
//     _Display.data = data;
//     _Display.currentType = 0;
//     _Display.session = session;

//     // setup
//     display::init();
//     display::clear();
//     display::display();

//     views_all[_Display.currentType]();
// }

// void Display_setDisplayType(uint8_t type)
// {
//     _Display.currentType = (uint8_t)type;
// }

// void Display_incDisplayType()
// {
//     _Display.currentType = (_Display.currentType + 1) % DISPLAY_TYPES_LENGTH;
//     assert(_Display.currentType >= 0 && _Display.currentType < DISPLAY_TYPES_LENGTH);

//     views_all[_Display.currentType]();
// }

// void Display_set_charge_display_type()
// {
//     view_charge();
// }
// void Display_set_main_display_type()
// {
//     view_main();
// }

// void Display_decDisplayType()
// {
//     _Display.currentType = _Display.currentType > 0 ? (_Display.currentType - 1) : DISPLAY_TYPES_LENGTH - 1;
//     assert(_Display.currentType >= 0 && _Display.currentType < DISPLAY_TYPES_LENGTH);

//     views_all[_Display.currentType]();
// }

// void Display_update()
// {
//     display::clear();

//     //display data on screen
//     view_draw(&_Display.view);

//     // display::display();
// }
