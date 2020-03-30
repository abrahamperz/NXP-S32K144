#include "S32K144.h" /* include peripheral declarations S32K144 */


void LPTMR0_IRQHandler (void)
{
	LPTMR0->CSR|=(1<<7); //vuelve a empezar cuando CMR y CNR son iguaes
	PTD->PTOR=0x00000001;		//led azul toggle
}

int main(void)
{
	PCC->PCCn[PCC_PORTD_INDEX]=0x40000000; //1<<30
	PORTD->PCR[0]=0x00000100;
	//PORTD->PCR[15]=0x00000100;  //1<<8
	//PORTD->PCR[16]=0x00000100;
	PTD->PDDR=0x00018001;

	PCC->PCCn[PCC_LPTMR0_INDEX]=0x40000000;
	LPTMR0->CMR=500-1;			// 500 cuentas de 1 ms
	LPTMR0->PSR=5;				//Bypass preescaler, clock source=1 KHz (pag 1406)
	LPTMR0->CSR=(1<<6)+1;		//Hab Intr LPTRM0, Enable
	S32_NVIC->ISER[58/32]=(1<<(58%32));

        for(;;) {
        }

	return 0;
}
