#ifndef SD_FILE_H
#define SD_FILE_H

#include <string>
#include <vector>
#include "ff.h"

class Sd_File
{
    public:
    enum Result{
        F_OK, F_ERROR
    };
private:
    Result last_result;
    const std::string file_name;
    // positions of lines after get_no_of_lines()
    std::vector<size_t> lines_pos;
    bool correct_lines_pos;

    FIL append_file{};
public:
    Sd_File(const std::string& file_name);
    ~Sd_File();
    void remove();
    inline Result get_result()
    {
        return last_result;
    }
    inline const char* get_file_name(){return this->file_name.c_str();}
    Result append(const char* string, bool reset = true);
    bool is_empty();
    size_t get_size();
    Result clear();
    std::string read_all();
    Result overwrite(const char* string, size_t pos = 0);

    size_t get_no_of_lines();
    std::string read_last_line(size_t max_len);
    std::string read_line(size_t line_no, size_t max_len);
    std::vector<std::string> read_all_lines(size_t max_len);


};

#endif
