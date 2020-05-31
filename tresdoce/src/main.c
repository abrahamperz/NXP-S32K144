

#include "S32K144.h" /* include peripheral declarations S32K144 */

int mensaje[]={1,2,3};


void SIRC_div(void)
{

    // Slow IRC Control Status Register
    SCG->SIRCCSR &= ~ (1 << 24);
    // [24] LK = 0 Control Status Register can be written.

    SCG->SIRCCSR &= ~ (1 << 0);
    // [0] SIRCEN = 0 Disable Slow IRC

    // Slow IRC Divide Register
    SCG->SIRCDIV |= 0x0101;
    // [10-8] SIRCDIV2 0b001 Divide by 1 (8MHz)
    // [2-0]  SIRCDIV1 0b001 Divide by 1 (8MHz)

    SCG->SIRCCSR |= (1 << 0);
    // [0] SIRCEN = 1 Enable Slow IRC

    while((SCG->SIRCCSR & (1 << 24)) == 0); // wait until clock is valid
    // [24] SIRCVLD = 1 Slow IRC is enabled and output clock is valid
}


void UART_init(void){

	PCC->PCCn[PCC_PORTC_INDEX]=1<<30;
		PORTC->PCR[7]=2<<8;						//LPUART1 TX
		PORTC->PCR[6]=2<<8;						//LPUART1 RX

		SCG->SIRCDIV=1<<8;						//SIRCDIV2: 8 MHz/1

	    PCC->PCCn[PCC_LPUART1_INDEX]=3<<24;		//FIRCDIV2
		PCC->PCCn[PCC_LPUART1_INDEX]|=1<<30;
		LPUART1->BAUD|=52;						//BAUD_SRG=CLK_UART/(16*9600)
		LPUART1->CTRL|=(1<<19)+(1<<18)+(1<<23);			//TE=RE=1

	    S32_NVIC->ISER[33/32]=(1<<(33%32));

}
void LPUART0_RxTx_IRQHandler (void) //CHECHAR QUE BANDERA NOS TRAJO A LA INTERRUPCION! (SOLO SI ESTAS USANDO EL RECEIVER Y EL TRANSMITER)
{
	LPUART1->DATA=mensaje[2];
	    if(mensaje[2]==0){
	        LPUART1->CTRL&=~(1<<23);

	    }
}

int main(void)
{
	SCG->SIRCDIV |= 0x0101;
	//SIRC_div();
	UART_init();

	return 0;
}
