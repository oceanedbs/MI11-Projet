/*----------------------------------------------------------------------------*
 * fichier : FIFO_GEN.h                                                           *
 * gestion d'une file (type FIFO_GEN) circulaire d'index
 * Utile pour faire une file de message	                              *
 *----------------------------------------------------------------------------*/

#ifndef __FIFO_GEN_H__
#define __FIFO_GEN_H__

#include <stdint.h>

/*----------------------------------------------------------------------------*
 * declaration des constantes                                                 *
 *----------------------------------------------------------------------------*/

/*
 * taille maximale de la FIFO
 */
#define TAILLE_FIFO_GEN 1024

/*----------------------------------------------------------------------------*
 * declaration des structures                                                 *
 *----------------------------------------------------------------------------*/

/*
 * structure definissant une file
 */
typedef struct {
    // taille de la file
    uint16_t fifo_taille;
    // index de tete de la file
    // pour rappel, la tete d'une file designe le premier element qui a ete
    // insere
    uint16_t fifo_tete;
    // index de queue de la file
    // pour rappel, la queue d'une file designe le dernier element qui a ete
    // insere
    uint16_t fifo_queue;
    // Nombre d'éléments réellement disponible dans la FIFO
    uint16_t Nb_ptr;
    // tableau qui stocke les donnees de la file
    uint32_t tab[TAILLE_FIFO_GEN];
} FIFO_GEN;

/*----------------------------------------------------------------------------*
 * declaration des prototypes                                                 *
 *----------------------------------------------------------------------------*/

/*
 * initialise la file circulaire
 * entrée  : pointeur sur la file a initialiser, Nb d'elements pointés
 * sortie : sans
 * description : initialise la file circulaire
 */
void fifogen_init(FIFO_GEN *f, uint16_t Nb_ptr);

/*
 * ajoute un element a la file
 * entrée  : pointeur sur la file ou il faut ajouter l'element
 *          pointeur utilisé pour récuperer la valeur d'index ajouté à la file
 * sortie : -1 si succes, O si erreur
 * description : ajoute un element a la file
 */
uint8_t fifogen_get(FIFO_GEN *f, uint16_t* idx);

/*
 * retire un element a la file
 * entrée  : pointeur sur la file ou il faut retirer l'element
 *          pointeur utilisé pour récuperer la valeur retiree de la file
 * sortie : -1 si succes, O si erreur
 * description : retire un element de la file
 */
uint8_t fifogen_put(FIFO_GEN *f, uint16_t* idx);

/*
 * retire un element a la file
 * entrée  : pointeur sur la file ou il faut lire l'element
 * sortie : valeur de la taille
 * description : renvoie le nombre d'élément (taille) dans la file
 */
uint16_t fifogen_taille_get(FIFO_GEN *f);

/*
 * retire un element a la file
 * entrée  : pointeur sur la file ou il faut lire l'element
 * sortie : valeur de la queue
 * description : renvoie l'index pointeur de queue
 */
uint16_t fifogen_queue_get(FIFO_GEN *f);

/*
 * retire un element a la file
 * entrée  : pointeur sur la file ou il faut lire l'element
 * sortie : valeur de la tete
 * description : renvoie l'index pointeur de tete
 */
uint16_t fifogen_tete_get(FIFO_GEN *f);

#endif
