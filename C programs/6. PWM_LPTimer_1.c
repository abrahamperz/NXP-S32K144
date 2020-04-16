#include "S32K144.h" /* include peripheral declarations S32K144 */
#define T 500

unsigned char duty_cycle=30;
unsigned char espejo_pin;

void LPTMR0_IRQHandler (void)
{
	LPTMR0->CSR|=(1<<7);
	PTC->PTOR=(1<<7);

	if (espejo_pin==1)
	{
		espejo_pin=0;
		LPTMR0->CMR=(((100-(unsigned int)duty_cycle)*T)/100)-1;
	}
	else
	{
		espejo_pin=1;
		LPTMR0->CMR=((((unsigned int)duty_cycle)*T)/100)-1;
	}
}

int main(void)
{
	//Clock
	PCC->PCCn[PCC_PORTC_INDEX]=(1<<30);
	//GPIO
	PORTC->PCR[7]=(1<<8);
	PTC->PDDR=(1<<7);
	PTC->PDOR=0;
	espejo_pin=0;

	PCC->PCCn[PCC_LPTMR0_INDEX]=(1<<30);
	LPTMR0->PSR=5;   //Bypass prescaler, CLS=01 LPO (1KHz)
	
	LPTMR0->CMR=(((100-(unsigned int)duty_cycle)*T)/100)-1;
	LPTMR0->CSR=(1<<6)+1;       //Hab intr, enable
	S32_NVIC->ISER[58/32]=(1<<(58%32));

	while (1);

	return 0;
}
