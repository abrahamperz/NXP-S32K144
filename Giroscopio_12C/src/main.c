/*
 * I2C0.c
 *
 *  Created on: 16/10/2019
 *      Author: Jesus
 */

#include "S32K144.h" /* include peripheral declarations S32K144 */


unsigned char error = 0;
double pos_y;
double pos_x;
double pos_z;

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

    LPI2C0->MCCR0 = 0x022E2E0B;
    // [24] DATAVD  0x02
    // [16] SETHOLD 0x2E
    // [8]  CLKHI   0x2E
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
    S32_NVIC->ISER[0] |= (1 << (24 % 32));

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

static void receive_data2(int8_t *p_buffer, uint8_t n_bytes)
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

uint8_t LPI2C0_write2(uint8_t s_w_address, uint8_t s_reg_address, uint8_t byte)
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

int8_t LPI2C0_read2(uint8_t s_r_address, uint8_t s_reg_address, int8_t *p_buffer, uint8_t n_bytes)
{
    if(bus_busy()) return (error |= (1 << 1));
    generate_start_ACK(s_r_address - 1);
    transmit_data(s_reg_address);
    generate_start_ACK(s_r_address);
    receive_data2(p_buffer, n_bytes);
    if(generate_stop()) return error;
    else return 0;
}


/*******************************************************************************
Function Name : LPI2C0_Master_IRQHandler
 *******************************************************************************/
void LPI2C0_Master_IRQHandler(void)
{
    if(LPI2C0->MSR & (1 << 10))
    {
        error |= (1 << 4);
        // NACK/ACK detected and expecting ACK/NACK for address byte
        // When set, the master will transmit a STOP condition and will not initiate a new START
        // condition until this flag has been cleared.
        LPI2C0->MSR = 0x400;     // clear NDF
    }
}


void UART_init(void){

	PCC->PCCn[PCC_PORTB_INDEX] = (1<<30);	//Reloj del PUERTOB
	PORTB->PCR[0]=(2<<8);				//PUERTOB PIN 0 como UART Rx
	PORTB->PCR[1]=(2<<8);				//PUERTOB PIN 1 como UART Tx

	PCC->PCCn[PCC_LPUART0_INDEX] = (1<<30) + (3<<24); //FIRCDIV2
	LPUART0->BAUD = 312;			//Baud rate de 312 para frecuencia de 9600Hz
	//LPUART0->CTRL = (3<<18);		//Habilitar receptor y transmisor

	S32_NVIC->ISER[31/32]|=(1<<(31%32));

}

void LPUART0_RxTx_IRQHandler (void) //CHECHAR QUE BANDERA NOS TRAJO A LA INTERRUPCION! (SOLO SI ESTAS USANDO EL RECEIVER Y EL TRANSMITER)
{
	unsigned char dato_temp;	//Asignamos un dato temporal
	if (LPUART0->CTRL & (1<<23)){		//Checamos que la interrupcion del transmiter este habilitidad
		if((LPUART0->STAT) & (1<<23)){	//Checamos la bandera del transmiter
			if(digito < 4){					//Con esta condicion mandamos las variables que vayammos a usar en blueetoth
				LPUART0->DATA = monitoreo[digito++] + 0x30;	//Se manda la variable numerica digito por digito
			} else {					//Y despues de la variable mandamos una coma que indica que cambiamos de variable
					LPUART0->DATA = ','; //Se manda la coma
					digito = 0;				//reiniciamos t2 a 0
					LPUART0->CTRL&=~(1<<19)+(1<<23);	//Apagamos el transmiter
			}
		}
	}
}



int main(void)
{




    for(;;)
    {


    }
    return 0;
}
