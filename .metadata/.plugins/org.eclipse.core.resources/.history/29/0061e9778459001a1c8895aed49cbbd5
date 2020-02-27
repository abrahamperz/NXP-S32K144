#include "S32K144.h" /* include peripheral declarations S32K144 */

unsigned long secuencia[4]={0x00004000,0x00008000, 0x00010000, 0x00020000};

void delay (void)
{
unsigned long i=1000000;
do{}while (i--);
}

int main(void)
{
unsigned char i;

		PCC->PCCn[PCC_PORTB_INDEX]=0x40000000;

		PORTB->PCR[17]=0x00000100;
		PORTB->PCR[16]=0x00000100;
		PORTB->PCR[15]=0x00000100;
		PORTB->PCR[14]=0x00000100;

		PTB->PDDR=0x0003C000;

		PCC->PCCn[PCC_PORTC_INDEX]=0x40000000;
		PORTC->PCR[13]=0x00000100;

        for(;;) {
        	if ((PTC->PDIR & (0x00002000))==0) PTB->PDOR=secuencia[(i++)%4];
        	else PTB->PDOR=secuencia[(i--)%4];
        	delay();
        }

	return 0;
}
