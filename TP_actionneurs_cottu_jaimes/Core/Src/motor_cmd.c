/**
 * \file motor_cmd.c
 * \brief Fichier contenant les fonctions de gestion du moteur
 * \date 25/10/2022*
 *
 * \author Baptiste Cottu , Diego jaimes
 */

#include "motor_cmd.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>

#define MONTEE_CINQ_POURCENT 87			//!< Montée en douceur tout les 5%. (87 corespond à un raport cyclique de 5%)

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;

extern char speed[SPEED_MAX_DIGIT];

/**
 * \fn void motor_start(void)
 * \brief Démarre le timer 2 et met la Pin ISO_RESET à 1 pendant le temps requis (2us). Le reset de ISO_RESET se fait dans l'interruption de tim2
 */
void motor_start(void){
	motor_stop_PWM();
	HAL_GPIO_WritePin(ISO_RESET_GPIO_Port,ISO_RESET_Pin,1);
	HAL_Delay(1);
	HAL_GPIO_WritePin(ISO_RESET_GPIO_Port,ISO_RESET_Pin,0);
	//motor_start_PWM();
}

void motor_stop_PWM(void){
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
}

/**
 * \fn void motor_start_PWM(void)
 * \brief Fonction de racourci pour éviter de réécrire le redémarage des PWM à chaque fois
 */
void motor_start_PWM(void){
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
}

/**
 * \fn void motor_set_speed(int *speed)
 * \brief Récupère et traite la commande de vitesse du moteur
 */
void motor_set_speed(char *speed){
	printf("motor_speed_entered\r\n");
	int res=0;													// atoi = ASCII string to the integer value represented by the string
	//int rapport_cyclique=MONTEE_CINQ_POURCENT; 							//!< Rapport cyclique initial à 5%
	int digit=1;
	for(int i=0;i<SPEED_MAX_DIGIT;i++){
		res=res+((speed[SPEED_MAX_DIGIT-i-1])*digit);					//!< reconstitution de la valeur décimale a partir des digits
		digit=digit*10;
	}
	printf("res : %d\r\n",res);
	if(res>SPEED_MAX){
		res=SPEED_MAX;
	}
	//methode bourin
	TIM1->CCR1=res;
	TIM1->CCR2=SPEED_MAX-res;
	HAL_TIM_PWM_Init(&htim1);
	//motor_start_PWM();

	/*// methode gradation du rapport cyclique
	while(raport_cyclique<res){
		TIM1->CCR1=rapport_cyclique;
		TIM1->CCR2=SPEED_MAX-rapport_cyclique;
		HAL_TIM_PWM_Init(&htim1);
		motor_start_PWM();
		HAL_Delay(300);
		rapport_cyclique=rapport_cyclique+MONTEE_CINQ_POURCENT;
	}
	TIM1->CCR1=res;
	TIM1->CCR2=SPEED_MAX-res;
	HAL_TIM_PWM_Init(&htim1);
	motor_start_PWM();
	 */

}
