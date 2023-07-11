#ifndef __GPIO_UART_HPP__
#define __GPIO_UART_HPP__
// #-------------------------------#
// |           includes            |
// #-------------------------------#
// pico includes

// c/c++ includes

// my includes

// #-------------------------------#
// |            macros             |
// #-------------------------------#
// swap real uart for mock
#include <string>
#define uart_puts(uart, str) \
    mock_uart_puts(str)
#define uart_getc(uart) \
    mock_uart_getc()
#define uart_is_readable(uart) \
    mock_uart_is_readable()

#define irq_set_exclusive_handler(urat, handler) \
    mock_irq_set_exclusive_handler(handler)
// #-------------------------------#
// | global types declarations     |
// #-------------------------------#
typedef void (*uart_irq)(void);
// #-------------------------------#
// | global variables declarations |
// #-------------------------------#

// #-------------------------------#
// | global function declarations  |
// #-------------------------------#

void mock_uart_puts( const char* str);
char mock_uart_getc();
size_t mock_uart_is_readable();
void mock_irq_set_exclusive_handler(uart_irq irq_callback);

/*
Send data to sim868. Append data to buffer and call sim callback function
*/
void send_to_pico(const std::string& str);

/*
gets data from pico
*/
std::string get_msg_from_pico();

#endif
