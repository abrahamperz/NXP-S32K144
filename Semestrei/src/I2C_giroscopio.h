/*
 * I2C_giroscopio.h
 *
 *  Created on: 22/05/2020
 *      Author: abraham
 */

#ifndef I2C_GIROSCOPIO_H_
#define I2C_GIROSCOPIO_H_

#include "math.h"


extern double pos_y;
extern double pos_x;
extern double pos_z;
unsigned int vel=3.1416;
extern unsigned array[4];

unsigned char error = 0;
unsigned char tx = 0;

extern int8_t aceleracionx[2];
extern int16_t aceleracion_x;

extern int8_t aceleraciony[2];
extern int16_t aceleracion_y;

extern int8_t aceleracionz[2];
extern int16_t aceleracion_z;

extern int8_t gyrox[2];
extern int16_t gyro_x;

extern int8_t gyroy[2];
extern int16_t gyro_y;

extern int8_t gyroz[2];
extern int16_t gyro_z;

extern double pitch_y_acc;
extern double roll_x_acc;
extern double yaw_z;


unsigned long bandera=0;
extern unsigned long tiempo_prev;
extern float dt;
extern double pitch_y;
extern double roll_x;

void SIRC_div(void);
void LPI2C0_init(void);
//static unsigned char bus_busy(void);
void generate_start_ACK(uint8_t address);
//static void transmit_data(uint8_t data);
//static uint8_t generate_stop(void);
//static void receive_data(int8_t *p_buffer, uint8_t n_bytes);
uint8_t LPI2C0_write(uint8_t s_w_address, uint8_t s_reg_address, uint8_t byte);
int8_t LPI2C0_read(uint8_t s_r_address, uint8_t s_reg_address, int8_t *p_buffer, uint8_t n_bytes);
void LPI2C0_Master_IRQHandler(void);
void LPIT0_Ch2_IRQHandler(void);
void LPUART2_RxTx_IRQHandler (void);
void UART2_init(void);


#endif /* I2C_GIROSCOPIO_H_ */
