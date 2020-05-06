#include "S32K144.h" /* include peripheral declarations S32K144 */

#define SCL0 PTB->PCOR=(1<<15)
#define SCL1 PTB->PSOR=(1<<15)
#define SDA0 PTB->PCOR=(1<<16)
#define SDA1 PTB->PSOR=(1<<16)

#define SDA_input PTB->PDDR&=~(1<<16);
#define SDA_output PTB->PDDR|=(1<<16);


void delay_us (unsigned char tiempo_us)
{
unsigned char temp;
unsigned char i;
    temp=tiempo_us*10;
	for (i=0;i<=temp;i++);
}

void vIIC_init (void)
{
	//PTB15 (SCL) PTB16 (SDA)

    PCC->PCCn[PCC_PORTB_INDEX]=(1<<30);
	PORTB->PCR[15]= 1<<8;
	PORTB->PCR[16]= 1<<8;
	SCL0;
	SDA0;
	PTB->PDDR=(3<<15);
}

void vIIC_start_bit (void)
{
    SDA1;
    SCL1;
    //esperar t7
    delay_us(5);
    SDA0;
    //esperar t6
    delay_us(5);
    SCL0;
}

void vIIC_stop_bit (void)
{
    SDA0;
    SCL1;
    //esperar t7
    delay_us(5);
    SDA1;
    //esperar t6
    delay_us(5);
    SCL0;
    SDA0;
}

void vIIC_send_byte (unsigned char dato)
{
unsigned char cont=8;  //contador de bits pendientes
do{

    if ((dato&(1<<7))==(1<<7)) SDA1;
    else SDA0;
    delay_us(1);
    SCL1;
    delay_us(5);
    SCL0;
    delay_us(5);
    dato=dato<<1;
}while (--cont!=0);
SDA0;
}

unsigned char vIIC_rec_byte (void)
{
unsigned char cont=8;  //contador de bits pendientes
unsigned char dato;

	SDA_input;
do{
	SCL1;
	dato<<=1;
    if ((PTB->PDIR & (1<<16))==(1<<16)) dato|=1;
    delay_us(5);
    SCL0;
    delay_us(5);
}while (--cont!=0);
    SDA_output;
    return dato;
}

unsigned char vIIC_ack_input (void)
{
unsigned char temp;
    SDA_input;
    SCL1;
    delay_us(1);
    if ((PTB->PDIR & (1<<16))==0) temp=0;
    else temp=1;
    delay_us(5);
    SCL0;
    SDA_output;
    return temp;
}

void vIIC_ack_output (unsigned char dato)
{
     if (dato==0) SDA0;
     else SDA1;
     SCL1;
     delay_us(5);
     SCL0;
     delay_us(5);
     SDA0;
}

void vIIC_byte_write(unsigned short direccion, unsigned char dato)
{
    vIIC_start_bit();
    vIIC_send_byte(0xA0);   //1010 0000. 1010. ID Tipo Mem, 000: Slave, 0 Write
    if (vIIC_ack_input()==0)
        {
        vIIC_send_byte(direccion>>8);
        if (vIIC_ack_input()==0)
        {
            vIIC_send_byte(direccion);
            if (vIIC_ack_input()==0)
                {
                vIIC_send_byte(dato);
                if (vIIC_ack_input()==0)
                     {
                	  vIIC_stop_bit();
                     }
                }
        }
        }
}

unsigned char vIIC_random_read(unsigned short direccion)
{
	unsigned char dato;
    vIIC_start_bit();
    vIIC_send_byte(0xA0);   //1010 0000. 1010. ID Tipo Mem, 000: Slave, 0 Write
    if (vIIC_ack_input()==0)
        {
        vIIC_send_byte(direccion>>8);
        if (vIIC_ack_input()==0)
        {
            vIIC_send_byte(direccion);
            if (vIIC_ack_input()==0)
                {
            	vIIC_start_bit();
            	vIIC_send_byte(0xA1);  //read
            	if (vIIC_ack_input()==0)
            	{
            		dato=vIIC_rec_byte();
            		vIIC_ack_output(1);
            		vIIC_stop_bit();
            	}

                }
        }
        }
    return dato;
}

int main(void)
{
	unsigned char temp;
    vIIC_init();
    vIIC_byte_write(0x100, 0x25);
    temp=vIIC_random_read(0x100);
        for(;;)
        {
        }
  return 0;
}
