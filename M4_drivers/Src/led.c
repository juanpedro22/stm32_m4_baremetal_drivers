#include "led.h"

#define GPIODEN (1U<<3)
#define LED_PIN (1U<<15)

void led_init(void)
{
	/*Enable clock access for port D*/
	RCC->AHB1ENR |= GPIODEN;
	 /* Set LED pin as output*/
	GPIOD->MODER |= (1U<<30);
	GPIOD->MODER &= ~(1U<<31);

}

void led_on(void)
{
	/*set led on pin PD15*/
	GPIOD->ODR |= LED_PIN;
}

void led_off(void)
{
	/*set led on pin PD15*/
	GPIOD->ODR &= ~LED_PIN;
}
