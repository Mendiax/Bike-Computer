#ifndef __PRINT_H__
#define __PRINT_H__

#include <assert.h>
/*print in serial in range 0-100*/
void printOnPC(double data)
{

    //Serial.print(100);
    //Serial.print("\t");
    Serial.print(0);
    Serial.print("\t");
    Serial.print(data);
    Serial.println("\t");
}

//#define TRACE_SPEED

#ifdef NDEBUG
    #define DEBUG_PRINT(X) (void*) 0
#else
    #define DEBUG_PRINT(X) Serial.println(String(__FILE__) + " line: " + String(__LINE__) + " " + X)
#endif

#ifdef TRACE_DISPLAY
    #define TRACE_DISPLAY_PRINT(X) Serial.println(String(__FILE__) + " line: " + String(__LINE__) + " " + X)   
#else
    #define TRACE_DISPLAY_PRINT(X) (void*) 0
#endif

#ifdef TRACE_FRAME
    #define TRACE_FRAME_PRINT(X) Serial.println(String(__FILE__) + " line: " + String(__LINE__) + " " + X)   
#else
    #define TRACE_FRAME_PRINT(X) (void*) 0
#endif

#ifdef TRACE_SPEED
    #define TRACE_SPEED_PRINT(X) Serial.println(X)   
#else
    #define TRACE_SPEED_PRINT(X) (void*) 0
#endif
//max 200 length
void printArgs(char *format, ...)
{
    const size_t MAX_LENGTH = 200;
    char buffer[MAX_LENGTH] = {0};
    char *end = strchr(format, 0);
    char *currentPos = 0;
    currentPos = format;
    size_t bufferPos = 0;
    assert(strchr(currentPos, '\0'));

    va_list args;
    va_start(args, format);

    while (currentPos != end)
    {
        switch (*currentPos)
        {
        case '%':
            switch (*++currentPos)
            {
            case 'd':
                String((int)va_arg(args, int)).c_str();
                break;
            case 's':
                (char*)va_arg(args, char *);
            default:
                break;
            }
            break;
        case '\0':
            goto RET;
        default:
            if(bufferPos == MAX_LENGTH - 1){goto RET;}
            buffer[bufferPos++] = *currentPos;
            break;
        }
        currentPos++;
    }
RET:
    Serial.print(buffer);
    va_end(args);
}

#endif
