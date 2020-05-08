/*
 * main implementation: use this 'C' sample to create your own application
 *
 */
#include "S32K144.h" /* include peripheral declarations S32K144 */
#include "math.h"
#define pi 3.14159
#define R .008  //Radio de llanta en metros
#define L .6486 //Distancia entre las ruedas en metros
#define V 5 //Velocidad deseada en metros/segundos
#define T 100 //periodo
#define left pi/2.5
#define right -pi/2.5
#define k 2260 //constante sensor, distancia en 50cm

unsigned char i;
unsigned char espejo_pin=0;
unsigned char sensor1=0;
unsigned char sensor2=0;
int duty_right;
int duty_left;
int Wm[6]={(0), (left), (right), (right) , (left), (0)};

void LPIT0_Ch0_IRQHandler (void)
{
	unsigned short delay;
	PTC->PDOR=(1<<14)+(1<<7);
	for (i = 0;i<=5;i++)
	{

		LPIT0->MSR=1;					//Borrar bandera
		LPIT0->TMR[0].TCTRL&=~(1<<0);  //Escribimos en TVAL estando detenido el timer
		duty_right=((2*V)+(Wm[i]*L))/(2*R);
		if (espejo_pin==1)
		{
			espejo_pin=0;
			LPIT0->TMR[0].TVAL=(((100-duty_right)*T)/100); //PWM
		}
		else
		{
			espejo_pin=1;
			LPIT0->TMR[0].TVAL=(((duty_right)*T)/100); //PWM
		}
		LPIT0->TMR[0].TCTRL|=(1<<0);		//Enable->Trigger
		for (delay=0; delay<= 5000;delay++);
	}
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
	LPIT0->TMR[0].TCTRL|=1;
	S32_NVIC->ISER[48/32]=(1<<(48%32));
}
void LPIT0_Ch1_IRQHandler (void)
{
	unsigned short delay;
	PTC->PDOR=(1<<14)+(1<<7);
	for (i = 0;i<=5;i++)
	{
		duty_left=((2*V)-(Wm[i]*L))/(2*R);
		LPIT0->MSR=1;					//Borrar bandera
		LPIT0->TMR[0].TCTRL&=~(1<<0);  //Escribimos en TVAL estando detenido el timer
		if (espejo_pin==1)
		{
			espejo_pin=0;
			LPIT0->TMR[0].TVAL=(((100-duty_left)*T)/100);
		}
		else
		{
			espejo_pin=1;
			LPIT0->TMR[0].TVAL=(((duty_left)*T)/100);
		}
		LPIT0->TMR[0].TCTRL|=(1<<0);		//Enable->Trigger
		for (delay=0; delay<= 5000;delay++);
	}
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
	LPIT0->TMR[0].TVAL=T;
	LPIT0->MIER=2;
	LPIT0->TMR[0].TCTRL|=1;
	S32_NVIC->ISER[49/32]=(1<<(49%32));
}
void PORTC_init(void)
{
	PCC->PCCn[PCC_PORTC_INDEX]=(1<<30);
	PORTC->PCR[7]=(1<<8);
	PORTC->PCR[15]=(1<<8);
	PORTC->PCR[17]=(1<<8);
	PORTC->PCR[14]=(1<<8);
	PTC->PDDR=(1<<17)+(1<<15)+(1<<14)+(1<<7);
	PTC->PDOR=0;

}





int main(void)
{
	SCG->SIRCDIV=(1<<8);	//SIRCDIV2=8MHz/1=8MHz
	PORTC_init();
	LPIT0_Ch0_init();
	LPIT0_Ch1_init();

	for(;;) {
	        }

	return 0;
}
