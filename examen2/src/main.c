/*
 *
 *
 *

 *

 */

 /*****************
  * Problema 1 activamos pulsos puente h
  *
 *4 adelante
 *5 atras
 *
 *utiizamos un adc para saber cuando esta en el limite, los limites seran 0 y 255
 * y se apagan en la interrupcion del adc
 *
 * Los datos del giroscopio los leemos de la funcion LPI2CHandler de la variable yaw_z
 * si esta posicion es mayor a 30, suponiendo que la pendiente de 5 grados se identifica a partir del valor 30
 * cambiamos el duty cycle en el LPIT0 channel 0
 * PTC->PTOR=dirrecion;
 *duty_cycle = duty_cicle + aumentar_corriente, aumentar corriente es = 0, y solo cambia de valor
 *so yaw_z es > 30, as� aumentamos el duty_cicle, para que pueda subir la corriente
 *
 ********************************************++
 *Problema 2 dos motores
* Activamos con pulsos el puente h
 * Motor derecho PORTC
 * 7 arriba
 * 14 atras
 *
 * Motor izquierdo PORTC
 * 15 izquierda
 * 17 derecha
  *
  *dichos movimientos los guardamos en la variable movimiento, y de acuerdo a la situacion
  *que active la interrupcion del puerto c de los botones, es el valor que se le asignara
  *y se hara en LPI0T_Channel 0
  *la posicion en x se guarda en ventana x y la posicion en y en ventanay para hacer la retroalimentacion
  *para esto se utiliza un adc
  *y
  *y
  *y
  * ****************************************
  * Problema3
  *
  * Suponiendo que se tiene 4 ventanas y el retrovisor del copiloto
  * 8 motores ser�n ser�n para subir y bajar ventanas utilizados con flextimer
  * 4 motores para el retrovisor de copiloto tambien por flextimer
  * En cada FTM tenemos 8 channels por lo tanto estar� bien
  * y segun lo que recibamos en rx lo mandaremos en dato en tx
  * Con A,B,C,D Activamos el channel 1
  * Con E,F,G,H Activamos el channel 2
  * Con I,J,K,L Activamos el channel 3
  * Estas indicaciones las hara el micro del lado del conductor por eso no inicializamos
  * ningun FTM aqui
*
		   *
		   */


#include "S32K144.h" /* include peripheral declarations S32K144 */

#define arriba (1<<7)   //
#define atras (1<<14)     //L
#define derecha (1<<15)    //L
#define izquierda (1<<17) //



//un arreglo para la direcci�n y otro arreglo para la duracion en milisegundos
int8_t var_read[8];

int16_t datos_recibidosI2C[];
int16_t datos_tx[];


unsigned char duty_cycle=50;
unsigned char espejo_pin;
unsigned char i=0;
unsigned char T=100;
unsigned char data=0;
unsigned char dato=0;
unsigned char error = 0;
unsigned char direccion = (1<<4);
unsigned char aumentar_corriente=0;
unsigned char movimiento;
unsigned ventanax;
unsigned ventanay;


//variables I2C



unsigned int dato1=2;
unsigned int dato2=2;
unsigned int dato3=2;
extern double pos_y;
extern double pos_x;
double pos_z;
unsigned int vel=3.1416;





unsigned int arreglo[];




unsigned char tx = 0;

int8_t aceleracionx[2];
int16_t aceleracion_x;

int8_t aceleraciony[2];
int16_t aceleracion_y;

int8_t aceleracionz[2];
int16_t aceleracion_z;

int8_t gyrox[2];
int16_t gyro_x;

int8_t gyroy[2];
int16_t gyro_y;

int8_t gyroz[2];
int16_t gyro_z;

double pitch_y_acc;
double roll_x_acc;
double yaw_z;

double* datos[4]={&roll_x_acc,&pitch_y_acc,&yaw_z,&dato};



unsigned long bandera=0;
unsigned long tiempo_prev;
float dt;
double pitch_y;
double roll_x;


void adc_init(void){
	PCC->PCCn[PCC_ADC0_INDEX]=(2<<24);
			PCC->PCCn[PCC_ADC0_INDEX]|=(1<<30);

			SCG->SIRCDIV=(1<<8);	//SIRCDIV2=8MHz/1=8MHz

			ADC0->CFG1=(2<<5);      //DIVCLK=4, Mode: 8 bits, ALTCLK1 proviene PCC
			ADC0->SC1[0]=(1<<6)+12;	//hab Intr ADC, seleccionar canal 12 (pot)

			S32_NVIC->ISER[39/32]=(1<<(39%32));
}

void ADC0_IRQHandler (void)
{
	ADC0->SC1[0]=(1<<6)+12;
	if (ADC0->R[0]==0){

		//si llego al limite se apagan
		direccion = (0<<4);
		direccion = (0<<5);

	}
	else if ((ADC0->R[0]>=255)){
		//si llego al limite se apagan

		direccion = (0<<4);
		direccion = (0<<5);
	}

}
void ADC1_IRQHandler (void)
{
	ADC1->SC1[0]=(1<<6)+12;
	if (ADC0->R[0]==0){

		//si llego al limite se apagan
		direccion = (0<<4);
		direccion = (0<<5);

	}
	else if ((ADC1->R[0]>=255)){
		//si llego al limite se apagan

		direccion = (0<<4);
		direccion = (0<<5);
	}

}





void delay (unsigned char tiempoms){
	i=500*tiempoms;
	do{

	}while (--i);
}


void ADC1_init(void)

{
	PCC->PCCn[PCC_ADC1_INDEX]=(2<<24);
	PCC->PCCn[PCC_ADC1_INDEX]|=(1<<30);


	ADC1->CFG1=(2<<5);      //DIVCLK=4, Mode: 8 bits, ALTCLK1 proviene PCC
	//ADC0->SC1[0]=(1<<6)+12;	//hab Intr ADC, seleccionar canal 12 (pot)

	S32_NVIC->ISER[40/32]=(1<<(40%32));
}

void LPUART1_RxTx_IRQHandler (void)
{

	if ((LPUART1->STAT & (1<<21))==(1<<21))   //Rx
	{
		 /*
		   * Suponiendo que se tiene 4 ventanas y el retrovisor del copiloto
		   * 8 motores ser�n ser�n para subir y bajar ventanas utilizados con flextimer
		   * 4 motores para el retrovisor de copiloto tambien por flextimer
		   * En cada FTM tenemos 8 channels por lo tanto estar� bien
		   * y segun lo que recibamos en rx lo mandaremos en dato en tx
		   *
		   * Con A,B,C,D Activamos el channel 1
		   * Con E,F,G,H Activamos el channel 2
		   * Con I,J,K,L Activamos el channel 3
		   * Estas indicaciones las hara el micro del lado del conductor por eso no inicializamos
		   * ningun FTM aqui
		   *
		   *
		   */

		LPUART2->DATA=dato;


	}

	if ((LPUART1->STAT & (1<<23))==(1<<23)) //TX
	{
		unsigned char i=0;
		if (data == 'A') {

						LPIT0->TMR[0].TCTRL|=1;
					}
		if (data == 'B') {

								LPIT0->TMR[0].TCTRL|=1;
							}
		if (data == 'C') {

								LPIT0->TMR[0].TCTRL|=1;
							}
		if (data == 'D') {

								LPIT0->TMR[0].TCTRL|=1;
							}
		if (data == 'E') {

								LPIT0->TMR[1].TCTRL|=1;
							}
		if (data == 'F') {

								LPIT0->TMR[1].TCTRL|=1;
							}
		if (data == 'G') {

								LPIT0->TMR[1].TCTRL|=1;
							}
		if (data == 'H') {

								LPIT0->TMR[1].TCTRL|=1;
							}
		if (data == 'I') {

								LPIT0->TMR[2].TCTRL|=1;
							}
		if (data == 'J') {

								LPIT0->TMR[2].TCTRL|=1;
							}
		if (data == 'K') {

								LPIT0->TMR[2].TCTRL|=1;
							}
		if (data == 'L') {

								LPIT0->TMR[2].TCTRL|=1;
							}





		LPUART2->DATA=dato;

     }
}

void UART_init(void)
{

	PCC->PCCn[PCC_PORTC_INDEX]=1<<30;
	PORTC->PCR[7]=2<<8;						//LPUART1 TX
	PORTC->PCR[6]=2<<8;						//LPUART1 RX

	SCG->SIRCDIV=1<<8;						//SIRCDIV2: 8 MHz/1

	PCC->PCCn[PCC_LPUART1_INDEX]=2<<24;		//SIRCDIV2
    PCC->PCCn[PCC_LPUART1_INDEX]|=1<<30;
	LPUART1->BAUD|=52;						//BAUD_SRG=CLK_UART/(16*9600)
	LPUART1->CTRL|=(1<<21)+(1<<19)+(1<<18);	//TE=RE=1
	S32_NVIC->ISER[33/32]=(1<<(33%32));

}
void PORTC_IRQHandler (void)
{
	PORTC->PCR[4]|=1<<24; //apagar bandera isf (w1c)
	PORTC->PCR[5]|=1<<24; //apagar bandera isf (w1c)

	PORTC->PCR[7]|=1<<24; //apagar bandera isf (w1c)//adelante
	PORTC->PCR[14]|=1<<24; //apagar bandera isf (w1c)//atras
	PORTC->PCR[15]|=1<<24; //apagar bandera isf (w1c)//derecha
	PORTC->PCR[17]|=1<<24; //apagar bandera isf (w1c)//izquierda

	//cambiamos la dirrecion del motor cada vez que se presiona
	if ((direccion) == (1<<4)){
		direccion == (1<<5);
	}
	else  direccion == (1<<5);

	if (PORTC->PCR[7]=0<<24){
		PORTC->PCR[7]|=1<<24;
		movimiento=arriba;
		}
	if (PORTC->PCR[14]=0<<24){
			PORTC->PCR[14]|=1<<24;
			movimiento=atras;
			}
	if (PORTC->PCR[15]=0<<24){
			PORTC->PCR[15]|=1<<24;
			movimiento=derecha;
			}
	if (PORTC->PCR[17]=0<<24){
			PORTC->PCR[17]|=1<<24;
			movimiento=izquierda;
			}
	}



void PORTC_init(void)
{
	//programa 1
	PCC->PCCn[PCC_PORTC_INDEX]=(1<<30);
	PORTC->PCR[4]=(9<<16)+(1<<8);
	PORTC->PCR[5]=(9<<16)+(1<<8);


	//pr2

	PORTC->PCR[7]=(9<<16)+(1<<8);
	PORTC->PCR[15]=(9<<16)+(1<<8);
	PORTC->PCR[17]=(9<<16)+(1<<8);
	PORTC->PCR[14]=(9<<16)+(1<<8);
	PTC->PDDR=(1<<7)+(1<<15)+(1<<17)+(1<<14);
	PTC->PDOR=0;
	S32_NVIC->ISER[61/32]=1<<(61%32); //PortC interrupt
}
void LPIT0_Ch0_init(void)
{

	PCC->PCCn[PCC_LPIT_INDEX]=(2<<24);
    PCC->PCCn[PCC_LPIT_INDEX]|=(1<<30);
	SCG->SIRCDIV=(1<<8);  //8MHz/1

	LPIT0->MCR=1;		//M_CEN=1;
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	LPIT0->TMR[0].TCTRL=(0<<24)+(1<<23)+(1<<17)+(1<<18);
	LPIT0->TMR[0].TVAL=T;
	LPIT0->MIER=1;
	//LPIT0->TMR[0].TCTRL|=1;
	S32_NVIC->ISER[48/32]=(1<<(48%32));

}
void LPIT0_Ch0_IRQHandler (void)
{

LPIT0->MSR|=1;
//Borrar bandera
LPIT0->TMR[0].TCTRL&=~(1<<0);
//pones la direccion previamente asignada
PTC->PTOR=direccion;
duty_cycle = duty_cycle + aumentar_corriente;

if (espejo_pin==1)
{
espejo_pin=0;
LPIT0->TMR[0].TVAL=(((100-(unsigned int)duty_cycle)*T)/100);
}
else
{
espejo_pin=1;
LPIT0->TMR[0].TVAL=((((unsigned int)duty_cycle)*T)/100);
}
LPIT0->TMR[0].TCTRL|=(1<<0);       //Enable->Trigger
}


void LPIT0_Ch3_init(void)
{


	PCC->PCCn[PCC_LPIT_INDEX]=(2<<24);
    PCC->PCCn[PCC_LPIT_INDEX]|=(1<<30);
	SCG->SIRCDIV=(1<<8);  //8MHz/1

	LPIT0->MCR=1;		//M_CEN=1;
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	LPIT0->TMR[3].TCTRL=(0<<24)+(1<<23)+(1<<17)+(1<<18);
	LPIT0->TMR[3].TVAL=T;
	LPIT0->MIER|=(1<<4);
	//LPIT0->TMR[0].TCTRL|=1;
	S32_NVIC->ISER[51/32]=(1<<(51%32));

}
void LPIT0_Ch3_IRQHandler (void)
{

LPIT0->MSR|=1;
//Borrar bandera
LPIT0->TMR[3].TCTRL&=~(1<<0);
PTC->PTOR=movimiento;

if (espejo_pin==1)
{
espejo_pin=0;
LPIT0->TMR[3].TVAL=(((100-(unsigned int)duty_cycle)*T)/100);
}
else
{
espejo_pin=1;
LPIT0->TMR[3].TVAL=((((unsigned int)duty_cycle)*T)/100);
}
LPIT0->TMR[3].TCTRL|=(1<<0);       //Enable->Trigger
}


//********** I2C

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


/*******************************************************************************
Function Name : LPI2C0_init
Notes         : BAUD RATE: 400 kbps
                I2C module frequency 8Mhz (SIRCDIV2_CLK)
                PRESCALER:0x00; FILTSCL/SDA:0x0/0x0; SETHOLD:0x4; CLKLO:0x0B; CLKHI:0x05; DATAVD:0x02
                See Table 50-10 Example timing configuration in S32K1xx Reference manual rev.9
 *******************************************************************************/
void LPI2C0_init(void)
{
    // Peripheral Clock Controller
    PCC-> PCCn[PCC_PORTA_INDEX] = 1<<30;
    // PTA2 ALT3 LPI2C0_SDA
    PORTA->PCR[2] |= 3<<8;
    // PTA3 ALT3 LPI2C0_SCL
    PORTA->PCR[3] |= 3<<8;

    PCC->PCCn[PCC_LPI2C0_INDEX] |= 2<<24;   // SIRCDIV2_CLK (8 MHz)
    PCC->PCCn[PCC_LPI2C0_INDEX] |= 1<<30;

    LPI2C0->MCCR0 = 0x0204050B;
    // [24] DATAVD  0x02
    // [16] SETHOLD 0x04
    // [8]  CLKHI   0x05
    // [0]  CLKLO   0x0B

    // Master Interrupt Enable Register (MIER)
    LPI2C0->MIER = (1<<10);
    // [10] NDIE = 1  (NACK detect interrupt enabled)

    // Master Control Register
    LPI2C0->MCR = 0x301;
    // [9] RRF = 1   (Receive FIFO is reset)
    // [8] RTF = 1   (Transmit FIFO is reset)
    // [0] MEN = 1   (Master logic is enabled)

    // LPI2C_Master_IRQHandler
    S32_NVIC->ICPR[0] = (1 << (24 % 32));
    S32_NVIC->ISER[0] = (1 << (24 % 32));
    S32_NVIC->IP[24]  = 0x00;                 // Priority level 0
}

/*******************************************************************************
Function Name : I2C_bus_busy
Returns       : uint8_t
Notes         : I2C Bus is idle/busy
 *******************************************************************************/
static unsigned char bus_busy(void)
{
     error = 0;                 // CLEAR ALL ERRORS

     unsigned int timeout_b = 0;
     while ((LPI2C0->MSR & (1 << 25)) && (timeout_b < 1000))  ++timeout_b;

     if(timeout_b >= 1000) return (error |= (1 << 1));

     /*
      * For debugging purposes
      */ //PTD-> PCOR |= (1 << 0); // BLUE LED ON

     return 0;
}

/*******************************************************************************
Function Name : I2C_start_ACK
Parameters    : uint8_t address
Notes         : Generate (repeated) START and transmit address in DATA[7:0]
 *******************************************************************************/
void generate_start_ACK(uint8_t address)
{
    uint32_t command    = (address << 0);
    command             |= (0b100 << 8);
    LPI2C0->MTDR = command;
}

/*******************************************************************************
Function Name : I2C_write_byte
Parameters    : uint8_t data
Notes         : Transmit DATA[7:0]
 *******************************************************************************/
static void transmit_data(uint8_t data)
{
    LPI2C0->MTDR = data;
}


/*******************************************************************************
Function Name : I2C_stop
Returns       : uint8_t
Notes         : Generate STOP condition
 *******************************************************************************/
static uint8_t generate_stop(void)
{
    uint32_t timeout_s      = 0;
    uint8_t stop_sent_err   = 0;

    LPI2C0->MTDR = 0x0200; //command

    while((!(LPI2C0->MSR & (1 << 9))) && (!stop_sent_err))
    {
        if(timeout_s > 3000)
        {
            error |= (1 << 3);
            stop_sent_err = 1;
        }
        timeout_s++;
    }

    if(LPI2C0->MSR & (1 << 9))
    {
        LPI2C0->MSR |= (1 << 9); // Clear Stop flag
    }

     if(error) return error;
     else return 0;
}

/*******************************************************************************
Function Name : I2C_read
Parameters    : uint8_t *p_buffer, uint8_t n_bytes
Modifies      : uint8_t *p_buffer
Returns       : uint8_t
Notes         : Receive (DATA[7:0] + 1) bytes
 *******************************************************************************/
static void receive_data(uint8_t *p_buffer, uint8_t n_bytes)
{
    uint8_t  n;
    uint16_t time;
    uint16_t timeout_r = (2000 * n_bytes);
    uint16_t command;

    command =    0x0100;
    command |=  (n_bytes - 1);
    LPI2C0->MTDR = command;

    while (((LPI2C0->MFSR) >> 16 != n_bytes) && (time < timeout_r)) ++time;

    if(time >= timeout_r)
    {
        LPI2C0->MCR |= (1 << 9);     // reset receive FIFO
        error |= (1 << 2);
    }
    else{
        for(n = 0; n < n_bytes; ++n)
        {
            p_buffer[n] = (uint8_t)(LPI2C0->MRDR & 0x000000FF);
        }
    }
}

/*******************************************************************************
Function Name : I2C_write_reg
Parameters    : uint8_t s_w_address, uint8_t s_reg_address, uint8_t byte
Returns       : uint8_t
Notes         : Write a single byte to a slave's register
 *******************************************************************************/
uint8_t LPI2C0_write(uint8_t s_w_address, uint8_t s_reg_address, uint8_t byte)
{
    if(bus_busy()) return (error |= (1 << 1));
    generate_start_ACK(s_w_address);
    transmit_data(s_reg_address);
    transmit_data(byte);
    if(generate_stop()) return error;
    else return 0;
}

/*******************************************************************************
Function Name : I2C_read
Parameters    : uint8_t s_r_address, uint8_t s_reg_address, uint8_t *p_buffer, uint8_t n_bytes
Modifies      : uint8_t *p_buffer
Returns       : uint8_t
Notes         : Read from a slave
 *******************************************************************************/
uint8_t LPI2C0_read(uint8_t s_r_address, uint8_t s_reg_address, uint8_t *p_buffer, uint8_t n_bytes)
{
    if(bus_busy()) return (error |= (1 << 1));
    generate_start_ACK(s_r_address - 1);
    transmit_data(s_reg_address);
    generate_start_ACK(s_r_address);
    receive_data(p_buffer, n_bytes);
    if(generate_stop()) return error;
    else return 0;
}


/*******************************************************************************
Function Name : LPI2C0_Master_IRQHandler
 *******************************************************************************/
void LPI2C0_Master_IRQHandler(void)
{



	  LPIT0->MSR|=1;
	  PTD->PTOR=1;
	  //s_reg_adrress = transmit data = Es el tercer valor 0x6B/0x3B leer o escribir
	  //1010 adress IIC 24LC01B

	  //escribimos un dato para poder leer
	  LPI2C0_write((0x68<<1),0b01010011,0x00);


	  LPI2C0_read((0x68<<1)+1,0b01010011,var_read,2);

	  //movimiento1
	  datos_recibidosI2C[0] =(var_read[0]);
	  //duracion de movimiento1
	  datos_recibidosI2C[1] =(var_read[0]<<8);

	  	  	  	  	 LPI2C0_write((0x68<<1),0x6B,0x00);
	                 LPI2C0_write((0x68<<1),0x1B,0x00);
	        	     LPI2C0_write((0x68<<1),0x1C,0x00);

	        	     //hacemos lectura en posicion x para retrovisores
	        	     LPI2C0_read((0x68<<1)+1,0x3B,aceleracionx,2);
	        	     aceleracion_x =(aceleracionx[0]<<8)+aceleracionx[0];

	        	     //hacemos lectura en posicion y para retrovisores
	        	     LPI2C0_read((0x68<<1)+1,0x3D,aceleraciony,2);
	        	     aceleracion_y =(aceleraciony[0]<<8)+aceleraciony[0];

	        	     //hacemos lectura en posicion z para saber si el robot esta en una pendiete
	        	     LPI2C0_read((0x68<<1)+1,0x3F,aceleracionz,2);
	        	     aceleracion_z =(aceleracionz[0]<<8)+aceleracionz[0];

	        	     LPI2C0_read((0x68<<1)+1,0x43,gyrox,2);


	        	     gyro_x =(gyrox[0]<<8)+gyrox[0];



	        	     LPI2C0_read((0x68<<1)+1,0x45,gyroy,2);
	        	     gyro_y =(gyroy[0]<<8)+gyroy[0];

	        	     LPI2C0_read((0x68<<1)+1,0x47,gyroz,2);
	        	     gyro_z =(gyroz[0]<<8)+gyroz[0];

	               //Metodo de las aceleraciones por las tangentes
	        	     *datos[2]=pitch_y_acc=atan(-1*(((float)aceleracion_x)/16384)/sqrt(pow(((float)aceleracion_y/16384),2)+pow(((float)aceleracion_z/16384),2)))*(57.295779); // 2/32768=1/16384 en terminos de g, 180/pirad=57.295779
	               *datos[1]=roll_x_acc=atan(((float)aceleracion_y/16384)/sqrt(pow(((float)aceleracion_x/16384),2)+pow(((float)aceleracion_z/16384),2)))*(57.295779); //se van a la app

	               //Metodo del giroscopio
	               dt=((float)bandera*(0.03125)-tiempo_prev);
	               tiempo_prev=((float)bandera*(0.03125));

	               //Aplicar el filtro complementario pasa bajas para el acelerometro y pasa altas para el giroscopio
	               roll_x=0.98*(roll_x+(float)((gyro_x)/131)*dt)+0.02*(roll_x_acc); //250/32768=1/131


	               pitch_y=0.98*(pitch_y+(float)((gyro_y)/131)*dt)+0.02*(pitch_y_acc);

	               //Integracion respecto del tiempo para calcular el YAW
	               //resultado final de posicion en z y se guarda en la variable yaw_z
	               *datos[3]=yaw_z=yaw_z+(float)((gyro_z)/131)*dt; //Se va a la app
	               if (yaw_z>30){
	            	   aumentar_corriente=25;
	               }
	               //agignamos valores a la ventana en posicion x y y para poder hacer la orientacion
	               ventanay=pitch_y_acc;
	               ventanax=roll_x_acc;


}



int main(void)
{
	UART_init();
	PORTC_init();
	LPIT0_Ch0_init();

	LPIT0_Ch3_init();

	adc_init();
	ADC1_init();
	//I2C Hardware
	SIRC_div();
	LPI2C0_init();

    while(1){

    }
	return 0;
}
