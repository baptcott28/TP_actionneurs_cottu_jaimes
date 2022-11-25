# INCLURE LA DOC DOXYGEN

# TP_actionneurs_cottu_jaimes
 
Nous implémentons le shell disponible avec les commandes start, stop,pinout et help. On teste les nouvelles fonctionnalitées qui remplissent bien leur rôle d'affichage.

# Table des matières 
- [Plan des PIN](#plan-des-pin)
- [Réglage des PWMs](#réglage-des-pwms)
- [Séquence de démarrage du hacheur](#séquence-de-démarage-du-hacheur)
- [Contôle de la vitesse du moteur en ligne de commande](#contôle-de-la-vitesse-du-moteur-en-ligne-de-commande)
- [Mesure du courant de phase Y](#mesure-du-courant-de-phase-y)




# Plan des PIN :

Le hacheur dispose d'un connecteur 37 pin. On identifie deja la Pin ISO_RESET (Pin 33) par laquelle on va allumer le hacheur a connecter avec PC13. On branche la masse de la carte sur la pin ISO_GND (Pin 36) et le 5V de la carte sur la pin ISO_5V (Pin 37).

On choisit ensuite deux phase sur 3, sur lesquelles on branche les deux PWM génerées. on procède donc comme suit :
- CMD_B_TOP (Pin 11) sur TIM1_CH1 (PA8)
- CMD_Y_TOP (Pin 12) sur TIM1_CH2 (PA9)
- CMD_B_BOT (Pin 29) sur TIM1_CH1N (PA11)
- CMD_Y_BOT (Pin 30) sur TIM1_CH2N (PA12)
- ISO_RESET (Pin 33) sur GPIO_OUTPUT (PC_0)
- Y_HALL (Pin 35) sur ADC1 (PA0)


# Réglage des PWMs :

## En commande décallée avec résolution de 10 bits : 

Nous devons avoir une résolution de 10 bit ce qui signifie une valeur d'ARR entre  511 et 1024. Pour avoir une fréquence finale de 16kHz, nous mettons l'ARR à 1750 et le PSC à 2. Plus tard, nous voudrons que la commande "start" mette le rapport cyclique a 50% ce qui équivaudra a une valeur de 875.

## Réglage du Dead Time : 

Le dead time est calculé via l'horlage du SysClock. Nous mettons une valeur de 200 dans ce registre ce qui donne une valeur du dead time de 2,8 us

## Vérification des commandes à l'oscilloscope



# Séquence de démarage du hacheur

La pin qui permet le démarrage du hacheur est la pin ISO_RESET (Pin 33 sur le hacheur). Pour démarrer le hacheur, il faut respecter une certaine séquence, c'est à dire imposer un état haut sur cette pin pendant 2us au minimum avant de déclencher les PWMs. C'est le rôle de la fonction `void motor_start(void)`. Cette fonction met la pin à 1, crée un delay de 1 ms et remet la pin à 0. Il faut faire attention à ne pas mettre un délai trop long car le reset du hacheur ne fonctionne pas.  

## Démarage du moteur par appui sur le bouton utilisateur

Pour réaliser cela, il suffit de mettre la pin connectée au bouton (PC13) en mode GPIO_EXTI. Dans l'interruption liée à ce bouton, on appelle la fonction `void motor_start(void)`. Pour le contrôle du demarage en ligne de commande, il suffit d'ajouter cette portion de code au moment où l'ordre "start" est détecté.
```C
else if(strcmp(argv[0],"start")==0){
	HAL_UART_Transmit(&huart2, start, sizeof(start), HAL_MAX_DELAY);
	motor_start();
}
```

# Contôle de la vitesse du moteur en ligne de commande

Pour réaliser cette fonction, l'utilisateur devra entrer une fonction du type "speed XXXX". Cette typologie nous permet d'isoler une première chaine de caractère dans argv[0] ("speed"), et la vitesse demandée dans argv[1] ("XXXX"). C'est le but de la fonction `void motor_set_speed(int speed)`. Pour récuperer la commande dans le shell, ainsi que la vitesse, on écrit le code suivant dans une nouvelle boucle `else if` du shell. 

```C
else if(strcmp(argv[0],"speed")==0){
    HAL_UART_Transmit(&huart2,speed_msg, sizeof(speed_msg), HAL_MAX_DELAY);
	int speed;
	speed=atoi(argv[1]);
	motor_set_speed(speed);
}
``` 
On inclut ensuite une correction d'une potentielle erreur avec le code suivant. La macro `SPEED_MAX` définit la valeur max de la vitesse, ici 1750. De la même manière, si la vitesse entrée devait être négative, on ramènerai cette vitesse à 0.

```C
	if(speed>SPEED_MAX){
		speed=SPEED_MAX;
	}
	else if (speed<0){
		speed=0;
	}
```

## Montée naïve du rapport cyclique
Cette montée naïve est réalisée lorsque l'on veut mettre directement la bonne valeur du rapport cyclique dans les PWMs du moteur. Cela induit un appel de courant trop important de la part du moteur ce qui déclenche une mise en rideau du hacheur avec une 'FAULT OVERCURRENT'. 

```C
    // ------ Methode bourin -------
	TIM1->CCR1=res;
	TIM1->CCR2=SPEED_MAX-res;
	HAL_TIM_PWM_Init(&htim1);
	motor_start_PWM();
```

## Montée prograssive du rapport cyclique

L'idée est d'incrémenter le raport cyclique de 1% toutes les `DELAY_GRADATION` (en ms). Ceci est permit en remplacant les 4 lignes précedentes par le morceau de code suivant dans la fonction `void motor_set_speed(int *speed)`. On ajoute le même bloc pour la descente en vitesse a la suite. La seule chose qui change est la ligne 

`rapport_cyclique=rapport_cyclique-GRADATION_UN_POURCENT;`.

```C
//----- methode gradation du rapport cyclique -----
int rapport_cyclique=0;

// en montée
rapport_cyclique= TIM1->CCR1;	//<! prise en compte de la vitesse actuelle

if (speed>rapport_cyclique){
	while(rapport_cyclique<speed){
		TIM1->CCR1=rapport_cyclique;
		TIM1->CCR2=SPEED_MAX-rapport_cyclique;
		printf("rapport_cyclique : %d\r\n",rapport_cyclique);
		HAL_TIM_PWM_Init(&htim1);
		motor_start_PWM();
		HAL_Delay(DELAY_GRADATION);

        // Augmentation du rapport cyclique d'un pourcent
		rapport_cyclique=rapport_cyclique+GRADATION_UN_POURCENT;
	}
}
```

# Mesure du courant de phase Y

Pour mesurer le courant passant dans le moteur, on décide de le mesurer sur la phase jaune, dont la sortie est sur la pin 35 du hacheur. Nous la branchons sur l'ADC1 (pin PA0). On paramètre l'ADC en `single-ended`, la résolution sur 8 bit comme ça la valeur renvoyée est directement au format d'un entier. 

## Ajout de la commande au shell

Nous ajoutons ensuite la fonctionnalité `current` au shell, ce qui permet d'appeler la fonction `motor_get_current()`. Cette fonction met en oeuvre une mesure de un courant, et affiche la valeur. 

```C
else if(strcmp(argv[0],"current")==0){
	HAL_UART_Transmit(&huart2,current_msg, sizeof(current_msg),            HAL_MAX_DELAY);
	motor_get_current();
}
```

## Ecriture de la fonction motor_get_current()
Pour des questions de fiabilité de la mesure, il faut mesurer le courant entre deux fronts de PWM. Pour cela, on attend d'avoir atteint la demi-periode avant de déclencher une mesure. Ceci est fait avec la ligne 
```C 
while((TIM1->CNT)<(rapport_cyclique/2)){
}
```

On lance ensuite la conversion en enchainant les trois instructions suivantes. On vérifie que la prise de mesure se soit bien passée, et on capture la mesure. 

```C
	HAL_ADC_Start(&hadc1);

	if(HAL_ADC_PollForConversion(&hadc1, 100)==HAL_OK){
		//printf("poll for conv bien passé\r\n");
	}
	else{
		printf("Echec poll for conversion\r\n");
	}
	current=HAL_ADC_GetValue(&hadc1);
```

Finalement, on essaie de transformer cette valeur de sortie de l'ADC en courant à l'aide d'une opértion de proportionalité. En testant cette fonction, elle nous renvoie bin=en une valeur de l'ADC mais la conversion en une valeur de courant ne semble pas être la bonne. 
