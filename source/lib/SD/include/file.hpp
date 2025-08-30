#ifndef __SD_FILE_HPP__
#define __SD_FILE_HPP__
// #-------------------------------#
// |           includes            |
// #-------------------------------#
// c/c++ includes
#include <stddef.h>
#include <string>
// my includes

// #-------------------------------#
// |            macros             |
// #-------------------------------#

// #-------------------------------#
// | global types declarations     |
// #-------------------------------#

class FilePrivate;

class File {
    public:
        enum Flags {
            READ = 1,
            WRITE = 2,
            APPEND = 4,
            CREATE = 8,
            TRUNCATE = 16
        };
    private:
        int flags;
        const std::string file_name;
        FilePrivate* pimpl;
    public:
        File(const std::string_view& file_name, int flags);
        ~File();
        bool is_open();
        size_t read(char* buffer, size_t len);
        size_t write(const std::string_view& file_name);
        void flush();
        void close();
        bool eof();
        bool seek(size_t pos);
        size_t tell();
        inline const std::string& get_file_name() { return this->file_name; }
        inline bool is_empty() { return this->tell() == 0; }
};

// #-------------------------------#
// | global variables declarations |
// #-------------------------------#

// #-------------------------------#
// | global function declarations  |
// #-------------------------------#

// #-------------------------------#
// |  global function definitions  |
// #-------------------------------#

#endif
