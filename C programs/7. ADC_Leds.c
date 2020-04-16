
#include "S32K144.h" /* include peripheral declarations S32K144 */

void ADC0_IRQHandler (void)
{
	ADC0->SC1[0]=(1<<6)+12;
	if (ADC0->R[0]<0x40) PTD->PDOR=(1<<16)+1;  //prender el rojo
	if ((ADC0->R[0]>=0x40) && (ADC0->R[0]<0x80)) PTD->PDOR=(1<<16)+(1<<15);  //prender led azul
    if (ADC0->R[0]>=0x80)PTD->PDOR=(1<<15)+1; //led verde
}

int main(void)
{
		PCC->PCCn[PCC_PORTD_INDEX]=(1<<30);
		PORTD->PCR[16]=(1<<8);    //GPIO
		PORTD->PCR[15]=(1<<8);    //GPIO
		PORTD->PCR[0]=(1<<8);    //GPIO

		PTD->PDDR=(1<<16)+(1<<15)+1;   //outputs
		PTD->PDOR=(1<<16)+(1<<15)+1;   //estado inicial off

		PCC->PCCn[PCC_ADC0_INDEX]=(2<<24);
		PCC->PCCn[PCC_ADC0_INDEX]|=(1<<30);

		SCG->SIRCDIV=(1<<8);	//SIRCDIV2=8MHz/1=8MHz

		ADC0->CFG1=(2<<5);      //DIVCLK=4, Mode: 8 bits, ALTCLK1 proviene PCC
		ADC0->SC1[0]=(1<<6)+12;	//hab Intr ADC, seleccionar canal 12 (pot)

		S32_NVIC->ISER[39/32]=(1<<(39%32));

		while (1);

	return 0;

}
