# INCLURE LA DOC DOXYGEN
# ReadMe pas à jour
# TP_actionneurs_cottu_jaimes
 
Nous implémentons le shell disponible avec les commandes start, stop,pinout et help. On teste les nouvelles fonctionnalitées qui remplissent bien leur rôle d'affichage.

## Réglage des fréquences de PWM : 

Nous devons avoir une résolution de 10 bit ce qui signifie une valeur d'ARR plus élevée que 1024. Pour avoir une fréquence finale de 16kHz, nous mettons l'ARR a 1750 et le PSC à 2. Par ailleurs, le duty cycle de la première PWM est de 60%, d'ou la valeur du pulse a 1050, et celui de la deuxième est 0.4, d'ou la valeur du duty cycle a 700.

## Réglage du Dead Time : 

Le dead time est calculé via l'horlage du SysClock. Nous mettons une valeur de 200 dans ce registre ce qui donne une valeur du dead time de 2,8 us

## Plan des PIN :

Le hacheur dispose d'un connecteur 37 pin. On identifie deja la Pin ISO_RESET (Pin 33) par laquelle on va allumer le hacheur a connecter avec PC13. On branche la masse de la carte sur la pin ISO_GND (Pin 36) et le 5V de la carte sur la pin ISO_5V (Pin 37).

On choisit ensuite deux phase sur 3, sur lesquelles on branche les deux PWM génerées. on procède donc comme suit :
- CMD_B_TOP (Pin 11) sur TIM1_CH1 (PA8)
- CMD_Y_TOP (Pin 12) sur TIM1_CH2 (PA9)
- CMD_B_BOT (Pin 29) sur TIM1_CH1N (PA11)
- CMD_Y_BOT (Pin 30) sur TIM1_CH2N (PA12)

## Séquence de démarage du hacheur

La pin qui permet le démarrage du hacheur est la pin ISO_RESET (Pin 33). Pour démarrer le hacheur, il faut respecter une certaine séquence, c'est à dire imposer un état haut sur cette pin (ISO_RESET) pendant 2us au minimum avant de déclencher les PWMs. C'est le rôle de la fonction `void motor_start(void)`. Cette fonction active le timer 2 et met ISO_RESET à 1. ISO_RESET est remise à l'état bas lors de l'interruption déclenchée par le timer 2. Le timer 2 est désactivé à ce moment là. 

## Démarage du moteur par appui sur le bouton utilisateur

Pour réaliser cela, il suffit de mettre la pin connectée au bouton (PC13) en mode GPIO_EXTI. Dans l'interruption liée à ce bouton, on appelle la fonction `void motor_start(void)`. Pour le contrôle du demarage en ligne de commande, il suffit d'ajouter cette portion de code au moment où l'ordre "start" est détecté.
```C
			else if(strcmp(argv[0],"start")==0)
			{
				HAL_UART_Transmit(&huart2, start, sizeof(start), HAL_MAX_DELAY);
				motor_start();
			}
```

## Contôle de la vitesse du moteur en ligne de commande

Pour réaliser cette fonction, il faut reussir à detecter une commande du type "speed=". Pour cela, on fait appel à la fonction `strncmp(char* chaine1,char* chaine 2, int n)` qui permet de comparer deux chaines de caractères jusqu'à la position n spécifiée en argument. Une fois cela fait, on prélève les 4 digits de la vitesse spécifiée en les mettant dans le buffer `int speed[SPEED_MAX_DIGIT]`, et on le passe en argument de la fonction `void motor_set_speed(int *speed)`. 

Cette fonction convertie d'abord ces digits sous une forme décimale, avant de comparer la valeur obtenue avec la vitesse maximale autorisée. 

Nous avons défini cette vitesse maximale atteinte pour un rapport cyclique proche de 1 (99,4% environ). Nous réglons ensuite la vitesse du moteur par le biai des instructions `	TIM1->CCR1=res` et `TIM1->CCR2=SPEED_MAX-res`.

L'appel de la fonction `void motor_set_speed(int *speed)` est conditionée par le passage dans la boucle suivant qui analse le buffer `cmdBuffer[CMD_BUFFER_SIZE]` : 
```C
			else if(strncmp(argv[0],"speed=",6)==0)
			{
				HAL_UART_Transmit(&huart2,speed_msg, sizeof(speed_msg), HAL_MAX_DELAY);
				for(int i=0;i<SPEED_MAX_DIGIT+1;i++){
					speed[i]=cmdBuffer[i];
				}
				motor_set_speed(speed);
			}
```
#### Montée naïve du rapport cyclique
Cette montée naïve est réalisée lorsque l'on veut mettre directement la bonne valeur du rapport cyclique dans les PWMs du moteur. Cela induit un appel de courant trop important de la part du moteur ce qui à pour effet de mettre en échec le hacheur. 
```C
	TIM1->CCR1=res;
	TIM1->CCR2=SPEED_MAX-res;
	HAL_TIM_PWM_Init(&htim1);
	motor_start_PWM();
```

#### Montée prograssive du rapport cyclique

L'idée est d'incrémenter le raport cyclique de 5% toutes les 300ms. Si jamais le rapport cyclique devait être la valeur maximale autorisée, alors, le moteur mettrait 6 secondes à atteindre sa vitesse maximale ce qui semble être une durée convenable et qui permet certainement de limiter les appels de courant. Ceci est permit en remplacant les 4 lignes précedentes par le morceau de code suivant dans la fonction `void motor_set_speed(int *speed)` :
```C
while(raport_cyclique<res){
		TIM1->CCR1=rapport_cyclique;
		TIM1->CCR2=SPEED_MAX-rapport_cyclique;
		HAL_TIM_PWM_Init(&htim1);
		motor_start_PWM();
		HAL_Delay(300);
		rapport_cyclique=rapport_cyclique+87;
	}
	TIM1->CCR1=res;
	TIM1->CCR2=SPEED_MAX-res;
	HAL_TIM_PWM_Init(&htim1);
	motor_start_PWM();
```
