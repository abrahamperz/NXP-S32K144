/*
 * rutas_app.h
 *
 *  Created on: 22/05/2020
 *      Author: abraham
 */

#ifndef RUTAS_APP_H_
#define RUTAS_APP_H_
#define adelante (1<<14)+(1<<7) //Seleccionamos pines 7 y 14;
#define atras (1<<17)+(1<<15) //Seleccionamos pines 15 y 17;
#define derecha (1<<17)+(1<<7) //Seleccionamos pines 7 y 17;
#define izquierda (1<<15)+(1<<14) //Seleccionamos pines 15 y 14;
#define alto (0<<17)+(0<<15)+(0<<14)+(0<<7)

extern unsigned movimiento;
extern unsigned char duty_cycle=50;
extern unsigned char espejo_pin;
extern unsigned char i=0;
extern unsigned char T=100;
extern unsigned char data=0;
extern unsigned char dato=0;

void LPUART1_RxTx_IRQHandler (void);
void UART_init(void);
void PORTC_init(void);
void LPIT0_Ch0_init(void);
void LPIT0_Ch0_IRQHandler (void);


#endif /* RUTAS_APP_H_ */
