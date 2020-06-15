/*----------------------------------------------------------------------------*
 * fichier : noyau.c                                                          *
 * mini noyau temps reel fonctionnant sur MC9328MXL                           *
 * ce fichier definit toutes les fonctions du noyau                           *
 *----------------------------------------------------------------------------*/
#include <stdint.h>

#include "serialio.h"
#include "versatile_timer.h"
#include "versatile_interrupt.h"
#include "noyau.h"
#include "utils.h"

/*----------------------------------------------------------------------------*
 * variables internes du noyau                                                *
 *----------------------------------------------------------------------------*/

/*
 * compteur d'activation de chaque tache
 */
static int compteurs[MAX_TACHES];
/*
 * tableau stockant le contexte de chaque tache
 */
CONTEXTE _contexte[MAX_TACHES];
/*
 * numero de la tache en train d'etre executee
 */
volatile uint16_t _tache_c;
/*
 * mémorisation de l'adresse du sommet de la pile après chaque allocation d'un
 * espace de pile à une tâche
 */
uint32_t _tos;
/*
 * variable d'acquittement du timer
 * = 1 s'il faut acquitter le timer
 */
int16_t _ack_timer = 1;

/*
 * variable d'acquittement du flag permettant une ecriture de la tache 0
 * a chaque nouvelle periode timer ou la tache est sur le processeur
 * = 1 s'il faut acquitter le timer
 */
int16_t flag_tache_vide;

/*----------------------------------------------------------------------------*
 * fonctions du noyau                                                         *
 *----------------------------------------------------------------------------*/

/*
 * termine l'execution du noyau
 * entre  : sans
 * sortie : sans
 * description : termine l'execution du noyau, execute en boucle une
 *               instruction vide
 */
void noyau_exit(void) {
    printf("Sortie du noyau\n");
    /* desactivation des interruptions */
    _irq_disable_();
    /* affichage du nombre d'activation de chaque tache !*/
    int j;
    for (j = 0; j < MAX_TACHES; j++)
        printf("\nActivations tache %d : %d", j, compteurs[j]);
    /* Que faire quand on termine l'execution du noyau ? */
    for (;;);
}

/*
 * termine l'execution d'une tache
 * entre  : sans
 * sortie : sans
 * description : une tache dont l'execution se termine n'est plus executee
 *               par le noyau
 *               cette fonction doit etre appelee a la fin de chaque tache
 */
void fin_tache(void) {
    /* on interdit les interruptions */
    _irq_disable_();
    /* la tache est enlevee de la file des taches */
    _contexte[_tache_c].status = CREE;
    file_retire(_tache_c);
    schedule();
}

/*
 * demarrage du system
 * entre  : adresse de la tache a lancer
 * sortie : sans
 * description : active la tache et lance le scheduler
 */
void start(TACHE_ADR adr_tache) {
    short j;
    register unsigned int sp asm("sp");
    /* initialisation de l'etat des taches */
    for (j = 0; j < MAX_TACHES; j++) {
        _contexte[j].status = NCREE;
    }
    /* initialisation de la tache courante */
    _tache_c = 0;
    /* initialisation de la file noyau et de la file d'attente nticks*/
    file_init();
    waitfile_init();
    /* initialisation du sommet de la pile */
    _tos = sp;
    /* passage en mode IRG */
    _set_arm_mode_(ARMMODE_IRQ);
    /* initialisation du pointeur de pile */
    sp = _tos;
    /* passage en mode SYS */
    _set_arm_mode_(ARMMODE_SYS);
    /* on interdit les interruptions */
    _irq_disable_();
    /* initialisation du timer a 100 Hz soit une période de 5ms */
    timer_priodic_setup(0, Period_5ms);
    /* initialisation de l'AITC */
    interrupt_enable(4);
    /* creation et activation de la premiere tache */
    active(cree(adr_tache));
}

/*
 * creation d'une nouvelle tache
 * entre  : adresse de la tache a creer
 * sortie : numero de la tache cree
 * description : la tache est creee en lui allouant une pile et un numero
 *               en cas d'erreur, le noyau doit etre arrete
 */
uint16_t cree(TACHE_ADR adr_tache) {
    /* pointeur d'une case de _contexte */
    CONTEXTE *p;
    /* numero de la derniere tache creee */
    static uint16_t tache = -1;
    /* debut section critique */
    _lock_();
    /* generation du numero de la tache suivante */
    tache++;
    /* arret du noyau si plus de  MAX_TACHES*/
    if (tache >= MAX_TACHES) {
        noyau_exit();
    }
    /* creation du contexte de la nouvelle tache */
    p = &_contexte[tache];
    /* allocation d'une pile a la tache */
    p->sp_ini = _tos;
    /* decrementation du pointeur de pile general, afin que la prochaine tache
     * n'utilise pas la pile allouee pour la tache courante */
    _tos -= PILE_TACHE + PILE_IRQ;
    /* fin section critique */
    _unlock_();
    /* memorisation de l'adresse de debut de la tache */
    p->tache_adr = adr_tache;
    /* mise a jour de l'etat de la tache a CREE */
    p->status = CREE;
    return (tache);
}

/*
 * ajout d'une tache pouvant etre executee a la liste des taches eligibles
 * entre  : numero de la tache a ajouter
 * sortie : sans
 * description : ajouter la tache dans la file d'attente des taches eligibles
 *               en cas d'erreur, le noyau doit etre arrete
 */
void active(uint16_t tache) {
    /* acces au contexte de la tache */
    CONTEXTE *p = &_contexte[tache];
    /* verifie que la tache n'est pas dans l'etat NCREE, sinon arrete le noyau*/
    if (p->status == NCREE) {
        noyau_exit();
    }
    /* debut section critique */
    _lock_();
    /* activation de la tache seulement si elle a ete creee */
    if (p->status == CREE) {
        /* mise a jour de l'etat de la tache a PRET */
        p->status = PRET;
        /* ajoute la tache a la file des taches eligibles */
        file_ajoute(tache);
        /* lancement du scheduler */
        schedule();
    }
    /* fin section critique */
    _unlock_();
}

/*
 * lancement du scheduler pour selectionner une nouvelle tache a executer
 * entre  : sans
 * sortie : sans
 * description : provoque une commutation de tache
 */
void schedule(void) {
    /* debut section critique */
    _lock_();
    /* on simule une exception irq pour forcer un appel correct au scheduler */
    /* remise a zero de l'aquittement du timer */
    _ack_timer = 0;
    /* passage en mode d'exception IRQ */
    _set_arm_mode_(ARMMODE_IRQ);
    /* code assembleur pour simuler une interruption */
    __asm__ __volatile__(
            /* sauvegarde de cpsr dans spsr */
            "mrs  r0, cpsr\t\n"
            "msr  spsr, r0\t\n"
            /* sauvegarde de pc dans lr an ajustant sa valeur */
            "add  lr, pc, #4\t\n"
            /* saut au scheduler */
            "b    scheduler\t\n"
    );
    /* passage en mode system */
    _set_arm_mode_(ARMMODE_SYS);
    /* fin de section critique */
    _unlock_();
}


/*
 * endort la tache courante
 * entre  : sans
 * sortie : sans
 * description : endort la tache courante et lance l'execution d'une nouvelle
 *               tache
 */
void dort(void) {
    _lock_();
    _contexte[_tache_c].status = SUSP;
    file_retire(_tache_c);
    schedule();
    _unlock_();
}

/*
 * recupere le numero de tache courante
 * entre  : sans
 * sortie : valeur de _tache_c
 * description : fonction d'acces a la valeur d'identite de la tache courante
 */
uint16_t 	noyau_get_tc(void){
	return(_tache_c);
}

/*
 * reveille une tache
 * entre  : numero de la tache a reveiller
 * sortie : sans
 * description : reveille une tache
 *               en cas d'erreur, le noyau doit etre arrete
 */
void reveille(uint16_t t) {
    CONTEXTE *p;
    p = &_contexte[t];
    if (p->status == NCREE) {
        noyau_exit();
    }
    _lock_();
    if (p->status == SUSP) {
        p->status = EXEC;
        file_ajoute(t);
    }
    _unlock_();
}

/*-----------------------------------------------------------------------------*
 *                  ORDONNANCEUR preemptif optimise                            *
 *                                                                             *
 *             !! Cette fonction doit s'executer en mode IRQ !!                *
 *  !! Pas d'appel direct ! Utiliser schedule pour provoquer une commutation !!*
 *----------------------------------------------------------------------------*/
void __attribute__((naked)) scheduler(void) {
    register CONTEXTE *p;
    /* pointeur de pile */
    register unsigned int sp asm("sp");
    /* sauvegarde le contexte complet sur la pile IRQ de la tache courante */
    __asm__ __volatile__(
            /* sauvegarde des registres du mode system */
            "stmfd  sp, {r0-r14}^\t\n"
            /* attente d'un cycle pour eviter d'ecraser des donnees */
            "nop\t\n"
            /* ajustement du pointeur de pile  IRQ*/
            "sub    sp, sp, #60\t\n"
            /* sauvegarde du contexte spsr et de lr sur la pile */
            "mrs    r0, spsr\t\n"
            "stmfd  sp!, {r0, lr}\t\n"
    );
    /* reinitialisation du timer si necessaire */
    if (_ack_timer) {
    	// on traite les taches en attentes sur ntiks
    	waitfile_process();
    	// on autorise la tache vide (0) a ecrire
    	flag_tache_vide = 1;
        timer_int_clear(0);
    } else {
        _ack_timer = 1;
    }
    /* initialise le pointeur de pile IRQ de la tache courante a la position
       actuelle du pointeur de pile */
    _contexte[_tache_c].sp_irq = sp;
    /* recherche la prochaine tache a executer */
    _tache_c = file_suivant();
    /* verifie qu'une tache suivante existe, sinon arret du noyau */
    if (_tache_c == F_VIDE) {
        printf("Plus rien à ordonnancer.\n");
        noyau_exit();
    }

    /* on bascule sur la nouvelle tache a executer */
    /* incrémentation du compter d'activation de la nouvelle tache */
    compteurs[_tache_c]++;
    /* p pointe sur la nouvelle tache courante */
    p = &_contexte[_tache_c];
    /* on verifie que la tache est dans l'etat PRET, si oui, on la charge
       si non, elle est deja en cours d'execution donc on restaure la valeur
       de sp pour preparer la restauration du contexte */
    if (p->status == PRET) {
        /* charger sp initial */
        sp = p->sp_ini;
        /* passage en mode SYS */
        _set_arm_mode_(ARMMODE_SYS);
        /* charger sp pour le systeme */
        sp = p->sp_ini - PILE_IRQ;
        /* mise a jour du status de la tache en EXEC */
        p->status = EXEC;
        /* autoriser les interruptions */
        _irq_enable_();
        /* lancement de la tache */
        (*p->tache_adr)();
    } else {
        sp = p->sp_irq;
    }
    /* restauration du contexte complet de la tache depuis la pile IRQ de la
       tache */
    __asm__ __volatile__(
            /* restauration du contexte spsr et de lr */
            "ldmfd  sp!, {r0, lr}\t\n"
            "msr    spsr, r0\t\n"
            /* restauration des registres du mode system */
            "ldmfd  sp, {r0-r14}^\t\n"
            /* attente d'un cycle pour eviter d'ecraser des donnees */
            "nop\t\n"
            /* ajustement du pointeur de pile IRQ */
            "add    sp, sp, #60\t\n"
            /* ajustement de lr pour faire un retour d'exception */
            "subs   pc, lr, #4\t\n"
    );
}

