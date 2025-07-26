#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "uart.h"
#include "stm32f4xx.h"

#define MAX_BUFFER 100


#define SYS_FREQ				16000000
#define APB1_CLK				SYS_FREQ
#define UART_BAUDRATE		    115200

#define GPIOAEN (1U<<0)
#define UART2EN (1U<<17)
//tx
#define CR1_TE					(1U<<3)
#define CR1_UE					(1U<<13)
#define SR_TXE					(1U<<7)

//rx
#define CR1_RE     (1U << 2)     // Receiver enable
#define CR1_RXNEIE (1U << 5)     // RXNE interrupt enable
#define SR_RXNE    (1U << 5)     // Receive buffer not empty


static uint16_t compute_uart_bd(uint32_t periph_clk,uint32_t baudrate);
static void uart_set_baudrate(uint32_t periph_clk,uint32_t baudrate);
static void uart_write(int ch);
void USART2_IRQHandler(void);

volatile char rx_buffer[MAX_BUFFER];
volatile uint8_t rx_index = 0;
volatile bool frase_completa = false;

int __io_putchar(int ch)
{
	uart_write(ch);
	return ch;
}

void uart_tx_init(void)
{
	/*enable clock access to GPIOA*/
	RCC->AHB1ENR |= GPIOAEN;
	/*set PA2 to alternate function*/
	GPIOA->MODER &=~(1U<<4);
	GPIOA->MODER |= (1U<<5);

    // PA3  to alternate function*/ (RX)
    GPIOA->MODER &= ~(1U << 6);
    GPIOA->MODER |=  (1U << 7);

	/*Set alternate function type to AF7 uart2_tx*/
	GPIOA->AFR[0] |= (1U<<8);
	GPIOA->AFR[0] |= (1U<<9);
	GPIOA->AFR[0] |= (1U<<10);
	GPIOA->AFR[0] &=~(1U<<11);

	/*Set alternate function type to AF7 uart2_tx*/
    GPIOA->AFR[0] |=  (1U << 12) | (1U << 13) | (1U << 14);
    GPIOA->AFR[0] &= ~(1U << 15);

	/*enable clock access to UART2*/
	RCC->APB1ENR |= UART2EN;

	/*Configure uart baudrate*/
	uart_set_baudrate(APB1_CLK,UART_BAUDRATE);

	/*Configure transfer direction*/
	USART2->CR1 = CR1_TE| CR1_RE | CR1_RXNEIE | CR1_UE;;

	/*Enable uart module*/
	USART2->CR1 |= CR1_UE;

	// Habilita interrupção USART2 no NVIC
	NVIC_EnableIRQ(USART2_IRQn);
}

static void uart_write(int ch)
{
	/*Make sure transmit data register is empty*/
	while(!(USART2->SR & SR_TXE)){}

	/*Write to transmit data register*/
	USART2->DR =(ch & 0xFF);
}


static uint16_t compute_uart_bd(uint32_t periph_clk,uint32_t baudrate)
{
	return ((periph_clk + (baudrate/2U))/baudrate);
}

static void uart_set_baudrate(uint32_t periph_clk,uint32_t baudrate)
{
	USART2->BRR = compute_uart_bd(periph_clk,baudrate);
}

void USART2_IRQHandler(void)
{
    if (USART2->SR & SR_RXNE)
    {
        char c = USART2->DR;

        // Ignora múltiplos delimitadores
        static bool esperando_lf = false;

        if (esperando_lf && c == '\n') {
            esperando_lf = false;
            return;
        }

        if (c == '\r' || c == '\n') {
            rx_buffer[rx_index] = '\0';
            frase_completa = true;
            rx_index = 0;

            if (c == '\r')
                esperando_lf = true;
        }
        else {
            if (rx_index < MAX_BUFFER - 1) {
                rx_buffer[rx_index++] = c;
            } else {
                rx_index = 0; // Buffer cheio, resetar
            }
        }
    }
}

