/*
 * motor_cmd.h
 *
 *  Created on: Oct 25, 2022
 *      Author: bapti
 */

#include "main.h"

#ifndef SRC_MOTOR_CMD_H_
#define SRC_MOTOR_CMD_H_

#define SPEED_MAX 1740			//!< Vitesse maximale que peut atteindre le moteur. Ici, 1750 est le raport cyclique égal à 1, on empêche cette valeur d'être atteinte et dépassée en limittant la consigne de vitesse à 1740

void motor_start(void);
void motor_start_PWM(void);
void motor_set_speed(int speed);
void motor_stop_PWM(void);


#endif /* SRC_MOTOR_CMD_H_ */
