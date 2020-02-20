#include "S32K144.h" /* include peripheral declarations S32K144 */
/*delay*/
void delay (void)
{
unsigned long i=10000000;
do{}while (--i);

}

int main(void)
{
	/*Activate clock PCC register description PCC_PORTD*/ /*Chapter 29 page 633*/
	PCC->PCCn[PCC_PORTD_INDEX]=0X40000000;
	/*Select port (Mux) PORT_PCR*/ /*Chapter 12  PORT_PCRn page 198*/
	PORTD->PCR[0]=0X00000100;
	PORTD->PCR[15]=0X00000100;
	PORTD->PCR[16]=0X00000100;
	/*Port Data direction PDDR Activar todos*/ /*Chapter 13 page 218*/
	PTD->PDDR=0X0003C000;


        for(;;) {
        	/*Port Data direction PDOR*/ /*Chapter 13 page 218*/
        	PTD->PDOR=0X00180000;
        	delay();
        	PTD->PDOR=0X00180001;
        	delay();

        }


	return 0;
}
