#include "uart.h"
#include "pin_mux.h"
#include "lpuart1.h"
#include "lpuart2.h"
#include "dmaController1.h"
#include "Cpu.h"
//extern uint8_t echo_char(uint32_t instance, uint8_t c);
 uint8_t pRxBuffPtr_uart0[UART0_RCV_BUFF_SIZE];
uint8_t pRxBuffPtr_uart1[UART1_RCV_BUFF_SIZE];
 uint8_t pRxBuffPtr_uart2[UART2_RCV_BUFF_SIZE];

static RingBuf_t  uart0_rx_ring_control;
static RingBuf_t  uart1_rx_ring_control;
 RingBuf_t  uart2_rx_ring_control;

volatile uint32_t ReadCnt;

void bsp_uart0_init(void)
{
	  /* Initialize LPUART instance */
	  LPUART_DRV_Init(INST_LPUART1, &lpuart1_State, &lpuart1_InitConfig0);
	  LPUART_DRV_InstallRxCallback(INST_LPUART1, bsp_uart_rx_callback, 0/*void * callbackParam*/);
	  RingBuf_Reset(&uart1_rx_ring_control, UART1_RCV_BUFF_SIZE);
	  LPUART_DRV_ReceiveData(INST_LPUART1,pRxBuffPtr_uart1,1);
}

/*
void bsp_uart1_init(void)
{
	  // Initialize LPUART instance
	  LPUART_DRV_Init(INST_LPUART1, &lpuart1_State, &lpuart1_InitConfig0);
	  LPUART_DRV_InstallRxCallback(INST_LPUART1, bsp_uart_rx_callback, 0);//void * callbackParam);
	  RingBuf_Reset(&uart1_rx_ring_control, UART1_RCV_BUFF_SIZE);
}
*/



void bsp_uart2_init(void)
{
	    LPUART_DRV_Init(INST_LPUART2, &lpuart2_State, &lpuart2_InitConfig0);
	  LPUART_DRV_InstallRxCallback(INST_LPUART2, bsp_uart_rx_callback, 0/*void * callbackParam*/);
	  RingBuf_Reset(&uart2_rx_ring_control, UART2_RCV_BUFF_SIZE);
	  LPUART_DRV_ReceiveData(INST_LPUART2,pRxBuffPtr_uart2,1);
}

//extern uint8_t echo_char(uint32_t instance, uint8_t c);

void bsp_uart_rx_callback(uint32_t instance, void * lpuartState)
{
    const LPUART_Type * base = g_lpuartBase[instance];
    uint8_t tmp;

	/* Receive the data */
	LPUART_HAL_Getchar(base, &tmp);

    if(instance==0)
    {
    	pRxBuffPtr_uart0[uart0_rx_ring_control.in] = tmp;
    	RingBuf_Add(&uart0_rx_ring_control);
    }
    else if(instance==1)
    {
    	pRxBuffPtr_uart1[uart1_rx_ring_control.in] = tmp;
    	RingBuf_Add(&uart1_rx_ring_control);

    }
    else if (instance==2)
    {
    	pRxBuffPtr_uart2[uart2_rx_ring_control.in] = tmp;
    	RingBuf_Add(&uart2_rx_ring_control);
    	//ReadCnt++;
    }
}




uint8_t Uart_Get_Char (uint32_t instance, uint8_t* ptr)
{

   bool ret = false;        // Return value. Assume buffer empty!

   // Error checking
   if (!ptr)
	   return false;    // Do not accept NULL pointers!

   //Disable_Interrupts();
   //ENABLE_INTERRUPTS();
   DISABLE_INTERRUPTS();
   if(1)
   {
	   if(instance==0)//uart0
	      {
	   	   *ptr = pRxBuffPtr_uart0[uart0_rx_ring_control.out];
	   	   RingBuf_Remove(&uart0_rx_ring_control);
	   	   ret = true;
	      }
	      else if(instance==1)//uart1
	      {
	   	   *ptr = pRxBuffPtr_uart1[uart1_rx_ring_control.out];

	   	   RingBuf_Remove(&uart1_rx_ring_control);
	   	   ret = true;
	      }
	      else if(instance==2)//uart2
	      {
	    	  if(!RingBuf_IsEmpty(&uart2_rx_ring_control))
	    	  {
	    		  *ptr = pRxBuffPtr_uart2[uart2_rx_ring_control.out];
	    			   	   //ReadCnt--;
	    			   	   RingBuf_Remove(&uart2_rx_ring_control);
	    			   	   ret = true;
	    	  }

	      }

   }

   ENABLE_INTERRUPTS();
   //Enable_Interrupts();
   //DISABLE_INTERRUPTS();

   return (ret);
}

