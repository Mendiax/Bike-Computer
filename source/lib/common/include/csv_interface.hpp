#ifndef CSV_INTERFACE_HPP
#define CSV_INTERFACE_HPP

#include <string>

/**
 * @brief Interface for classes that provide CSV data.
 */
class Csv_Interface
{
public:
    /**
     * @brief Virtual destructor.
     */
    virtual ~Csv_Interface() {}

    /**
     * @brief Get the CSV header.
     * @return The CSV header as a C-style string.
     */
    virtual const char* get_header() = 0;

    /**
     * @brief Get a CSV line.
     * @return The CSV line as a std::string.
     */
    virtual std::string get_line() = 0;
};

#endif // CSV_INTERFACE_HPP
