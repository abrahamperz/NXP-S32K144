#include "S32K144.h" /* include peripheral declarations S32K144 */



int x = 5;
int y = 10;
int z = 15;
int vel = 20;

int* array[]={&x,&y,&z};
unsigned char dato=0;

//int array2[]={x,y,z};

int mensaje[]={4,8,16,20,0};
//int mensaje=0b10100;
//unsigned char mensaje[]={"Hola mundo"};



unsigned char i =0;


void LPUART2_RxTx_IRQHandler(void){

	if((LPUART2->STAT & (1<<23)) == (1<<23)){ //tx



		LPUART2->DATA=mensaje[2];
		//if (dato!=2){LPUART2->CTRL&=~(1<<23);}
		    	//if(mensaje[6]) LPUART1->CTRL|=~(1<<23);


			//  if(mensaje[i]==0){
			  //     LPUART1->CTRL&=~(1<<23);
			    //   i=0; }
	}

	if((LPUART2->STAT & (1<<21)) == (1<<21)){
		dato = LPUART2->DATA; //El dato temporal pasa a valer el dato recibido por la UART0



		}

}


int main(void)
{

	PCC->PCCn[PCC_PORTA_INDEX]=1<<30;
	PORTA->PCR[9]=2<<8;						//LPUART1 TX
	PORTA->PCR[8]=2<<8;						//LPUART1 RX

	PCC->PCCn[PCC_PORTD_INDEX]=0x40000000; //1<<30
	PORTD->PCR[0]=0x00000100;
	PORTD->PCR[15]=0x00000100;  //1<<8
	PORTD->PCR[16]=0x00000100;
	PTD->PDDR=(1<<16)+(1<<15)+1;
	PTD->PDOR=(1<<16)+(1<<15)+1; //leds off
	//mensaje2[]={x,y,z,vel,0};


	SCG->SIRCDIV=1<<8;						//SIRCDIV2: 8 MHz/1

    PCC->PCCn[PCC_LPUART2_INDEX]=2<<24;		//SIRCDIV2
	PCC->PCCn[PCC_LPUART2_INDEX]|=1<<30;
	LPUART2->BAUD|=52;						//BAUD_SRG=CLK_UART/(16*9600)
	LPUART2->CTRL|=(1<<23)+(1<<21)+(1<<19)+(1<<18);			//TE=RE=1

    S32_NVIC->ISER[35/32]=(1<<(35%32));

	while (1);
	if (dato=='A') {
		PTD->PDOR=(1<<16)+1; //rojo
	}

	return 0;
}
