#ifndef CSV_INTERFACE_HPP
#define CSV_INTERFACE_HPP


class Csv_Interface
{
private:
    
public:
    virtual const char* get_header();
    virtual std::string get_line();
};

#endif