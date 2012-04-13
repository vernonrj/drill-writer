#include "drill.h"

// Tempo storage
int tempo_construct(struct tempo_proto **tempo_r, int anchorpoint)
{
	// Add a new tempo at anchorpoint
	struct tempo_proto *stempo;
	stempo = (struct tempo_proto*) malloc(sizeof(struct tempo_proto));
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


void change_tempo(int tempo, struct tempo_proto **currtempo_r)
{
	struct tempo_proto *prevtempo;
	struct tempo_proto *currtempo;
	struct tempo_proto *nexttempo;
	struct tempo_proto *stempo;
	struct tempo_proto *othertempo;
	currtempo = *currtempo_r;
	if (tempo >= 30 && tempo <= 250)
	{
		// Tempo is valid. Make changes thusly
		pushTempo(&pshow->undobr, currtempo->tempo);
		if (currtempo->anchorpoint == setnum)
		{
			// changing an existing tempo
			currtempo->tempo = tempo;
		}
		else
		{
			// making a new node
			stempo = (struct tempo_proto*) malloc(sizeof(struct tempo_proto));
			// store data to new node
			stempo->tempo = tempo;
			stempo->anchorpoint = setnum;
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
				//pshow->currtempo = prevtempo;
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
	struct tempo_proto *currtempo;
	struct tempo_proto *othertempo;
	//int cset;
	int nset;

	currtempo = pshow->currtempo;
	// Go back to general location
	if (currtempo->anchorpoint > setnum)
	{
		// Go backward to tempo
		while (currtempo->anchorpoint > setnum && currtempo->prev != NULL)
			currtempo = currtempo->prev;
	}
	else
	{
		// Go forward to tempo
		othertempo = currtempo->next;
		while (othertempo)
		{
			//cset = currtempo->anchorpoint;
			nset = othertempo->anchorpoint;
			// find correct tempo forward
			if (nset <= setnum)
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
	//tempo = currtempo->tempo;
	pshow->currtempo = currtempo;
	return;
}


