/*
 * utils.h
 *
 *  Created on: 28 mai 2020
 *      Author: jdm
 */

#include "noyau.h"


/*----------------------------------------------------------------------------*
 * declaration des prototypes des fonctions du noyau                          *
 *----------------------------------------------------------------------------*/

/*
 * entrée  : sans (gestion sur une variable globale du noyau
 * sortie : sans
 * description : initialise la file d'attente passive (sleep)
 */
void waitfile_init(void);

/*
 * entrée  : nombre de tick d'attente
 * sortie : sans
 * description : endort une tâche pour un nombre fini de tick noyau
 */
void waitfornticks(uint32_t nticks);

/*
 * entrée  : sans
 * sortie : sans
 * description : parcours la file d'attente, décrémente les compteur et réveille la tache si
 * son compteur arrive à zéro
 */
void waitfile_process(void);

/*
 * entrée  : deux chaines de carractères
 * sortie : sans
 * description : concatène deux cahines de caractères
 */
void str_cat(char* str1, char* str2);

/*
 * entrée  : entier signé 16 bit, pointeur sur chaine de caractère
 * sortie : sans
 * description : convertit en chaine de caractère l'entier d'entrée en base 10 avec signe
 */
void i_to_a10 (int16_t Nb, char* str);






