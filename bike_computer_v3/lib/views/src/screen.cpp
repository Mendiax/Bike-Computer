#include "display/print.h"

#include <views/screen.h>
#include "traces.h"
void Screen_setup()
{
  display::init();
  display::clear();
  Paint_Println(0,10, "START", &Font24, FONT_FOREGROUND);
  display::display();
}

void Screen_draw(){
  display::display();
  TRACE_DEBUG(0, TRACE_DISPLAY_PRINT, "drawing display%s\n", "");
}

void Screen_clear(){
  display::clear();
  //__display->clear();
}

