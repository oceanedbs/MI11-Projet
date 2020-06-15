/*
 * utils.c
 *
 *  Created on: 28 mai 2020
 *      Author: jdm
 */

#include <stdint.h>

#include "serialio.h"
#include "noyau.h"
#include "noyau_file.h"
#include "utils.h"

// Tableau de gestion des tâches endormies sur attente passive
uint16_t _wait_file[MAX_TACHES];

void waitfile_init(void)
{
	uint16_t i;

	for(i=0; i< MAX_TACHES; i++){
		_wait_file[i] = MAX_TACHES;
	}
}

void waitfornticks(uint32_t nticks)
{
	uint16_t tachecourante;

	_lock_();
	if(nticks !=0){
		tachecourante = noyau_get_tc();
		_contexte[tachecourante].cmpt = nticks;
		_wait_file[tachecourante] = tachecourante;
		_contexte[tachecourante].status = SUSP;
		dort();
	}
	_unlock_();
}

void waitfile_process(void)
{
	uint16_t i;

	for(i=0; i< MAX_TACHES; i++){
		if (_wait_file[i] != MAX_TACHES){
			if (i != _wait_file[i]){
				printf("Sortie sur waitfile_treat\n");
				noyau_exit();
			}
			else {
				_contexte[i].cmpt--;
				if (_contexte[i].cmpt == 0){
					if (_contexte[i].status == NCREE) {
					        noyau_exit();
					    }
					    _lock_();
					    if (_contexte[i].status == SUSP) {
					    	_contexte[i].status = EXEC;
					        file_ajoute(i);
					    }
					_wait_file[i] = MAX_TACHES;
				}
			}
		}
	}
}

void str_cat(char* str1, char* str2)
{
	uint16_t i, j;

	  // Pour itérer la première chaîne du début à la fin
	  for (i = 0; str1[i]!='\0'; i++);

	  // Concaténer Str2 dans Str1
	  for (j = 0; str2[j]!='\0'; j++, i++)
	  {
	     str1[i] = str2[j];
	  }
	  str1[i] = '\0';
}

void i_to_a10 (int16_t Nb, char* str)
{
	int16_t temp, i, j, flag_sgn = 0;
	char str_temp[20];

	i = 0;
	if (Nb <0) {
		Nb = -Nb;
		flag_sgn = 1;
	}
	temp = Nb;
	do {
			str_temp[i] = (char) (temp%10+0x30);
			temp = temp/10;
			i++;
	} while(temp != 0);

	j = 0;
	do {
		if (flag_sgn == 1){
			str[j] = '-';
			j++;
		}
			str[j] = str_temp[i-1];
			j++;
			i--;
		} while(i != 0);
	str[j] = '\0';

}




