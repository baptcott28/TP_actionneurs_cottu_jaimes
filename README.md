# TP_actionneurs_cottu_jaimes
 
Nous implémentons le shell disponible avec les commandes start, stop,pinout et help. On teste les nouvelles fonctionnalitées qui remplissent bien leur rôle d'affichage.

Réglage du dead time : 

Le dead time est calculé via l'horlage du SysClock. Compte tenu du fait qu'on ne puisse pas mettre la valeur 1000 dans le registre, on décide de mettre la valeur 250 et de diviser le Sysclock par 4 (via Internal clock Division CKD). Cela donne finalement un dead time théorique de 5us.
