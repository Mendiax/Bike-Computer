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