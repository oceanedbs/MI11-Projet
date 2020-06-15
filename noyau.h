/*----------------------------------------------------------------------------*
 * fichier : noyau.h                                                          *
 * mini noyau temps reel fonctionnant sur MC9328MXL                           *
 * ce fichier declare tous les prototypes, constantes et structures           *
 * necessaires au fonctionnement du noyau                                     *
 *----------------------------------------------------------------------------*/

#ifndef __NOYAU_H__
#define __NOYAU_H__

#include <stdint.h>
#include "noyau_file.h"

/*----------------------------------------------------------------------------*
 * declaration des constantes                                                 *
 *----------------------------------------------------------------------------*/

/*
 * taille maximale de la pile d'une tache
 */
#define PILE_TACHE  8192
/*
 * taille maximale de la pile IRQ d'une tache
 */
#define PILE_IRQ    4096

/*
 * période entre deux intérruption Timer
 */
#define Period_10ms    10
#define Period_5ms     5
#define Period_1ms     1



/*----------------------------------------------------------------------------*
 * definition des fonctions dependant du materiel sous forme de code inline   *
 *----------------------------------------------------------------------------*/

#define _lock_() __asm__ __volatile__(\
        "mrs	r0, cpsr\t\n"\
        "stmfd  sp!,{r0}\t\n"\
        "orr	  r0, #0x80\t\n"\
        "msr    cpsr_c, r0\t\n"\
        :::"r0")

#define _unlock_() __asm__ __volatile__(\
        "ldmfd  sp!,{r0}\t\n"\
        "msr    cpsr_c, r0\t\n"\
        :::"r0")


#define _set_arm_mode_(mode) \
  __asm__ __volatile__(\
      "mrs  r0, cpsr\t\n"\
      "bic  r0, r0, #0x1f\t\n"\
      "orr  r0, r0, %0\t\n"\
      "msr  cpsr_c, r0\t\n"\
      "nop\t\n"\
      :\
      : "I" (mode)\
      : "r0")

/*----------------------------------------------------------------------------*
 * declaration des modes d'exception du processeur                            *
 *----------------------------------------------------------------------------*/

#define ARMMODE_USR   0x10
#define ARMMODE_SYS   0x1f
#define ARMMODE_SVC   0x13
#define ARMMODE_ABT   0x17
#define ARMMODE_UND   0x1b
#define ARMMODE_IRQ   0x12
#define ARMMODE_FIQ   0x11

/*----------------------------------------------------------------------------*
 * declaration des etats que peut prendre une tache                           *
 *----------------------------------------------------------------------------*/

#define NCREE   0         /* Etat non cree          */
#define CREE    0x8000    /* Etat cree ou dormant   */
#define PRET    0x9000    /* Etat eligible          */
#define SUSP    0xA000    /* Etat suspendu          */
#define EXEC    0xC000    /* Etat execution         */

/*----------------------------------------------------------------------------*
 * declaration des types                                                      *
 *----------------------------------------------------------------------------*/

#define TACHE   void
typedef TACHE   (*TACHE_ADR)(void); /* pointeur de taches	*/

/*----------------------------------------------------------------------------*
 * declaration du contexte d'interruption des taches                          *
 *----------------------------------------------------------------------------*/

/*
 * structure du contexte sur la pile IRQ
 */
typedef struct {
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r4;
    uint32_t r5;
    uint32_t r6;
    uint32_t r7;
    uint32_t r8;
    uint32_t r9;
    uint32_t r10;
    uint32_t r11;
    uint32_t r12;
    uint32_t sp;
    uint32_t lr;
    uint32_t lr_irq;
    uint32_t spsr_irq;
} REGS;

/*
 * structure du contexte d'une tache
 */
typedef struct {
    // adresse de debut de la tache
    TACHE_ADR tache_adr;
    // etat courant de la tache
    uint16_t status;
    //valeur initiale de SP
    uint32_t sp_ini;
    //valeur courante de SP
    uint32_t sp_irq;
    //valeur courante decomptage pour reveil
    uint32_t cmpt;
} CONTEXTE;

/*----------------------------------------------------------------------------*
 * declaration des variables                                                 *
 *----------------------------------------------------------------------------*/
extern CONTEXTE _contexte[MAX_TACHES];
extern int16_t flag_tache_vide;

/*----------------------------------------------------------------------------*
 * declaration des prototypes des fonctions du noyau                          *
 *----------------------------------------------------------------------------*/

void 		noyau_exit(void);
void 		fin_tache(void);
uint16_t 	cree(TACHE_ADR adr_tache);
void 		active(uint16_t tache);
void 		schedule(void);
void 		start(TACHE_ADR adr_tache);
void 		dort(void);
void 		reveille(uint16_t tache);
uint16_t 	noyau_get_tc(void);

#endif //__NOYAU_H__
