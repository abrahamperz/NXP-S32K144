#include "S32K144.h" /* include peripheral declarations S32K144 */
#define T 4000000

unsigned char duty_cycle=62;
unsigned char espejo_pin;
unsigned char CW=1;

void LPIT0_Ch0_IRQHandler (void)
{
	LPIT0->MSR=1;					//Borrar bandera
	LPIT0->TMR[0].TCTRL&=~(1<<0);
	if (CW==1) PTC->PTOR=(1<<7);
	else PTC->PTOR=(1<<15);

	if (espejo_pin==1)
	{
		espejo_pin=0;
		LPIT0->TMR[0].TVAL=(((100-(unsigned int)duty_cycle)*T)/100);
	}
	else
	{
		espejo_pin=1;
		LPIT0->TMR[0].TVAL=((((unsigned int)duty_cycle)*T)/100);
	}
	LPIT0->TMR[0].TCTRL|=(1<<0);		//Enable->Trigger
}

int main(void)
{
	PCC->PCCn[PCC_PORTC_INDEX]=(1<<30);
	PORTC->PCR[7]=(1<<8);
	PORTC->PCR[15]=(1<<8);
	PTC->PDDR=(1<<7)+(1<<15);
	PTC->PDOR=0;

	espejo_pin=0;

	PCC->PCCn[PCC_LPIT_INDEX]=(2<<24);

	PCC->PCCn[PCC_LPIT_INDEX]|=(1<<30);
	SCG->SIRCDIV=(1<<8);  //8MHz/1

	LPIT0->MCR=1;		//M_CEN=1;
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	LPIT0->TMR[0].TCTRL=(0<<24)+(1<<23)+(1<<17)+(1<<16);
	                                            //TCTRL para el LPIT0->TMR0
												// Bit 24: 0 para TMR0, 1 TMR1, 2 TMR2, 3 TMR3
												// Bit 18 TROT=0
												// Bit 17 TSOI=1
	LPIT0->TMR[0].TVAL=(((100-(unsigned int)duty_cycle)*T)/100);
	LPIT0->MIER=1;					//Interrupt Enable TMR0
	LPIT0->TMR[0].TCTRL|=1;			//Hab conteo

	S32_NVIC->ISER[48/32]=(1<<(48%32));

	while (1);

	return 0;
}
