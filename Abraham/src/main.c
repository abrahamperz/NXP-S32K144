/*
 * main implementation: use this 'C' sample to create your own application
 *
 */


#include "S32K144.h" /* include peripheral declarations S32K144 */
unsigned char DATA1=0;
unsigned char DATA2=0;
int temp[4]={1,2,3,4};
int flag=0;


void LPUART1_RxTx_IRQHandler (void)
{ //Handler para interrupciones de LPUART1 (Simulink)
	unsigned char i=0;
	if ((LPUART1->STAT & (1<<23))==(1<<23)) //tx
	{ //Interrupción para transmitir a simulink

		LPUART1->DATA=flag;

	}

	if ((LPUART1->STAT & (1<<21))==(1<<21)) //rx
	{   //Interrupción para recibir de simulink         //LISTO
		flag=LPUART1->DATA;


	}
}

void LPUART2_RxTx_IRQHandler (void){ //Handler para interrupciones de LPUART2 (Bluetooth)
	unsigned char i=0;
	if ((LPUART2->STAT & (1<<23))==(1<<23)) //tx
	{ //Interrupción para transmitir al bluetooth          //LISTO
		LPUART2->DATA=flag;

	    }
	if ((LPUART2->STAT & (1<<21))==(1<<21)) //rx  //Interrupción para recibir del bluetooth         //LISTO
	 {
		flag=LPUART2->DATA;
	 }
    }
int main(void)
{

	PCC->PCCn[PCC_PORTA_INDEX]=1<<30;    //Habilitar clock del periférico en Puerto A
	PCC->PCCn[PCC_PORTC_INDEX]=1<<30;    //Habilitar clock del periférico en Puerto C
    PCC->PCCn[PCC_PORTD_INDEX]=1<<30;    //Habilitar clock del periférico en Puerto D

    PORTD->PCR[0]=(1<<8);                //LED Azul GPIO
    PORTD->PCR[15]=(1<<8);
    PTD->PDDR=(1<<15)+1;
    PTD->PDOR=(1<<15)+1;

	SCG->SIRCDIV=1<<8;				     //SIRCDIV2: 8 MHz/1
	/*  LPTMR0 Config   */
	PCC->PCCn[PCC_LPTMR0_INDEX]=(1<<30);   //Peripheral Clock al módulo LPTMR0
	LPTMR0->CMR=10-1;
	LPTMR0->PSR=5;                         //Bypass prescaler, escogemos reloj fijo de 1KHz -> 1ms por cuenta
	LPTMR0->CSR=(1<<6)+1;                  //Enable y habilitamos interrupción
	S32_NVIC->ISER[58/32]=(1<<(58%32));    //Interrupción por ISER

	/*  LPUART1 Config  */
	PORTC->PCR[7]=2<<8;				    //LPUART1 TX   PIN 7 - PUERTO C (Conexión desde Micro USB)
	PORTC->PCR[6]=2<<8;				    //LPUART1 RX   PIN 6 - PUERTO C (Conexión desde Micro USB)
    PCC->PCCn[PCC_LPUART1_INDEX]=2<<24;		//SIRCDIV2
	PCC->PCCn[PCC_LPUART1_INDEX]|=1<<30;
	LPUART1->BAUD|=52;						//BAUD_SRG=CLK_UART/(16*9600)
	LPUART1->CTRL|=(1<<23)+(1<<21)+(1<<19)+(1<<18);	        //TE=RE=1 Habilitamos interrupciones tanto de Tx como de Rx, y habilitamos transmisión y recepción
    S32_NVIC->ISER[33/32]=(1<<(33%32));     //Habilitar interrupcion de LPUART1

    /*  LPUART2 Config  */
    PORTA->PCR[9]=2<<8;				    //LPUART2 TX   PIN 9 - PUERTO A
    PORTA->PCR[8]=2<<8;				    //LPUART2 RX   PIN 8 - PUERTO A
    PCC->PCCn[PCC_LPUART2_INDEX]=2<<24;		//SIRCDIV2
   	PCC->PCCn[PCC_LPUART2_INDEX]|=1<<30;
   	LPUART2->BAUD|=52;						//BAUD_SRG=CLK_UART/(16*9600)
   	LPUART2->CTRL|=(1<<23)+(1<<21)+(1<<19)+(1<<18);	        //TE=RE=1 Habilitamos interrupciones tanto de Tx como de Rx, y habilitamos transmisión y recepción
    S32_NVIC->ISER[35/32]=(1<<(35%32));     //Habilitar interrupciones de LPUART2
}
