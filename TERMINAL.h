/*
 *TERMINAL.h
 *
 *  Created on: 28 mai 2020
 *      Author: jdm
 */


#ifndef __FIFO_H__
#define __FIFO_H__

#include <stdint.h>
#include "serialio.h"

/*
 * Definition de chaine de caractères pour construire des codes d'échappement pour le terminal
 * Attention il faut compléter par des éléments
 */
#define CODE_FONT_COLOR "\x1B[38;5;"
#define CODE_BACKGROUND_COLOR "\x1B[48;5;"
#define CODE_RESET_COLOR "\x1B[0m"

/*
 * Définition de macro pour envoi de codes d'échappement directement au terminal
 */
#define SET_FONT_COLOR_256(red,green,blue) \
	printf("%s%dm",CODE_FONT_COLOR, 16 + (red * 36) + (green * 6) + blue);

#define SET_FONT_COLOR(color) \
	printf("%s%dm",CODE_FONT_COLOR, color);

#define SET_BACKGROUND_COLOR_256(red,green,blue) \
	printf("%s%dm",CODE_BACKGROUND_COLOR, 16 + (red * 36) + (green * 6) + blue);

#define SET_BACKGROUND_COLOR(color) \
	printf("%s%dm",CODE_BACKGROUND_COLOR, color);

#define RESET_COLOR() \
	printf(CODE_RESET_COLOR);

#define SAUT_DE_LIGNE() \
	printf("\n");

/*----------------------------------------------------------------------------*
 * declaration des prototypes                                                 *
 *----------------------------------------------------------------------------*/

/*
 * entrée  : sans
 * sortie : sans
 * description : code de démonstration de la palette de couleur du terminal utilisant les macro
 * définies plus haut
 */
void test_colors(void);

#endif
