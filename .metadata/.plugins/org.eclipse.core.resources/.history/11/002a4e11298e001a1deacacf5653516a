#include "S32K144.h" /* include peripheral declarations S32K144 */

int mensaje[5]={5,10,15,20,0};  // 'H'<> "H"

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

	for (i=0;i<=3;i++)    //for (i=65; i<=91; i++)
	{
	do{}while ((LPUART1->STAT & (1<<23))==0);
	LPUART1->DATA=mensaje[i];
	}

/*
	i=0;
	do
	{
	do{}while ((LPUART1->STAT & (1<<23))==0);
	LPUART1->DATA=mensaje1[i++];
	}while (mensaje1[i]!=0);
*/
	while (1);
	return 0;
}
