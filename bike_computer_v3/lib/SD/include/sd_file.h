#ifndef SD_FILE_H
#define SD_FILE_H

class Sd_File
{
private:
    const char* file_name;
public:
    Sd_File(const char* file_name);
    ~Sd_File();
    void append(const char* string);
    bool is_empty();
    bool clear();
};


#endif