/*----------------------------------------------------------------------------*
 * fichier : noyau_test.c                                                     *
 * programme de test du noyaut                                                *
 *----------------------------------------------------------------------------*/

#include <stdint.h>
#include <stdlib.h>

#include "serialio.h"
#include "noyau.h"

#include "TERMINAL.h"
#include "utils.h"
#include "FIFO_GEN.h"



TACHE	tacheA(void);
TACHE	tacheB(void);
TACHE	tacheC(void);
TACHE	tacheD(void);

#define NB_MESSAGES 1024
#define TAILLE_MESSAGE 128

FIFO_GEN fifo_messages;

char messages[NB_MESSAGES*TAILLE_MESSAGE];



TACHE	tacheA(void)
{
	uint16_t idx;
	char temp_string[100];

	puts("------> EXEC tache A");
	active(cree(tacheB));
	active(cree(tacheC));
	active(cree(tacheD));
	while(1){
		 _lock_();
		  if(fifogen_taille_get(&fifo_messages) != 0){
			  fifogen_put(&fifo_messages, &idx);
			  printf(&messages[idx*TAILLE_MESSAGE]);
		  }

		  _unlock_();
		  while(flag_tache_vide != 0){
			  _lock_();
			  if(fifogen_get(&fifo_messages, &idx) != 0){
				  messages[idx*TAILLE_MESSAGE] = '\0';
				  str_cat(&messages[idx*TAILLE_MESSAGE], CODE_BACKGROUND_COLOR);
				  i_to_a10 (16,temp_string);
				  str_cat(&messages[idx*TAILLE_MESSAGE], temp_string);
				  str_cat(&messages[idx*TAILLE_MESSAGE], "m A");
				  i_to_a10 (idx,temp_string);
				  str_cat(&messages[idx*TAILLE_MESSAGE], temp_string);
				  str_cat(&messages[idx*TAILLE_MESSAGE], CODE_RESET_COLOR);
			 }
			 flag_tache_vide = 0;
			 _unlock_();

		  }
  	  }
}

TACHE	tacheB(void)
{
  int i=0;
  uint16_t idx;
  char temp_string[20];

  puts("------> DEBUT tache B");
  while (1) {
	_lock_();
	if(fifogen_get(&fifo_messages, &idx) != 0){
		  messages[idx*TAILLE_MESSAGE] = '\0';
		  str_cat(&messages[idx*TAILLE_MESSAGE], CODE_BACKGROUND_COLOR);
		  i_to_a10 (27,temp_string);
		  str_cat(&messages[idx*TAILLE_MESSAGE], temp_string);
		  str_cat(&messages[idx*TAILLE_MESSAGE], "m B");
		  i_to_a10 (i,temp_string);
		  str_cat(&messages[idx*TAILLE_MESSAGE], temp_string);
		  str_cat(&messages[idx*TAILLE_MESSAGE], CODE_RESET_COLOR);
	}
	_unlock_();
    i++;
    waitfornticks(40);
  }
}

TACHE	tacheC(void)
{
  int i=0;
  uint16_t idx;
  char temp_string[100];

  puts("------> DEBUT tache C");
  while (1) {
    _lock_();
    if(fifogen_get(&fifo_messages, &idx) != 0){
    	  messages[idx*TAILLE_MESSAGE] = '\0';
		  str_cat(&messages[idx*TAILLE_MESSAGE], CODE_BACKGROUND_COLOR);
		  i_to_a10 (201,temp_string);
		  str_cat(&messages[idx*TAILLE_MESSAGE], temp_string);
		  str_cat(&messages[idx*TAILLE_MESSAGE], "m C");
		  i_to_a10 (i,temp_string);
		  str_cat(&messages[idx*TAILLE_MESSAGE], temp_string);
		  str_cat(&messages[idx*TAILLE_MESSAGE], CODE_RESET_COLOR);
    }
    _unlock_();
    waitfornticks(20);
    i++;
  }
}

TACHE	tacheD(void)
{
  int i=0;
  uint16_t idx;
  char temp_string[100];

  puts("------> DEBUT tache D");
  while (1) {
    _lock_();
    if(fifogen_get(&fifo_messages, &idx) != 0){
     	  messages[idx*TAILLE_MESSAGE] = '\0';
  		  str_cat(&messages[idx*TAILLE_MESSAGE], CODE_BACKGROUND_COLOR);
  		  i_to_a10 (202,temp_string);
  		  str_cat(&messages[idx*TAILLE_MESSAGE], temp_string);
  		  str_cat(&messages[idx*TAILLE_MESSAGE], "m D");
  		  i_to_a10 (i,temp_string);
  		  str_cat(&messages[idx*TAILLE_MESSAGE], temp_string);
  		  str_cat(&messages[idx*TAILLE_MESSAGE], CODE_RESET_COLOR);
    }
    _unlock_();
    i++;
    if (i==50) noyau_exit();
    waitfornticks(10);
  }
}

int main()
{
  serial_init(0, 115200);

  test_colors();

  puts("Test noyau");
  puts("Noyau preemptif");

  fifogen_init(&fifo_messages, NB_MESSAGES);

  start(tacheA);
  return(0);
}
