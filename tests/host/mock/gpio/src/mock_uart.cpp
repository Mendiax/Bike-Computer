
// #------------------------------#
// |          includes            |
// #------------------------------#
// pico includes

// c/c++ includes
#include <cstddef>
#include <iostream>
#include <queue>
#include <string>
#include <sys/types.h>
#include <mutex>
// my includes
#include "mock_uart.hpp"

// #------------------------------#
// |           macros             |
// #------------------------------#

// #------------------------------#
// | global variables definitions |
// #------------------------------#

// #------------------------------#
// | static variables definitions |
// #------------------------------#
static std::mutex mtx_getc_buffer;
static std::string getc_buffer;
static uart_irq uart_irq_handler;

static std::mutex mtx_puts_buffer;
static std::queue<std::string> puts_buffer;
// #------------------------------#
// | static functions declarations|
// #------------------------------#

// #------------------------------#
// | global function definitions  |
// #------------------------------#
void mock_irq_set_exclusive_handler(uart_irq irq_callback)
{
    uart_irq_handler = irq_callback;
}

void mock_uart_puts(const char* str)
{
    std::unique_lock<std::mutex> lock(mtx_puts_buffer);
    std::cout << "mock uart:" << str << std::endl;
    puts_buffer.emplace(str);
}

std::string get_msg_from_pico()
{
    std::unique_lock<std::mutex> lock(mtx_puts_buffer);
    if (!puts_buffer.empty())
    {
        std::string msg = puts_buffer.front();
        puts_buffer.pop();

        return msg;
    }
    return "";
}

size_t mock_uart_is_readable()
{
    return getc_buffer.length();
}
char mock_uart_getc()
{
    std::unique_lock<std::mutex> lock(mtx_getc_buffer);
    if(mock_uart_is_readable() == 0){
        return '\0';
    }
    const char next_char = getc_buffer.front();
    getc_buffer = getc_buffer.substr(1);
    return next_char;
}


void send_to_pico(const std::string& str)
{
    mtx_getc_buffer.lock();
    getc_buffer += str;
    mtx_getc_buffer.unlock();
    if(uart_irq_handler)
    {
        uart_irq_handler();
    }
}
// #------------------------------#
// | static functions definitions |
// #------------------------------#
