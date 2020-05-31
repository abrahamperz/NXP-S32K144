#include "S32K144.h" /* include peripheral declarations S32K144 */

unsigned char mensaje1[]={"A####\r\n"};  // 'H'<> "H"
unsigned char dato[]={"0057"};

#define pi 3.14159
#define R .008  //Radio de llanta en metros
#define L .6486 //Distancia entre las ruedas en metros
#define V 5 //Velocidad deseada en metros/segundos
#define T 100 //periodo
#define left pi/2.5
#define right -pi/2.5

//variables motores
unsigned char i;
unsigned char espejo_pin=0;
unsigned char sensor1=0;
unsigned char sensor2=0;
int duty_right;
int duty_left;

int Wm[6]={(0), (left), (right), (right) , (left), (0)};

//variables sensores
unsigned short frec;
unsigned short t_viejo=0;
unsigned char n_muestra=1;
unsigned short tiempo1;
unsigned short delta_US;
unsigned char distancia_cm;


//funciones motores
void LPIT0_Ch0_IRQHandler (void)
{
	unsigned short delay;
	PTC->PDOR=(1<<14);
	for (i = 0;i<=5;i++)
	{

		LPIT0->MSR=1;					//Borrar bandera
		LPIT0->TMR[0].TCTRL&=~(1<<0);  //Escribimos en TVAL estando detenido el timer
		//ecuación cinematica del robot
		duty_right=((2*V)+(Wm[i]*L))/(2*R);
		//int Wm[6]={(0), (left), (right), (right) , (left), (0)};
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

	LPIT0->MCR=1;		//M_CEN=1;
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	LPIT0->TMR[0].TCTRL=(0<<24)+(1<<23)+(1<<17)+(1<<18);
	LPIT0->TMR[0].TVAL=T;
	LPIT0->MIER=1;
	LPIT0->TMR[0].TCTRL|=1; //flag
	S32_NVIC->ISER[48/32]=(1<<(48%32));
}

void LPIT0_Ch1_IRQHandler (void)
{
	unsigned short delay;
	PTC->PDOR=(1<<7);
	for (i = 0;i<=5;i++)
	{

		LPIT0->MSR=1;					//Borrar bandera
		LPIT0->TMR[1].TCTRL&=~(1<<0);  //Escribimos en TVAL estando detenido el timer
		//ecuación cinematica del robot
		duty_left=((2*V)-(Wm[i]*L))/(2*R);
		//int Wm[6]={(0), (left), (right), (right) , (left), (0)};
		if (espejo_pin==1)
		{
			espejo_pin=0;
			LPIT0->TMR[1].TVAL=(((100-duty_left)*T)/100); //PWM
		}
		else
		{
			espejo_pin=1;
			LPIT0->TMR[1].TVAL=(((duty_left)*T)/100); //PWM
		}
		LPIT0->TMR[1].TCTRL|=(1<<0);		//Enable->Trigger
		for (delay=0; delay<= 5000;delay++);
	}
}
void LPIT0_Ch1_init(void)
{
	PCC->PCCn[PCC_LPIT_INDEX]=(2<<24);
    PCC->PCCn[PCC_LPIT_INDEX]|=(1<<30);

	LPIT0->MCR=1;		//M_CEN=1;
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	LPIT0->TMR[1].TCTRL=(0<<24)+(1<<23)+(1<<17)+(1<<18);
	LPIT0->TMR[1].TVAL=T;
	LPIT0->MIER=1;
	LPIT0->TMR[1].TCTRL|=1; //flag
	S32_NVIC->ISER[49/32]=(1<<(49%32));
}


void PORTC_init(void)
{
	PCC->PCCn[PCC_PORTC_INDEX]=(1<<30);
	PORTC->PCR[7]=(9<<16)+(1<<8);
	PORTC->PCR[15]=(9<<16)+(1<<8);
	PORTC->PCR[17]=(9<<16)+(1<<8);
	PORTC->PCR[14]=(9<<16)+(1<<8);
	PTC->PDDR=(1<<17)+(1<<15)+(1<<14)+(1<<7);
	PTC->PDOR=0;

}


//funciones ultrasonico
void FTM2_Ch0_Ch1_IRQHandler (void)
{

	if ((FTM2->CONTROLS[0].CnSC & (1<<7))==(1<<7))
	{
		(void) FTM2->CONTROLS[0].CnSC;			//1er paso para borrar bandera
		FTM2->CONTROLS[0].CnSC&=~(1<<7);		//2o. paso para borrar bandera
		if (n_muestra==1)
		{
			tiempo1=FTM2->CONTROLS[0].CnV;
			n_muestra=2;
		}
		else
		{
			delta_US=FTM2->CONTROLS[0].CnV-tiempo1;
			distancia_cm=delta_US/59;
			n_muestra=1;
			FTM2->CONTROLS[0].CnSC&=~(1<<6);
			if (distancia_cm<=55)
			{
				//flag enable
				LPIT0->TMR[0].TCTRL|=1;
				LPIT0->TMR[1].TCTRL|=1;


			}
		}
		}

}

void ultrasonico_init (void)
{
unsigned short delay;
	PTA->PSOR=(1<<1);						//PTA1=1  Trigger
	//10 us al menos
	for (delay=0; delay<= 5000;delay++);	//mantener en PTA1=1 al menos 10 us
	PTA->PCOR=(1<<1);						//PTA1=0  Trigger
	for (delay=0; delay<= 2000;delay++);
	FTM2->CONTROLS[0].CnSC=(1<<6)+(3<<2);		//0011 MSB:MSA, ELSB:ELSA. hab Intr
												//ambos flancos
}

void FTM2_Ch0_Ch1_init(void)
{
	PCC->PCCn[PCC_PORTA_INDEX]=1<<30;
		PORTA->PCR[0]=2<<8;						//FTM2_CH1
		PORTA->PCR[1]=1<<8;						//GPIO para trigger ultrasonico
		PTA->PCOR=1<<1;							//PTA1=0
		PTA->PDDR=1<<1;							//Output

		PCC->PCCn[PCC_PORTC_INDEX]=1<<30;
		PORTC->PCR[5]=2<<8;						//FTM2_CH0

		//SCG->SIRCDIV=1<<0;						//SIRCDIV1: 8 MHz/1 (125 ns)

	    PCC->PCCn[PCC_FTM2_INDEX]=2<<24;		//SIRCDIV1
		PCC->PCCn[PCC_FTM2_INDEX]|=1<<30;

		FTM2->SC=(3<<3)+3;						//3<<3 (Reloj externo SIRCDIV1)
												// 3: preescaler 8 (1MHz, 1 us)
		FTM2->CONTROLS[0].CnSC=(1<<6)+(1<<2);		//0001 MSB:MSA, ELSB:ELSA. hab Intr

		S32_NVIC->ISER[111/32]=1<<(111%32);		//Hab FTM2 Ch0 y CH1

}



void delay (void)
{
unsigned long i=1000000;

do{}while (--i);
}

int main(void)
{
	unsigned char i;

	PCC->PCCn[PCC_PORTC_INDEX]=1<<30;
	PORTC->PCR[7]=2<<8;						//LPUART1 TX
	PORTC->PCR[6]=2<<8;						//LPUART1 RX

	SCG->SIRCDIV=1<<8;						//SIRCDIV2: 8 MHz/1

    PCC->PCCn[PCC_LPUART1_INDEX]=2<<24;		//SIRCDIV2
	PCC->PCCn[PCC_LPUART1_INDEX]|=1<<30;
	LPUART1->BAUD|=52;						//BAUD_SRG=CLK_UART/(16*9600)
	LPUART1->CTRL|=(1<<19)+(1<<18);			//TE=RE=1


	mensaje1[1]=dato[0];
	mensaje1[2]=dato[1];
	mensaje1[3]=dato[2];
	mensaje1[4]=dato[3];


			do{
				i=0;
				do{

					do{}while ((LPUART1->STAT & (1<<23))==0);
					LPUART1->DATA=mensaje1[i++];

				}while (mensaje1[i]!=0);

			} while(1);



	return 0;
}
