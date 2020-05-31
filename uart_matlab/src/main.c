#include "S32K144.h" /* include peripheral declarations S32K144 */

unsigned char mensaje1[]={"A####\r\n"};  // 'H'<> "H"
unsigned char dato[]={"1"};



int main(void)
{
	unsigned char i;
	unsigned char j;

	PCC->PCCn[PCC_PORTC_INDEX]=1<<30;
	PORTC->PCR[7]=2<<8;						//LPUART1 TX
	PORTC->PCR[6]=2<<8;						//LPUART1 RX

	SCG->SIRCDIV=1<<8;						//SIRCDIV2: 8 MHz/1

    PCC->PCCn[PCC_LPUART1_INDEX]=2<<24;		//SIRCDIV2
	PCC->PCCn[PCC_LPUART1_INDEX]|=1<<30;
	LPUART1->BAUD|=52;						//BAUD_SRG=CLK_UART/(16*9600)
	LPUART1->CTRL|=(1<<19)+(1<<18);			//TE=RE=1
	// funciona
	do{
		j=0;
		do{
			i=0;
			do{
				mensaje1[1]=dato[j];
				do{}while ((LPUART1->STAT & (1<<23))==0);
				LPUART1->DATA=mensaje1[i++];

			}while (mensaje1[i]!=0);
			j++;
		} while(dato[j]!=0);
	} while(1);

	return 0;
}
