// ##################################################################################################################################################
// notes uart irq
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/util/queue.h"
#include "pico/multicore.h"
#include <string.h>
#include "hardware/timer.h"
#include "hardware/uart.h"
#include "hardware/irq.h"




critical_section_t acrit;


uint32_t count = 0;

struct data_s
{
    uint8_t data[64];
}queue_entry_t;

queue_t rx_queue;
queue_t tx_queue;
queue_t rx_buf;
queue_t tx_buf;

uint8_t buff[256];

uint8_t t1[64];
uint8_t t2[64];
volatile uint8_t last = 0;

uint32_t int_calls = 0;

volatile absolute_time_t before_add;
volatile absolute_time_t after_add ;
volatile absolute_time_t int_called_first ;
volatile absolute_time_t int_fin_first ;
volatile absolute_time_t int_start_first ;

uint8_t write_data [64]; 
#define rx_buff_size 256

volatile uint32_t received = 0;
volatile uint32_t int_rentered = 0;
volatile uint8_t int_flag = 0;

struct ringbuff_t
{
    volatile uint8_t buff[rx_buff_size];
    volatile uint32_t head;
    volatile uint32_t tail;
    volatile uint32_t added;
    volatile uint32_t consumed;
    volatile uint32_t discared;
};

struct ringbuff_t rx_buff;

#define UART_ID uart0
#define BAUD_RATE 300
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY    UART_PARITY_NONE

// We are using pins 0 and 1, but see the GPIO function select table in the
// datasheet for information on which other pins can be used.
#define UART_TX_PIN 12
#define UART_RX_PIN 13

static int chars_rxed = 0;
static uint32_t chars_rx_last = 0;

// RX interrupt handler
void on_uart_rx() {
    if(int_calls==0){int_start_first =  get_absolute_time();}//record first time the interupt was called
    if(int_flag == 1){int_rentered++;}// The interupt routine was re-entered
    int_flag = 1;
    if(int_calls!=0){int_called_first =  get_absolute_time();}//record first time the interupt was called
    int_calls++; // update the number of times the interupt is called

    // received = 0;
    received  = uart_is_readable(UART_ID);
    //chars_rx_last = 0;
    printf("Bytes in FIFO = :%u\n", received);
    while (uart_is_readable(UART_ID)) 
    {
        uint8_t ch = uart_getc(UART_ID);
        // Can we send it back?
        
            // Change it slightly first!
         //   ch++;
         //   uart_putc(UART_ID, ch);
         //printf("%c\n",ch);
       
        chars_rx_last++;
        chars_rxed++;
        uint16_t space = 0;
        //absolute_time_t before =  get_absolute_time();
       // for (int i = 0; i<32; i++)
        //{
            if (rx_buff.head < (rx_buff.tail))
            {
                space = rx_buff.tail - rx_buff.head -1;
            }
            else{
                    space = (rx_buff_size - rx_buff.head) + rx_buff.tail -1;
            }
            if(space>0)
            {
                //add data
                rx_buff.buff[rx_buff.head] = ch;
                 
                 last = ch;
                 //printf("%c\n",last);
                  //printf("%c\n",rx_buff.buff[rx_buff.head] );
                 // printf("%c\n",rx_buff.buff[rx_buff.head] );
                 // printf("Head Counter :%u\n",rx_buff.head);
                rx_buff.head++;
                if (rx_buff.head == rx_buff_size) {rx_buff.head = 0;}
                rx_buff.added++;
                
            }
            else{
                rx_buff.discared++;
               // printf("Core1 discarded head =   %u\n", rx_buff.head);
               // printf("Core1 discarder tail =   %u\n", rx_buff.tail);
            }
       // }

    }
     if(int_calls==1){int_fin_first =  get_absolute_time();}//record first time the interupt was called
      int_flag = 0;
}


int main()
{
    
    stdio_init_all();    
    // Set up our UART with a basic baud rate
    uart_init(UART_ID, BAUD_RATE);

    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    // Actually, we want a different speed
    // The call will return the actual baud rate selected, which will be as close as
    // possible to that requested
    int actual = uart_set_baudrate(UART_ID, BAUD_RATE);

    // Set UART flow control CTS/RTS, we don't want these, so turn them off
    uart_set_hw_flow(UART_ID, false, false);

    // Set our data format
    uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY);

    //Attemp to change RX watermark level, does not seem to change.
    //hw_set_bits(&uart_get_hw(UART_ID)->lcr_h, UART_UARTIFLS_RXIFLSEL_BITS); //bits 543
    hw_set_bits(&uart_get_hw(UART_ID)->lcr_h, _u(0x00000020)); //bits 5 = 1
    hw_clear_bits(&uart_get_hw(UART_ID)->lcr_h, _u(0x00000018));//clear bit 4,1

    //try to set the watermark level, does not seem to change
    uint32_t val_set = 0x20;
    hw_write_masked(&uart_get_hw(UART_ID)->lcr_h,val_set,
                   UART_UARTIFLS_RXIFLSEL_BITS);

    // Turn off FIFO's - we want to do this character by character
    //uart_set_fifo_enabled(UART_ID, false);
    uart_set_fifo_enabled(UART_ID, true);
    // Set up a RX interrupt
    // We need to set up the handler first
    // Select correct interrupt for the UART we are using
    int UART_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;

    // And set up and enable the interrupt handlers
    irq_set_exclusive_handler(UART_IRQ, on_uart_rx);
    irq_set_enabled(UART_IRQ, true);

    // Now enable the UART to send interrupts - RX only
    uart_set_irq_enables(UART_ID, true, false);

    //load data to send on the UART
    for(int i =0; i<64; i++)
    {
        write_data[i]= i+47; //ascii values from "\""
    }
    critical_section_init(&acrit);
    

    sleep_ms(10000);

    printf("Start UART FIFO and Interupt Test\n");
   // printf("time 1  %lld\n", time_us_64() );
   // printf("time2  %lld\n", time_us_64() );
    before_add = get_absolute_time(); // record time we start to put data out on the UART
    sleep_us(100);
    after_add = get_absolute_time(); // record time we start to put data out on the UART
    int64_t time_diff = absolute_time_diff_us(before_add, after_add);
    printf("Absolute time diff test =   %lld\n", time_diff);

   //initialise ring buffer
    rx_buff.added = 0;
    rx_buff.consumed =0;
    rx_buff.head = 0;
    rx_buff.tail = 0;
    rx_buff.discared = 0;
    chars_rxed = 0;
    int_calls = 0;
    int32_t loop_count = 0;
    //sleep_ms(1000);
    while(1)
    {
        
        before_add =  get_absolute_time(); // record time we start to put data out on the UART
        sleep_us(100);
        //uart_puts(UART_ID, "Loop 123456789 123456789 "); // 
        //With the FIFO eneabled the interupt seem to be called after 4 bytes
        if (loop_count ==0){uart_write_blocking(UART_ID,write_data,16);}
        loop_count++;
        printf("UART Data Set, wait for 5 seconds\n");
 
        sleep_ms(5000);
       // for (int i = 0; i<4; i++)
       // {
       //     printf("Data: %u\n",rx_buff.buff[i]);
       // }
        uint32_t tmphead = 0;
        uint32_t num_data = 0;
        uint8_t data = 0;
        uint64_t time_diff = absolute_time_diff_us(before_add, int_called_first);
        critical_section_enter_blocking(&acrit);  //Seems to be ok without so increment atomic ?
        tmphead = rx_buff.head ;
        critical_section_exit(&acrit);

        printf("*******************************************************\n");
        printf("\n");
        printf("%c\n",last);
        printf("Interupt handler was called, number of times = %u\n", int_calls);// int_flag = 1;
        printf("Interupt handler was re-entered , number of times = %u\n", int_rentered);// int_flag = 1;
        printf("Approx micorsecond after data available interupt was called:%lld\n", time_diff); //int_called_first 
        time_diff = absolute_time_diff_us(int_start_first, int_fin_first);
        printf("Interupt runtime microsedocns: :%lld\n", time_diff); //int_called_first 
        printf("Bytes processec by interupt = %u\n", chars_rxed);//rx_buff.tail 
        printf("Ring buffer head = %u\n", tmphead);
        printf("Ring buffer tail = %u\n", rx_buff.tail);
        printf("Total bytes added to ring: %u\n",rx_buff.added);
        //printf("Rx bytes recievd last %u\n", chars_rx_last);//after_add 
        printf("*******************************************************\n");


        //Get waht data in on the buffer and print it.

        if (tmphead != rx_buff.tail)
        {
            
            if (tmphead < rx_buff.tail)
            {
                num_data = (rx_buff_size - rx_buff.tail) + tmphead ;
            }
            else
            {
                  num_data = tmphead - rx_buff.tail ;
            }
            printf("Number of bytes to process in ring buffer: %u\n", num_data);
            int32_t tmptail = rx_buff.tail;
            printf("Start to decode buffer, tail: %u\n", tmptail);
            for(int i = 0; i < num_data; i++)
            {
                //printf("ring buffer decode for loop\n");
                data = rx_buff.buff[tmptail];
                 printf("\ndecoding at position: %u", tmptail);
                printf("  Data:%u",data);
                tmptail++;
                rx_buff.consumed++;
                if(tmptail == rx_buff_size) {tmptail=0;}

            }
            critical_section_enter_blocking(&acrit);
                rx_buff.tail = tmptail;
            critical_section_exit(&acrit);
        }
        printf("\nTotal bytes read from ring: = %u\n", rx_buff.consumed);
        printf("Ring buffer processed head = %u\n", tmphead);
        printf("Ring buffer tail = %u\n", rx_buff.tail);
        printf("\n");
       // for (int i = 0; i<4; i++)
      //  {
       //     printf("Data: %u\n",rx_buff.buff[i]);
       // }
        sleep_ms(100000);
    }

    /// \end::setup_multicore[]
    return 0;
}