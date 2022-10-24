# TP_actionneurs_cottu_jaimes
 
Nous implémentons le shell disponible avec les commandes start, stop,pinout et help. On teste les nouvelles fonctionnalitées qui remplissent bien leur rôle d'affichage.

# Réglage des fréquences de PWM : 

Nous devons avoir une résolution de 10 bit ce qui signifie une valeur d'ARR plus élevée que 1024. Pour avoir une fréquence finale de 16kHz, nous mettons l'ARR a 1750 et le PSC à 2. Par ailleurs, le duty cycle de la première PWM est de 60%, d'ou la valeur du pulse a 1050, et celui de la deuxième est 0.4, d'ou la valeur du duty cycle a 700.

# Réglage du Dead Time : 

Le dead time est calculé via l'horlage du SysClock. Nous mettons une valeur de 200 dans ce registre ce qui donne une valeur du dead time de 2,8 us

# Plan des PIN :

Le hacheur dispose d'un connecteur 37 pin. On identifie deja la Pin ISO_RESET (Pin 33) par laquelle on va allumer le hacheur a connecter avec PC13. On branche la masse de la carte sur la pin ISO_GND (Pin 36) et le 5V de la carte sur la pin ISO_5V (Pin 37).

On choisit ensuite deux phase sur 3, sur lesquelles on branche les deux PWM génerées. on procède donc comme suit :
- CMD_B_TOP (Pin 11) sur TIM1_CH1 (PA8)
- CMD_Y_TOP (Pin 12) sur TIM1_CH2 (PA9)
- CMD_B_BOT (Pin 29) sur TIM1_CH1N (PA11)
- CMD_Y_BOT (Pin 30) sur TIM1_CH2N (PA12)
