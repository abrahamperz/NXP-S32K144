/*
 * Equipo azul #6
 * Abraham Pérez Martínez A01633926
 * Alma Fernanda De la Torre
 * Jessica Janet Cisneros Gonzalez
 * Alvaro Emmanuel Jimenez
 *
 * En cada programa se encuentra la descripción del mismo
 */

#include "S32K144.h" /* include peripheral declarations S32K144 */
#include "rutas_app.h"
#include "I2C_giroscopio.h"
#include "evasion_obstaculos.h"


int main(void)
{
	//I2C
	SIRC_div();
	LPI2C0_init();

	//Rutas y enviar datos a la app
	UART_init();
	UART2_init();
 	LPIT0_Ch3_init();

 	//Activación de motores
	PORTC_init();

	//Evasión obstaculos
	SCG->SIRCDIV=(1<<8)+(1<<0);
	ultrasonico_init();
	FTM2_Ch0_Ch1_init();
	LPIT0_Ch0_init();
	LPIT0_Ch1_init();

	//rpm del motores



	return 0;
}
