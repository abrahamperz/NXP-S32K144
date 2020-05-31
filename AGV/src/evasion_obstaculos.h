/*
 * evasion_obstaculos.h
 *
 *  Created on: 29/05/2020
 *      Author: abraham
 */

#ifndef EVASION_OBSTACULOS_H_
#define EVASION_OBSTACULOS_H_

void LPIT0_Ch0_IRQHandler (void);
void LPIT0_Ch0_init(void);
void LPIT0_Ch1_IRQHandler (void);
void LPIT0_Ch1_init(void);
void FTM2_Ch0_Ch1_IRQHandler (void);
void ultrasonico_init (void);
void FTM2_Ch0_Ch1_init(void);

#endif /* EVASION_OBSTACULOS_H_ */
