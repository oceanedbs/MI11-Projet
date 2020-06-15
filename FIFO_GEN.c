/*----------------------------------------------------------------------------*
 * fichier : FIFO_GEN.c                                                           *
 * gestion d'une file (type FIFO_GEN) circulaire d'index                             *
 *----------------------------------------------------------------------------*/

#include "FIFO_GEN.h"

/*----------------------------------------------------------------------------*
 * definition des fonctions                                                   *
 *----------------------------------------------------------------------------*/

void fifogen_init(FIFO_GEN *f, uint16_t Nb_ptr)
{
	uint16_t i;

	f->fifo_taille = f->fifo_queue = f->fifo_tete = 0;
	f->Nb_ptr = Nb_ptr;
	for (i=0; i< TAILLE_FIFO_GEN; i++)
		f->tab[i] = TAILLE_FIFO_GEN;
}

uint8_t fifogen_get(FIFO_GEN *f, uint16_t* idx)
{
	if (f->fifo_taille >= f->Nb_ptr) // @suppress("Symbol is not resolved")
	{
		return(0);
	}

	f->tab[f->fifo_queue] = f->fifo_queue;
	*idx = (uint16_t) f->tab[f->fifo_queue];
	f->fifo_queue++;
	if (f->fifo_queue >= f->Nb_ptr)
		f->fifo_queue = 0;
	f->fifo_taille++;
	return(-1);
}

uint8_t fifogen_put(FIFO_GEN *f, uint16_t* idx)
{
	if (f->fifo_taille == 0)
	{
		return(0);
	}
	
	*idx = (uint16_t) f->tab[f->fifo_tete];
	f->tab[f->fifo_tete] = TAILLE_FIFO_GEN;
	f->fifo_tete++;
	if (f->fifo_tete >= f->Nb_ptr)
		f->fifo_tete = 0;
  	f->fifo_taille--;
	return(-1);
}


uint16_t fifogen_taille_get(FIFO_GEN *f)
{
	return(f->fifo_taille);
}

uint16_t fifogen_queue_get(FIFO_GEN *f)
{
	return(f->fifo_queue);
}

uint16_t fifogen_tete_get(FIFO_GEN *f)
{
	return(f->fifo_tete);
}



