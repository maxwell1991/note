#ifndef __UART_H__
#define __UART_H__

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "lpuart1.h"
#include "lpuart_driver.h"
#include "ring_buf.h"

#define UART0_RCV_BUFF_SIZE		600
#define UART1_RCV_BUFF_SIZE		200
#define UART2_RCV_BUFF_SIZE		200


/*
typedef struct uart_Ringbuff_tag
{
   uint16_t rx_in;                  // Rx buffer input index
   uint16_t rx_out;                 // Rx buffer output index
//   uint16_t rx_count;               // Rx buffer byte count
//   uint16_t rx_size;                // Rx buffer size
//   uint8_t* rx_buf;                 // Rx ring buffer

//   uint16_t tx_in;                  // Tx buffer input index
//   uint16_t tx_out;                 // Tx buffer output index
//   uint16_t tx_count;               // Tx buffer byte counter
//   uint16_t tx_size;                // Tx buffer size
//   uint8_t* tx_buf;                 // Tx ring buffer
//   bool     tx_progress;            // Tx in progress
} uart_Ringbuff;
*/
//uart_Ringbuff  *ringbuff0, *ringbuff1,*ringbuff2;




void bsp_uart0_init(void);
void bsp_uart1_init(void);
void bsp_uart2_init(void);
uint8_t Uart_Get_Char (uint32_t uart_ch, uint8_t* ptr);
void bsp_uart_rx_callback(uint32_t instance, void * lpuartState);

#endif /* __UART_H__*/
/*******************************************************************************
 * EOF
 ******************************************************************************/

