/*----------------------------------------------------------------------------*
 * fichier : noyau_file.c                                                     *
 * gestion de la file d'attente des taches pretes et actives                  *
 * la file est rangee dans un tableau. ce fichier definit toutes              *
 * les primitives de base                                                     *
 *----------------------------------------------------------------------------*/

#include "noyau_file.h"
#include <stdint.h>
// recuperation du bon fichier selon l'architecture pour la fonction printf
#ifdef __arm__
#include "serialio.h"
#endif
#ifdef __x86_64__
#include <stdio.h>
#endif

/*----------------------------------------------------------------------------*
 * variables communes a toutes les procedures                                 *
 *----------------------------------------------------------------------------*/

/*
 * tableau qui stocke les taches
 * indice = numero de tache
 * valeur = tache suivante
 */
static uint16_t _file[MAX_TACHES];

/*
 * index de queue
 * valeur de l'index de la tache en cours d'execution
 * pointe sur la prochaine tache a activer
 */
static uint16_t _queue;

/*----------------------------------------------------------------------------*
 * fonctions de gestion de la file                                            *
 *----------------------------------------------------------------------------*/

/*
 * initialise la file
 * entre  : sans
 * sortie : sans
 * description : la queue est initialisee à une valeur de tache impossible
 */
void file_init(void) {
    _queue = F_VIDE;
}

/*
 * ajoute une tache dans la file
 * entre  : n numero de la tache a ajouter
 * sortie : sans
 * description : ajoute la tache n en fin de file
 */
void file_ajoute(uint16_t n) {
    if (_queue == F_VIDE) {
        _file[n] = n;
    } else {
        _file[n] = _file[_queue];
        _file[_queue] = n;
    }

    _queue = n;
}

/*
 * retire une tache de la file
 * entre  : t numero de la tache a retirer
 * sortie : sans
 * description : retire la tache t de la file. L'ordre de la file n'est pas
                 modifie
 */
void file_retire(uint16_t t) {
    if (_queue == _file[_queue]) {
        _queue = F_VIDE;
    } else {
        if (t == _queue) {
            _queue = _file[_queue];
            while (_file[_queue] != t) {
                _queue = _file[_queue];
            }
            _file[_queue] = _file[t];
        } else {
            while (_file[_queue] != t) {
                _queue = _file[_queue];
            }
            _file[_queue] = _file[_file[_queue]];
        }
    }
}

/*
 * recherche la tache suivante a executer
 * entre  : sans
 * sortie : numero de la tache a activer
 * description : queue pointe sur la tache suivante
 */
uint16_t file_suivant(void) {
    if (_queue != F_VIDE) {
        _queue = _file[_queue];
    }
    return (_queue);
}

/*
 * affiche la queue, donc la derniere tache
 * entre  : sans
 * sortie : sans
 * description : affiche la valeur de queue
 */
void file_affiche_queue() {
    printf("_queue = %d\n", _queue);
}

/*
 * affiche la file
 * entre  : sans
 * sortie : sans
 * description : affiche les valeurs de la file
 */
void file_affiche() {
    int i;

    printf("Tache   | ");
    for (i = 0; i < MAX_TACHES; i++) {
        printf("%03d | ", i);
    }

    printf("\nSuivant | ");
    for (i = 0; i < MAX_TACHES; i++) {
        printf("%03d | ", _file[i]);
    }
    printf("\n");
}
