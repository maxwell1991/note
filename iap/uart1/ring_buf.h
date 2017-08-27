/* $Header:   O:\pvcs\ibs_software\07_inr_dvd\cc0177\src\ring_buf.h_v   1.0   20 Sep 2004 10:33:20   Lee_Long  $*/

#ifndef  Ring_Buf_H_FILE
#define  Ring_Buf_H_FILE 1

/**********************************************************************
 *
 *
 *********************************************************************/
/**********************************************************************
 *             Title:   Ring_Buf.H
 *
 *       Description:   
 *
 *********************************************************************/

/**********************************************************************
 * Include files                                                       
 *********************************************************************/
#include <stdint.h>

/**********************************************************************
 * Global Constant and Macro Definitions using #define                        
 *********************************************************************/

/**********************************************************************
 * Global Enumerations and Structures and Typedefs                          
 *********************************************************************/
typedef struct RingBuf_s
{
   uint16_t    in;            /* index of next array element to add to */
   uint16_t    out;           /* index of oldest element to remove */
   uint16_t    size;          /* number of elements in ring */
   uint8_t  full;          /* Flag to indicate between full and empty buffer */
} RingBuf_t;

/**********************************************************************
 * Global Variable extern Declarations                               
 *********************************************************************/

/**********************************************************************
 * Global Function Prototypes                                               
 *********************************************************************/
 
extern void RingBuf_Reset(RingBuf_t *buf, uint16_t size);
/**********************************************************************
 *    Purpose: Reset / Initialize ring buffer to empty
 * Parameters: Pointer to ring buf control structure
 *    Returns: None
 *********************************************************************/

extern uint8_t RingBuf_IsEmpty(RingBuf_t *buf);
/**********************************************************************
 *    Purpose: returns whether buffer is empty
 * Parameters: Pointer to ring buf control structure
 *    Returns: TRUE if buffer is empty
 *********************************************************************/

extern uint8_t RingBuf_IsFull(RingBuf_t *buf);
/**********************************************************************
 *    Purpose: returns wether buffer is full
 * Parameters: Pointer to ring buf control structure
 *    Returns: TRUE if buffer is full
 *********************************************************************/

extern void RingBuf_Add(RingBuf_t *buf);
/**********************************************************************
 *    Purpose: Logical adds element to ring buffer 
 * Parameters: Pointer to ring buf control structure
 *    Returns: None
 *********************************************************************/

extern void RingBuf_Remove(RingBuf_t *buf);
/**********************************************************************
 *    Purpose: Logical remove element  from ring buffer 
 * Parameters: Pointer to ring buf control structure
 *    Returns: None
 *********************************************************************/

/**********************************************************************
 *
 * Modification Record
 *
 **********************************************************************
 * Initial revision.
 * 
 *********************************************************************/
#endif
