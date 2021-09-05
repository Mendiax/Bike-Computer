#ifndef __BUTTONS_H__
#define __BUTTONS_H__

enum Buttons{
  btnNone, btnLeft, btnLeftMiddle, btnMiddle, btnRightMiddle, btnRight
};

Buttons getButton(){
  long val = analogRead(PIN_BUTTONS);
  Serial.println(val);
  if(val < 200){  //no press
    return btnNone;
  }
  if(val < 300){  //right
    return btnRight;
  }
  if(val < 400){  //right + middle
    return btnRightMiddle;
  }
  if(val < 600){  //middle
    return btnMiddle;
  }
  if(val < 700){  //middle + left
    return btnLeftMiddle;
  }
  return btnLeft;       //left
}

#endif