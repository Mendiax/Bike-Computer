#ifndef SD_FILE_H
#define SD_FILE_H

#include <string>

class Sd_File
{
    public:
    enum Result{
        F_OK, F_ERROR
    };
private:
    Result last_result;
    const char* file_name;
public:
    Sd_File(const std::string& file_name);
    ~Sd_File();
    void remove();
    inline Result get_result()
    {
        return last_result;
    }
    Result append(const char* string);
    bool is_empty();
    size_t get_size();
    Result clear();
    std::string read_all();
    Result overwrite(const char* string, size_t pos = 0);

};

#endif
