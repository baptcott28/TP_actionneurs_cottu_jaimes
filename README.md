# TP_actionneurs_cottu_jaimes
 
Nous implémentons le shell disponible avec les commandes start, stop,pinout et help. On teste les nouvelles fonctionnalitées qui remplissent bien leur rôle d'affichage.

Réglage des fréquences de PWM : 

Nous devons avoir une résolution de 10 bit ce qui signifie une valeur d'ARR plus élevée que 1024. Pour avoir une fréquence finale de 16kHz, nous mettons l'ARR a 1750 et le PSC à 2. Par ailleurs, le duty cycle de la première PWM est de 60%, d'ou la valeur du pulse a 1050, et celui de la deuxième est 0.4, d'ou la valeur du duty cycle a 700.

Réglage du dead time : 

Le dead time est calculé via l'horlage du SysClock. Nous mettons une valeur de 200 dans ce registre ce qui donne une valeur du dead time de 2,8 us
