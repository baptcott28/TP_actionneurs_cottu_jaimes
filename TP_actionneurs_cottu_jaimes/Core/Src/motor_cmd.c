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

#define GRADATION_UN_POURCENT 17			//!< Montée en douceur tout les 1%. (17 correspond à un rapport cyclique de 1%)
#define DELAY_GRADATION 400					// delay entre chaque gradation de vitesse (en ms)

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern ADC_HandleTypeDef hadc1;

uint8_t current=0;

/**
 * \fn void motor_start(void)
 * \brief Met la Pin ISO_RESET à 1 pendant le temps requis (1ms).
 */
void motor_start(void){
	motor_stop_PWM();
	//printf("moteur start entered\r\n");						// debogage
	HAL_GPIO_WritePin(ISO_RESET_GPIO_Port,ISO_RESET_Pin,1);
	HAL_Delay(1);
	HAL_GPIO_WritePin(ISO_RESET_GPIO_Port,ISO_RESET_Pin,0);
	motor_start_PWM();
}

/**
  * \fn void motor_stop_PWM(void)
  * \brief Etteint toutes les PWM du moteur, et remet les valeur standarts (rapport cyclique de 660)
  *
  */
void motor_stop_PWM(void){
	//printf("moteur_stop_entered\r\n");						// debogage
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
	TIM1->CCR1=850;
	TIM1->CCR2=SPEED_MAX-850;
	HAL_TIM_PWM_Init(&htim1);
}

/**
 * \fn void motor_start_PWM(void)
 * \brief Fonction de racourci pour éviter de réécrire le redémarage des PWM à chaque fois
 */
void motor_start_PWM(void){
	//printf("start_PWM\r\n");									// debogage
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
}

/**
 * \fn void motor_set_speed(int *speed)
 * \brief Récupère et traite la commande de vitesse du moteur. Il y a une méthode bourin, qui met directement le rapport cyclique voulu
 * dans les registres, mais qui cause un 'HALL OVERCURRENT'.
 *
 * La deuxième methode est une méthode de gradation, qui permet d'augmenter ou de diminuer en douceur le rapport cyclique du moteur.
 */
void motor_set_speed(int speed){
	//printf("motor_speed_entered\r\n");							// debogage
	if(speed>SPEED_MAX){
		speed=SPEED_MAX;
	}
	else if (speed<0){
		speed=0;
	}
	//printf("consigne appliquée : %d\r\n",speed);				// debogage

	/*//---------methode bourin--------
	TIM1->CCR1=speed;
	TIM1->CCR2=SPEED_MAX-speed;
	HAL_TIM_PWM_Init(&htim1);
	motor_start_PWM();*/

	//----- methode gradation du rapport cyclique -----
	int rapport_cyclique=0;

	// en montée
	rapport_cyclique= TIM1->CCR1;						//<! prise en compte de la vitesse actuelle
	if (speed>rapport_cyclique){
		while(rapport_cyclique<speed){
			TIM1->CCR1=rapport_cyclique;
			TIM1->CCR2=SPEED_MAX-rapport_cyclique;
			printf("rapport_cyclique : %d\r\n",rapport_cyclique);
			HAL_TIM_PWM_Init(&htim1);
			motor_start_PWM();
			HAL_Delay(DELAY_GRADATION);
			rapport_cyclique=rapport_cyclique+GRADATION_UN_POURCENT;
		}
	}

	// en descente
	else if (speed<rapport_cyclique){
		while(rapport_cyclique>speed){
			TIM1->CCR1=rapport_cyclique;
			TIM1->CCR2=SPEED_MAX-rapport_cyclique;
			printf("rapport_cyclique : %d\r\n",rapport_cyclique);
			HAL_TIM_PWM_Init(&htim1);
			motor_start_PWM();
			HAL_Delay(DELAY_GRADATION);
			rapport_cyclique=rapport_cyclique-GRADATION_UN_POURCENT;
		}
	}
}

void motor_get_current(void){
	printf("motor_get_current_entered\r\n");
	int rapport_cyclique= TIM1->CCR1;
	while((TIM1->CNT)<(rapport_cyclique/2)){
	}
	HAL_ADC_Start(&hadc1);

	if(HAL_ADC_PollForConversion(&hadc1, 100)==HAL_OK){
		//printf("poll for conv bien passé\r\n");
	}
	else{
		printf("Echec poll for conversion\r\n");
	}
	current=HAL_ADC_GetValue(&hadc1);
	float real_current=(0.009*current);
	printf("Current : %f \r\n",real_current);
}
