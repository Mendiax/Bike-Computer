#ifndef CSV_INTERFACE_HPP
#define CSV_INTERFACE_HPP


class Csv_Interface
{
private:

public:
    virtual const char* get_header() = 0;
    virtual std::string get_line() = 0;
};

#endif