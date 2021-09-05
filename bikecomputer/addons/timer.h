#ifndef __TIMER_H__
#define __TIMER_H__

class Timer{
private:
    String name;

    unsigned long timeStart;
    unsigned long timerEnd;
    unsigned long duration;
public:
    Timer(String timerName){
        this->name = timerName;
    }

    void start(){
        this->duration = 0;
        this->timerStart = millis();
    }

    unsigned long stop(){
        this->timerEnd = millis();
        this->duration = this->timerEnd - this->timerStart;
        return this->duration;
    }

    void printLogSeconds(){
        float seconds = (float)this->duration / 1000.0;
        Serial.println("[TIMER LOG]" + this->name + " took " + String(seconds) + "s" );
    }

    void printLogMillis(){
        Serial.println("[TIMER LOG]" + this->name + " took " + String(this->duration ) + "ms" );
    }
}

#endif