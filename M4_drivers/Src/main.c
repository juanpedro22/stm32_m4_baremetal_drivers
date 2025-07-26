#include <stdio.h>
#include "led.h"
#include "uart.h"

int main(void)
{
	led_init();
	uart_tx_init();

	while (1)
	    {
	        if (frase_completa)
	        {
	            frase_completa = false;

	            // Aqui você pode processar a string recebida
	            printf("Recebido: %s\r\n", rx_buffer);
	        }

	        // Outras tarefas...
	    }

}
