#include "drillwriter.h"

// Tempo storage
int tempo_construct(tempo_t **tempo_r, int anchorpoint)
{
	// Add a new tempo at anchorpoint
	tempo_t *stempo;
	stempo = (tempo_t*) malloc(sizeof(tempo_t));
	if (stempo == NULL)
	{
		// allocation error
		return -1;
	}
	stempo->prev = 0;
	stempo->next = 0;
	stempo->tempo = 120;
	stempo->anchorpoint = anchorpoint;

	// pass by reference back
	*tempo_r = stempo;

	return 0;
}



void change_tempo(int tempo, tempo_t **currtempo_r)
{
	tempo_t *prevtempo;
	tempo_t *currtempo;
	tempo_t *nexttempo;
	tempo_t *stempo;
	tempo_t *othertempo;
	currtempo = *currtempo_r;
	if (tempo >= 30 && tempo <= 250)
	{
		// Tempo is valid. Make changes thusly
		pushTempo(&pstate.undobr, currtempo->tempo);
		if (currtempo->anchorpoint == pstate.setnum)
		{
			// changing an existing tempo
			currtempo->tempo = tempo;
		}
		else
		{
			// making a new node
			stempo = (tempo_t*) malloc(sizeof(tempo_t));
			// store data to new node
			stempo->tempo = tempo;
			stempo->anchorpoint = pstate.setnum;
			// link new node
			stempo->next = currtempo->next;
			currtempo->next = stempo;
			stempo->prev = currtempo;
			currtempo = currtempo->next;
		}
		// Now check to see if node is unneccesary
		// Check to see if node can be deleted
		prevtempo = currtempo->prev;
		if (prevtempo)
		{
			if (prevtempo->tempo == tempo)
			{
				// current node is redundant, can be deleted
				prevtempo->next = currtempo->next;
				nexttempo = currtempo->next;
				if (nexttempo)
				{
					// link previously
					nexttempo->prev = prevtempo;
				}
				free(currtempo);
				currtempo = prevtempo;
			}
		}
		nexttempo = currtempo->next;
		if (nexttempo)
		{
			if (nexttempo->tempo == tempo)
			{
				// next node is redundant, can be deleted
				currtempo->next = nexttempo->next;
				othertempo = nexttempo->next;
				if (othertempo)
				{
					// link next to prv
					othertempo->prev = currtempo;
				}
				free(nexttempo);
			}
		}
	}
	*currtempo_r = currtempo;
	return;
}



void update_tempo(void)
{
	// if the tempo is out of date, change it
	// to the correct tempo
	tempo_t *currtempo;
	tempo_t *othertempo;
	//int cset;
	int nset;

	currtempo = pshow->currtempo;
	// Go back to general location
	if (currtempo->anchorpoint > pstate.setnum)
	{
		// Go backward to tempo
		while (currtempo->anchorpoint > pstate.setnum 
				&& currtempo->prev != NULL)
			currtempo = currtempo->prev;
	}
	else
	{
		// Go forward to tempo
		othertempo = currtempo->next;
		while (othertempo)
		{
			nset = othertempo->anchorpoint;
			// find correct tempo forward
			if (nset <= pstate.setnum)
			{
				// go to next tempo
				currtempo = othertempo;
				othertempo = othertempo->next;
			}
			else
				othertempo = 0;
		}
	}
	// interface with deprecated tempo system
	pshow->currtempo = currtempo;
	return;
}


