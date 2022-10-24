# TP_actionneurs_cottu_jaimes
 
Nous implémentons le shell disponible avec les commandes start, stop,pinout et help. On teste les nouvelles fonctionnalitées qui remplissent bien leur rôle d'affichage.

Réglage du dead time 
Le dead time est calculé sur l'horlage du Sysclock. Compte tenu du fait qu'on ne puisse pas mettre la valeur 1000 dans le registre, on est obligé de diviser le Sysclock par 4 (via Internal clock Division CKD). ON peut alors mettre la valeur 250 dans le break Time, ce qui donne finalement une durée de 5 us.
