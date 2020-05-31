/*
 *
 *
 * Activamos con pulsos el puente h
 *
 * Motor derecho PORTC
 * 7 adelante
 * 15 atras
 *
 * Motor izquierdo PORTC
 * 14 adelante
 * 17 atras
 *
 * RX Uart
 * A Adelante
 * B atras
 * C derecha
 * D izquierda
 * E alto
 */


#include "S32K144.h" /* include peripheral declarations S32K144 */

#define adelante (1<<14)+(1<<7) //Seleccionamos pines 7 y 14;
#define atras (1<<17)+(1<<15) //Seleccionamos pines 15 y 17;
#define derecha (1<<17)+(1<<7) //Seleccionamos pines 7 y 17;
#define izquierda (1<<15)+(1<<14) //Seleccionamos pines 15 y 14;
#define alto (0<<17)+(0<<15)+(0<<14)+(0<<7)


unsigned char duty_cycle=50;
unsigned char espejo_pin;
unsigned char i=0;
unsigned char T=100;
unsigned char data=0;
unsigned char dato=0;

unsigned long ruta[3];

void LPUART1_RxTx_IRQHandler (void)
{

	if ((LPUART1->STAT & (1<<21))==(1<<21))   //Rx
	{
		LPUART2->DATA=dato;
		if (dato!='2') LPUART2->CTRL&=~(1<<23);

	}

	if ((LPUART1->STAT & (1<<23))==(1<<23)) //TX
	{
		data=LPUART2->DATA;
		if (data == 'A') LPIT0->TMR[0].TCTRL|=1;
	    else if (data == 'B') LPIT0->TMR[1].TCTRL|=1;
	    else if (data == 'C') LPIT0->TMR[2].TCTRL|=1;
	    else if (data == 'D') LPIT0->TMR[3].TCTRL|=1;
	    else if (data == 'E')  PCC->PCCn[PCC_LPIT_INDEX]|=(0<<30);


     }
}

void UART_init(void)
{

	PCC->PCCn[PCC_PORTC_INDEX]=1<<30;
	PORTC->PCR[7]=2<<8;						//LPUART1 TX
	PORTC->PCR[6]=2<<8;						//LPUART1 RX

	SCG->SIRCDIV=1<<8;						//SIRCDIV2: 8 MHz/1

	PCC->PCCn[PCC_LPUART1_INDEX]=2<<24;		//SIRCDIV2
    PCC->PCCn[PCC_LPUART1_INDEX]|=1<<30;
	LPUART1->BAUD|=52;						//BAUD_SRG=CLK_UART/(16*9600)
	LPUART1->CTRL|=(1<<21)+(1<<19)+(1<<18);	//TE=RE=1
	S32_NVIC->ISER[33/32]=(1<<(33%32));

}

void PORTC_init(void)
{
	PCC->PCCn[PCC_PORTC_INDEX]=(1<<30);
	PORTC->PCR[7]=(9<<16)+(1<<8);
	PORTC->PCR[15]=(9<<16)+(1<<8);
	PORTC->PCR[17]=(9<<16)+(1<<8);
	PORTC->PCR[14]=(9<<16)+(1<<8);
	PTC->PDDR=(1<<7)+(1<<15)+(1<<17)+(1<<14);
	PTC->PDOR=0;
}
void LPIT0_Ch0_init(void)
{

	PCC->PCCn[PCC_LPIT_INDEX]=(2<<24);
    PCC->PCCn[PCC_LPIT_INDEX]|=(1<<30);
	SCG->SIRCDIV=(1<<8);  //8MHz/1

	LPIT0->MCR=1;		//M_CEN=1;
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	LPIT0->TMR[0].TCTRL=(0<<24)+(1<<23)+(1<<17)+(1<<18);
	LPIT0->TMR[0].TVAL=T;
	LPIT0->MIER=1;
	//LPIT0->TMR[0].TCTRL|=1;
	S32_NVIC->ISER[48/32]=(1<<(48%32));

}
void LPIT0_Ch0_IRQHandler (void)
{

LPIT0->MSR|=1;
//Borrar bandera
LPIT0->TMR[0].TCTRL&=~(1<<0);
PTC->PTOR=adelante;

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
LPIT0->TMR[0].TCTRL|=(1<<0);       //Enable->Trigger
}

void LPIT0_Ch1_init(void)
{

	PCC->PCCn[PCC_LPIT_INDEX]=(2<<24);
    PCC->PCCn[PCC_LPIT_INDEX]|=(1<<30);
	SCG->SIRCDIV=(1<<8);  //8MHz/1

	LPIT0->MCR=1;		//M_CEN=1;
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	LPIT0->TMR[1].TCTRL=(0<<24)+(1<<23)+(1<<17)+(1<<18);
	LPIT0->TMR[1].TVAL=T;
	LPIT0->MIER|=(1<<2);
	//LPIT0->TMR[0].TCTRL|=1;
	S32_NVIC->ISER[49/32]=(1<<(49%32));

}
void LPIT0_Ch1_IRQHandler (void)
{

LPIT0->MSR|=(1<<2);
//Borrar bandera
LPIT0->TMR[1].TCTRL&=~(1<<0);
PTC->PTOR=atras;

if (espejo_pin==1)
{
espejo_pin=0;
LPIT0->TMR[1].TVAL=(((100-(unsigned int)duty_cycle)*T)/100);
}
else
{
espejo_pin=1;
LPIT0->TMR[1].TVAL=((((unsigned int)duty_cycle)*T)/100);
}
LPIT0->TMR[1].TCTRL|=(1<<0);       //Enable->Trigger
}

void LPIT0_Ch2_init(void)
{

	PCC->PCCn[PCC_LPIT_INDEX]=(2<<24);
    PCC->PCCn[PCC_LPIT_INDEX]|=(1<<30);
	SCG->SIRCDIV=(1<<8);  //8MHz/1

	LPIT0->MCR=1;		//M_CEN=1;
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	LPIT0->TMR[2].TCTRL=(0<<24)+(1<<23)+(1<<17)+(1<<18);
	LPIT0->TMR[2].TVAL=T;
	LPIT0->MIER|=(1<<3);
	//LPIT0->TMR[0].TCTRL|=1;
	S32_NVIC->ISER[50/32]=(1<<(50%32));

}
void LPIT0_Ch2_IRQHandler (void)
{

LPIT0->MSR|=(1<<3);
//Borrar bandera
LPIT0->TMR[2].TCTRL&=~(1<<0);
PTC->PTOR=derecha;

if (espejo_pin==1)
{
espejo_pin=0;
LPIT0->TMR[2].TVAL=(((100-(unsigned int)duty_cycle)*T)/100);
}
else
{
espejo_pin=1;
LPIT0->TMR[2].TVAL=((((unsigned int)duty_cycle)*T)/100);
}
LPIT0->TMR[2].TCTRL|=(1<<0);       //Enable->Trigger
}

void LPIT0_Ch3_init(void)
{


	PCC->PCCn[PCC_LPIT_INDEX]=(2<<24);
    PCC->PCCn[PCC_LPIT_INDEX]|=(1<<30);
	SCG->SIRCDIV=(1<<8);  //8MHz/1

	LPIT0->MCR=1;		//M_CEN=1;
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	LPIT0->TMR[3].TCTRL=(0<<24)+(1<<23)+(1<<17)+(1<<18);
	LPIT0->TMR[3].TVAL=T;
	LPIT0->MIER|=(1<<4);
	//LPIT0->TMR[0].TCTRL|=1;
	S32_NVIC->ISER[51/32]=(1<<(51%32));

}
void LPIT0_Ch3_IRQHandler (void)
{

LPIT0->MSR|=(1<<4);
//Borrar bandera
LPIT0->TMR[3].TCTRL&=~(1<<0);
PTC->PTOR=izquierda;

if (espejo_pin==1)
{
espejo_pin=0;
LPIT0->TMR[3].TVAL=(((100-(unsigned int)duty_cycle)*T)/100);
}
else
{
espejo_pin=1;
LPIT0->TMR[3].TVAL=((((unsigned int)duty_cycle)*T)/100);
}
LPIT0->TMR[3].TCTRL|=(1<<0);       //Enable->Trigger
}


int main(void)
{
	UART_init();
	PORTC_init();
	LPIT0_Ch0_init();
	LPIT0_Ch1_init();
	LPIT0_Ch2_init();
	LPIT0_Ch3_init();

    while(1){

    }
	return 0;
}
