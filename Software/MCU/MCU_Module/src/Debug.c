#include "Debug.h"

void USART2Init()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    /* USART IOs configuration ***********************************/

    /* Enable GPIOC clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    /* Connect PC10 to USART3_Tx */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_1);
    /* Connect PC11 to USART3_Rx*/
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_1);
    /* Configure USART3_Tx and USART3_Rx as alternate function */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
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
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl =
    USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure);
    /* Enable USART3 */
    USART_Cmd(USART2, ENABLE);

    //enable USART interrupt on receive
    USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
    //NVIC_EnableIRQ(USART2_IRQn);

    NVIC_InitTypeDef NVIC_InitStructure;
    /* Enable USART3 IRQ channel in the NVIC controller.
    When the USART3 interrupt is generated (in this example when
    data is received) the USART3_IRQHandler will be served */
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

}

/*
debug input button
*/
void input(){
    GPIO_InitTypeDef GPIO_InitStructure;
    //MODE
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*
print the address obtained by the initialization
*/
void printBIN(uint8_t val){
    int i;
    for( i = 0; i < 8 ; i++){
        if(val & (0b10000000 >> i)){
            USART_SendData(USART2, 0x31);
            /* Loop until the end of transmission */
            while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET){}
        }else{
            USART_SendData(USART2, 0x30);
            /* Loop until the end of transmission */
            while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET){}
        }
    }
    USART_SendData(USART2, 0x0A);
    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET){}
}

/*
output a value on the serial monitor
*/
void print16bits(uint16_t value, uint8_t pre, uint8_t len){
    USART_SendData(USART2, pre);
    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET){}
    USART_SendData(USART2, 0x0A);
    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET){}
    uint8_t split[5];
    split[4] = (value/10000);
    split[3] = (value - (split[4]*10000))/1000;
    split[2] = (value - split[4]*10000 - split[3]*1000)/100;
    split[1] = (value - split[4]*10000 - split[3]*1000 - split[2]*100)/10;
    split[0] = (value - split[4]*10000 - split[3]*1000 - split[2]*100 - split[1]*10);
    int i;
    for(i = len ; i > 0  ; i--){
        USART_SendData(USART2, split[i-1] + 48);
        /* Loop until the end of transmission */
        while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET){}
    }
    USART_SendData(USART2, 0x0A);
    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET){}
}

/*
print 1 byte
*/
void printbyte(uint8_t val){
    USART_SendData(USART2, val);
    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET){}
    USART_SendData(USART2, 0x0A);
    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET){}
}

/*
test the adc
*/
void ADCtest(){
    /*ADC1->CHSELR |= ADC_CHSELR_CHSEL0; // select channel 0
    delay(200);
    ADC1->CR |= ADC_CR_ADSTART;
    // wait for end of conversion: EOC == 1. Not necessary to clear EOC as we read from DR
    while((ADC1->ISR & ADC_ISR_EOC) == 0);
    print16bits(ADC1->DR/4,0x41,3);*/

    int i;

    for(i = 0; i < 5 ; i++){
        ADC1->CHSELR = ADC_CHSELR_CHSEL8; // select channel 8
        ADC1->CR |= ADC_CR_ADSTART;
        // wait for end of conversion: EOC == 1. Not necessary to clear EOC as we read from DR
        while((ADC1->ISR & ADC_ISR_EOC) == 0);
        print16bits(ADC1->DR,0x42,3);
    }

    for(i = 0; i < 5 ; i++){
        ADC1->CHSELR = ADC_CHSELR_CHSEL9; // select channel 9
        ADC1->CR |= ADC_CR_ADSTART;
        // wait for end of conversion: EOC == 1. Not necessary to clear EOC as we read from DR
        while((ADC1->ISR & ADC_ISR_EOC) == 0);
        print16bits(ADC1->DR,0x43,3);
    }

}

// USART 2 IRQ handler

