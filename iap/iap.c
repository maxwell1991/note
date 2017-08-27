#include <stdio.h>

#include "Cpu.h"
#include "clockMan1.h"
#include "Flash.h"
#include "pin_mux.h"
#include "lpuart1.h"
#include "lpuart2.h"
#include "device_registers.h"
#include <stdint.h>
#include <stdbool.h>


typedef enum UartChnTag_e
{
    UART1 = 0,
    UART2,
    UART3,
    UART4,
    UART5,
    UART6,
    UART7,
    UART8,

    UART_CHN_NUM

}UartChnTag_t;
#define IAPUPG_HINT()
//#define NULL            0x00000000
extern volatile uint32_t systick;
#define ROM_FLASH_ADDRESS   0x8000//32K
#define ROM_APP_SECTOR_CNT  120//(512K - 32K)/4K
//#define FEATURE_FLS_PF_BLOCK_SECTOR_SIZE (4096u)
flash_ssd_config_t flashSSDConfig;
#define UPG_UART_CHN    UART1

#define REQ_START  0x7F
#define REQ_SHAKEHANDS  0x55
#define REQ_DATA_SEND_END  0xAE
#define REQ_BIN_SEND_END  0xAF

#define RSP_START  0xA0
#define RSP_SHAKEHANDS  0xA1
#define RSP_FLASH_ERASE_OK  0xA2
#define RSP_DATA_RCV_SAVE_OK  0xA3
#define RSP_DATA_SEND_END  0xA4
#define RSP_BIN_SIZE_VERIFY_OK  0xA5
#define RSP_UPG_FINISH  0xA6

#define RSP_FLASH_ERASE_FAIL  0xE1
#define RSP_BIN_SIZE_OVERFLOW  0xE2
#define RSP_FLASH_VERIFY_FAIL  0xE3
#define RSP_DATA_VERIFY_FAIL  0xE4
#define RSP_DATA_LEN_WRONG  0xE5
#define RSP_JUMP_ERROR  0xE6
#define RSP_BIN_SIZE_VERIFY_FAIL  0xE7

#define RSP_TIMEOUT 0xEF

#define IAPUPG_HINT() //HAL_GPIO_TogglePin(TEST_OUT_GPIO_Port, TEST_OUT_Pin)

#define IAPUPG_CHECK_UART_TIME  100

enum IAP_UPG_STATE_e
{
    UPG_STT_CHECK_START = 0,
    UPG_STT_START,
    UPG_STT_SHAKEHANDS,
    UPG_STT_FLASH_ERASED,
    UPG_STT_RCV_DATA,
    UPG_STT_BIN_VERIFY,
    UPG_STT_WAIT_END
};

typedef void* UartHandle_t;
static uint8_t gUartRxRingBuf[200];


static UartHandle_t gUartHandle = NULL;
static uint8_t gPackBuf[260];


extern volatile uint32_t ReadCnt;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
//void SystemClock_Config(void);
//void Error_Handler(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */



void HAL_Delay(uint32_t Delay_ms);
uint32_t HAL_GetTick(void);
void System_Reboot(void)
{
	SystemSoftwareReset();
   // HAL_NVIC_SystemReset();
}



static uint8_t IapUpg_Checksum(uint8_t *buf, uint16_t len)
{
    uint32_t sum = 0;

    if(buf)
    {
        while(len--)
        {
            sum += buf[len];
        }
    }

    return (uint8_t)(sum & 0xFF);
}


uint8_t echo_char(uint32_t instance, uint8_t c)
{
	uint32_t bytesRemaining;
	uint8_t ch = c;
	LPUART_DRV_SendData(1, &ch, 1ul);
	while(LPUART_DRV_GetTransmitStatus(1, &bytesRemaining) != STATUS_SUCCESS);
	return 0;
}

extern uint8_t Uart_Get_Char (uint32_t instance, uint8_t* ptr);
static uint8_t IapUpg_GetChar(uint8_t *c)
{
	uint8_t ret=0;
	//echo_char(INST_LPUART2,0x19);
	ret = Uart_Get_Char(INST_LPUART2, c);
	//echo_char(INST_LPUART2,ret);
	//echo_char(INST_LPUART2,*c);
	return ret;
}



static uint8_t IapUpg_PutChar(uint8_t c)
{
	uint32_t bytesRemaining;
	uint8_t ch = c;
	LPUART_DRV_SendData(INST_LPUART2, &ch, 1ul);
	while(LPUART_DRV_GetTransmitStatus(INST_LPUART2, &bytesRemaining) != STATUS_SUCCESS);
	return 0;
}

static uint8_t IapUpg_RomFlashUnlock(void)
{
    //HAL_FLASH_Unlock();
    return 0;
}

static uint8_t IapUpg_RomFlashLock(void)
{
    //HAL_FLASH_Lock();
    return 0;
}

static uint8_t IapUpg_RomFlashErase(void)
{
   /* FLASH_EraseInitTypeDef EraseInit;
    static uint32_t PageError;

    //HAL_FLASH_Unlock();//add by scb
    EraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInit.PageAddress = ROM_FLASH_ADDRESS;
    EraseInit.NbPages = 50;// 128k -16k - 12K = 100k/2k=50 pages

    HAL_FLASHEx_Erase(&EraseInit, &PageError);

    return 1;*/
	status_t ret;
	INT_SYS_DisableIRQGlobal();
    ret = FLASH_DRV_EraseSector(&flashSSDConfig,ROM_FLASH_ADDRESS,FEATURE_FLS_PF_BLOCK_SECTOR_SIZE*ROM_APP_SECTOR_CNT);//512k-0x8000=120*4k
    INT_SYS_EnableIRQGlobal();
    //echo_char(INST_LPUART1, 0x00);
    //echo_char(INST_LPUART1, ret%256);
    //echo_char(INST_LPUART1, 0x11);
    return ret;
}
 uint8_t t_IapUpg_RomFlashErase(void)
 {
	 status_t ret;
	 ret = IapUpg_RomFlashErase();
	 return ret;
 }

/*static uint8_t IapUpg_RomFlashWrWord(uint32_t offset, uint32_t data)
{
    /*HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, ROM_FLASH_ADDRESS + offset, data);
    return 0;*/
/*	status_t ret;
	ret = FLASH_DRV_Program(&flashSSDConfig, ROM_FLASH_ADDRESS + offset, 4,&data );
	if (ret != STATUS_SUCCESS)
	{
	     return ret;
	}
	return 0;
}*/

static uint8_t IapUpg_RomFlashWrDWord(uint32_t offset, uint8_t * data)
{
    /*HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, ROM_FLASH_ADDRESS + offset, data);
    return 0;*/
	status_t ret;
	INT_SYS_DisableIRQGlobal();
	ret = FLASH_DRV_Program(&flashSSDConfig, ROM_FLASH_ADDRESS + offset, 8,data );
	INT_SYS_EnableIRQGlobal();
	if (ret != STATUS_SUCCESS)
	{
	     return ret;
	}
	return 0;
}

 uint8_t t_IapUpg_RomFlashWrDWord(uint32_t offset, uint8_t * data)
{
	uint8_t ret;
	ret = IapUpg_RomFlashWrDWord( offset,  data);
	return ret;
}

static uint8_t IapUpg_RomFlashWrHalfWord(uint32_t offset, uint16_t data)
{
    //HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, ROM_FLASH_ADDRESS + offset, data);
    return 0;
}

extern volatile uint8_t UpSuccess;
//extern uint8_t pRxBuffPtr_uart1[200];
void IapUpg_Proc(void)
{
    uint32_t timeout;
    uint8_t input = 0;
    uint8_t cnt=0, i;

    uint8_t upgState = UPG_STT_CHECK_START;

    uint16_t packRcvCnt;
    uint16_t packDataLen;
    uint32_t binRcvCnt;
    uint8_t checksum;
    uint8_t allChecksum;

    uint8_t buf[8];
    uint32_t *pData;
    uint16_t *pU16Data;

    uint32_t writedCnt;
    uint8_t leftLen;
    uint32_t writeLen;

    uint8_t upgSuccess = 0;
    uint8_t r_start= 0xff,shakehand=0xfe,shakesucess=0xee,end=0xfd,end1=0xfc,end2=0xfb,end3=0xfa;

    //gUartHandle = MX_USART_Open(UPG_UART_CHN, gUartRxRingBuf, sizeof(gUartRxRingBuf));//串口设置  rxbuf   handler ...
    //if(gUartHandle == NULL)
   // {
       // return;
   // }
    //for(;;)
    {
    	// if(!IapUpg_GetChar(&input))
    		// echo_char(INST_LPUART2, input);
    }

#if 0
    while(0)
    {
    	//LPUART_DRV_ReceiveData(INST_LPUART1,&input , 1UL);
    	IapUpg_GetChar(&input);
    	if(input != 0)
    	{
    		echo_char(INST_LPUART1,input );
    	}

    }
#endif
    timeout = HAL_GetTick() + IAPUPG_CHECK_UART_TIME;
   // echo_char(INST_LPUART1, 0x22);
    while(HAL_GetTick() < timeout)//每经历一个状态，增加timeout 值，似的可以继续进入下一个状态；若在同一 状态等待超时，则退出。
    {

        if(IapUpg_GetChar(&input))//更新状态
        {

        	//if(input != 0)
        	 // echo_char(INST_LPUART1,input );
        	//echo_char(INST_LPUART2, upgState);

            switch(upgState)
            {
                case UPG_STT_CHECK_START:
                    if(input == REQ_START)
                    {
                        upgState = UPG_STT_START;
                        timeout = HAL_GetTick() + IAPUPG_CHECK_UART_TIME * 10;
                        IAPUPG_HINT();
                        cnt = 1;
                    }
                break;
                case UPG_STT_START:
                	//echo_char(INST_LPUART2,input );
                    if(input == REQ_START)
                    {
                    	//echo_char(INST_LPUART1,r_start);
                        if(++cnt > 15)
                        {
                        	//RingBuf_Reset(pRxBuffPtr_uart1, 200);
                        	//ReadCnt = 0;
                            IapUpg_PutChar(RSP_START);
                            //echo_char(INST_LPUART1,RSP_START);
                            upgState = UPG_STT_SHAKEHANDS;
                            timeout = HAL_GetTick() + 3000;
                            IAPUPG_HINT();
                            cnt = 0;
                        }
                    }
                    else
                    {

                    	//echo_char(INST_LPUART1,end1);
                        return;
                    }
                break;
                case UPG_STT_SHAKEHANDS:
                    if(input == REQ_SHAKEHANDS)
                    {
                    	//echo_char(INST_LPUART1,shakehand);
                        if(++cnt > 15)
                        {
                            IapUpg_PutChar(RSP_SHAKEHANDS);
                            //echo_char(INST_LPUART1,shakesucess);
                            upgState = UPG_STT_FLASH_ERASED;
                            IapUpg_RomFlashUnlock();
                            //echo_char(INST_LPUART1,end2);

                            if(IapUpg_RomFlashErase())
                            {

                                IapUpg_PutChar(RSP_FLASH_ERASE_FAIL);
                                //echo_char(INST_LPUART1,RSP_FLASH_ERASE_FAIL);
                                goto RET;
                            }

                            HAL_Delay(500);
                           while(IapUpg_GetChar(&input))
                           {
                        	   //echo_char(INST_LPUART1,0x33);
                           }
                           IapUpg_PutChar(RSP_FLASH_ERASE_OK);
                           upgState = UPG_STT_RCV_DATA;
                           HAL_Delay(100);
                            timeout = HAL_GetTick() + 10000;
                            IAPUPG_HINT();
                            packRcvCnt = 0;
                            binRcvCnt = 0;
                            leftLen = 0;
                            cnt = 0;
                            allChecksum = 0;
                        }
                    }
                    else if(input == REQ_START)
                    {
                    	//timeout = HAL_GetTick() + IAPUPG_CHECK_UART_TIME ;
                    }
                    else
                    {
                    	//echo_char(INST_LPUART1,end);
                        return;
                    }
                break;
                case UPG_STT_FLASH_ERASED:
                break;
                case UPG_STT_RCV_DATA:
                	//echo_char(INST_LPUART1,0x20);
                    if((input == REQ_SHAKEHANDS) && (packRcvCnt == 0) && (binRcvCnt == 0))
                    {
                        //skip SHAKEHANDS byte
                    	//echo_char(INST_LPUART1,0x21);
                        break;
                    }

                    if(packRcvCnt == 0)
                    {
                    	//echo_char(INST_LPUART2,input);
                        packDataLen = input;//第一个包 数据长度
                        packRcvCnt++;
                        break;
                    }

                    gPackBuf[packRcvCnt - 1] = input;//开始接收数据  第二个 gPackBuf[0]
                    packRcvCnt++;
                    //timeout = HAL_GetTick() + 1000;//每接手一个数据，重置超时。
                    //echo_char(INST_LPUART2,packRcvCnt);
                    if((2 == packRcvCnt)
                    && (packDataLen == REQ_DATA_SEND_END)
                    && (gPackBuf[0] == REQ_DATA_SEND_END)) //all pack send
                    {
                    	//echo_char(INST_LPUART1,0x41);
                        if(leftLen != 0)
                        {
                            for(i = leftLen; i < 8; i++)
                            {
                                buf[i] = 0xFF;
                            }
                            //pData = (uint32_t *)buf;
                            IapUpg_RomFlashWrDWord(binRcvCnt - leftLen, buf);
                        }
                        IapUpg_PutChar(RSP_DATA_SEND_END);
                        upgState = UPG_STT_BIN_VERIFY;
                        timeout = HAL_GetTick() + 10000;
                        IAPUPG_HINT();
                        packRcvCnt = 0;
                        packDataLen = 0;
                        break;
                    }
                    //echo_char(INST_LPUART1,packRcvCnt);
                    //echo_char(INST_LPUART1,packDataLen);
                    if(packRcvCnt == (packDataLen + 2)) // checksum
                    {
                        //do checksum verify
                    	//echo_char(INST_LPUART1,0x41);
                        checksum = IapUpg_Checksum(gPackBuf, packDataLen);

                        if(checksum != gPackBuf[packDataLen])//gPackBuf 最后一个是checksum 每一个包的check
                        {
                            IapUpg_PutChar(RSP_DATA_VERIFY_FAIL);
                            //echo_char(INST_LPUART1,RSP_DATA_VERIFY_FAIL);
                            goto RET;
                        }

                        allChecksum += checksum;//所以包check

                        writedCnt = 0;
                        if(leftLen != 0)
                        {
                            if((packDataLen + leftLen) >= 8)
                            {
                                writedCnt = (8 - leftLen);
                                for(i = 0; (i + leftLen) < 8; i++)
                                {
                                    buf[i + leftLen] = gPackBuf[i];
                                }
                                //pData = (uint32_t *)buf;
                                IapUpg_RomFlashWrDWord(binRcvCnt - leftLen, buf);
                                leftLen = 0;

                            }
                            else
                            {
                                writedCnt = packDataLen;
                                for(i = 0; i < writedCnt; i++)
                                {
                                    buf[i + leftLen] = gPackBuf[i];
                                }
                                leftLen += writedCnt;
                            }
                        }

                        //pData = (uint32_t*)&gPackBuf[writedCnt];
                        while(packDataLen >= (writedCnt + 8))
                        {
                            IapUpg_RomFlashWrDWord(binRcvCnt + writedCnt, &gPackBuf[writedCnt]);
                            writedCnt += 8;
                            //pData++;
                        }

                        if(packDataLen > writedCnt)
                        {
                            leftLen = packDataLen - writedCnt;
                            /*if(leftLen == 2)
                            {
                            	for(i = 0; i < 8; i++)
                            	{
                            		buf[i]=0xff;
                            	}
                            	buf[0]=gPackBuf[writedCnt];
                            	buf[1]=gPackBuf[writedCnt+1];
                               // pU16Data = (uint16_t*)&gPackBuf[writedCnt];//ttt
                                //IapUpg_RomFlashWrHalfWord(binRcvCnt + writedCnt, *pU16Data);
                            	IapUpg_RomFlashWrDWord(binRcvCnt + writedCnt, buf);
                                writedCnt += 2;
                                leftLen = 0;
                                for(i = 0; i < 8; i++)
                                {
                                  buf[i] = 0;
                                }
                            }*/
                            //else
                            {
                                for(i = 0; i < leftLen; i++)
                                {
                                    buf[i] = gPackBuf[writedCnt + i];
                                }
                            }
                        }

                        IapUpg_PutChar(RSP_DATA_RCV_SAVE_OK);

                        HAL_Delay(100);

                        binRcvCnt += packDataLen;
                        packDataLen = 0;
                        packRcvCnt = 0;
                        timeout = HAL_GetTick() + 10000;
                        IAPUPG_HINT();

                    }

                break;
                case UPG_STT_BIN_VERIFY:
                    gPackBuf[packDataLen++] = input;
                    if(packDataLen >= 2)
                    {

                        checksum = IapUpg_Checksum((uint8_t *)(ROM_FLASH_ADDRESS), binRcvCnt);
                        if(((gPackBuf[0] << 8) | gPackBuf[1]) != (binRcvCnt & 0xFFFF) || (checksum != allChecksum))
                        {
                            IapUpg_PutChar(RSP_BIN_SIZE_VERIFY_FAIL);
                            break;
                        }

                        IapUpg_PutChar(RSP_BIN_SIZE_VERIFY_OK);
                        upgState = UPG_STT_WAIT_END;
                        timeout = HAL_GetTick() + 10000;
                        IAPUPG_HINT();
                        packDataLen = 0;
                        packRcvCnt = 0;
                    }
                break;
                case UPG_STT_WAIT_END:
                    gPackBuf[packDataLen++] = input;
                    if((packDataLen >= 2) && (REQ_BIN_SEND_END == gPackBuf[0]) && (REQ_BIN_SEND_END == gPackBuf[1]))
                    {
                        IapUpg_PutChar(RSP_UPG_FINISH);
                        IAPUPG_HINT();
                        HAL_Delay(500);
                        upgSuccess = 1;
                        goto RET;
                    }
                break;

            }


        }
        else
        {
        	//echo_char(INST_LPUART2,end2);
            HAL_Delay(5);
        }

    }

RET:

    if((!upgSuccess) && (HAL_GetTick() > timeout))
    {
        IapUpg_PutChar(RSP_TIMEOUT);
    }

    IapUpg_RomFlashLock();

    IAPUPG_HINT();

    if(upgState > UPG_STT_SHAKEHANDS)
    {
        System_Reboot();
        UpSuccess = 1;
    }
    echo_char(INST_LPUART1,end3);
    return;


}

