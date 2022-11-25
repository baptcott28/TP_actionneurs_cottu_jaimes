/**
 * \file motor_cmd.h
 * \brief Fichier contenant les définitions necessaire au moteur.
 * \date 25/11/2022*
 *
 * \author Baptiste Cottu , Diego jaimes
 */

#include "main.h"

#ifndef SRC_MOTOR_CMD_H_
#define SRC_MOTOR_CMD_H_

#define SPEED_MAX 1740			//!< Vitesse maximale que peut atteindre le moteur. Ici, 1750 est le raport cyclique égal à 1, on empêche cette valeur d'être atteinte et dépassée en limittant la consigne de vitesse à 1740

void motor_start(void);
void motor_start_PWM(void);
void motor_set_speed(int speed);
void motor_stop_PWM(void);
void motor_get_current(void);


#endif /* SRC_MOTOR_CMD_H_ */
