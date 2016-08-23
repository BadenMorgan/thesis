/*
Manage and setup the serial communications
*/

#include "SerialMonitor.h"

/*
setup USART1 communications
*/
void USART1Init()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    /* USART IOs configuration ***********************************/

    /* Enable GPIOC clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    /* Connect PC10 to USART3_Tx */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);
    /* Connect PC11 to USART3_Rx*/
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_1);
    /* Configure USART3_Tx and USART3_Rx as alternate function */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    /* USART configuration ***************************************/
    /* USART3 configured as follow:
    - BaudRate = 115200 baud
    - Word Length = 8 Bits
    - One Stop Bit
    - No parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Receive and transmit enabled
    */
    /* Enable USART3 clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl =
    USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);
    /* Enable USART3 */
    USART_Cmd(USART1, ENABLE);

    //enable USART interrupt
    NVIC_EnableIRQ(USART1_IRQn);
}

/*
USART incoming data handler
*/
void USART1_IRQHandler(){

}


