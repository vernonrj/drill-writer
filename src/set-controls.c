// Functions that change sets go here
#include "drill.h"

void add_set_gtk(GtkWidget *widget)
{
	if (!playing)
	{
		add_set();
	}
	return;
}

void delete_set_gtk(GtkWidget *widget)
{
	if (!playing)
	{
		delete_set();
	}
	return;
}



void set_first_gtk(GtkWidget *widget)
{
	// Move to first set, do gtk stuff
	if (!playing)
	{
		set_first();
		do_field=0;
		gtk_widget_queue_draw_area(window, 0, 0, width, height);
	}
	return;
}

void set_last_gtk(GtkWidget *widget)
{
	if (!playing)
	{
		set_last();
		do_field=0;
		gtk_widget_queue_draw_area(window, 0, 0, width, height);

	}
	return;
}

void set_next_gtk(GtkWidget *widget)
{
	if (!playing)
	{
		set_next();
		do_field=0;
		//gdk_drawable_get_size(widget->window, &width, &height);
		gtk_widget_queue_draw_area(window, 0, 0, width, height);
	}
	return;
}


void set_next_count_gtk(GtkWidget *widget)
{
	if (!playing)
	{
		set_next_count();
		do_field=0;
		gtk_widget_queue_draw_area(window, 0, 0, width, height);
	}
	return;
}

				
void set_prev_gtk(GtkWidget *widget)
{
	if (!playing)
	{
		set_prev();
		do_field=0;	// don't need to draw field
		gtk_widget_queue_draw_area(window, 0, 0, width, height);
	}
	return;
}



void goto_set (GtkWidget *widget)
{	// Go to set specified in entry_sets
	const gchar *entry_buffer;
	int set_buffer;
	int i = 0;
	struct set_proto *last = pshow->firstset;
	struct set_proto *curr = 0;
	if (!playing)
	{
		entry_buffer = gtk_entry_get_text (GTK_ENTRY (entry_sets));
		set_buffer = atoi(entry_buffer);
		for (i=0; i<set_buffer && last != NULL; i++)
		{
			// go to current set
			curr = last;
			last = last->next;
			i++;
		}
		/*
		if (set_buffer<set_tot)
			setnum=set_buffer;
		*/
		if (last != NULL)
		{
			pshow->currset = last;
			pshow->prevset = curr;
			setnum = i;
		}
		gtk_widget_queue_draw_area(window, 0, 0, width, height);
	}
}



void change_tempo (GtkWidget *widget)
{
	// Change the tempo internally
	// TODO: Non-global tempo
	const gchar *entry_buffer;
	int tmpo;

	entry_buffer = gtk_entry_get_text(GTK_ENTRY(entry_tempo));
	tmpo = atoi(entry_buffer);
	if (tmpo >= 30 && tmpo <= 250)
	{
		// Use a valid tempo
		tempo = tmpo;
	}

	return;
}


int isLastSet(void)
{
	// check to see if we're at the last set
	if (pshow->currset->next == NULL)
	{
		// at the last set
		return 1;
	}
	else
	{
		// not at the last set
		return 0;
	}
	return 0;
}

void add_set(void)
{
	// TODO: Deprecated
	/*
	int i, j;
	//newset_create
	set_tot++;
	for (i=set_tot-1; i>setnum; i--)
	{
		for (j=0; j<perfnum; j++)
		{
			perf[i][j][0] = perf[i-1][j][0];
			perf[i][j][1] = perf[i-1][j][1];
		}
		counts[i] = counts[i-1];
	}
	counts[setnum+1]=1;
	*/
}


void delete_set(void)
{
	// TODO: Deprecated
	/*
	int i, j;
	for (i=setnum; i<set_tot; i++)
	{
		for (j=0; j<perfnum; j++)
		{
			perf[i][j][0] = perf[i+1][j][0];
			perf[i][j][1] = perf[i+1][j][1];
		}
		counts[i] = counts[i+1];
	}
	set_tot--;
	gtk_widget_queue_draw_area(window, 0, 0, width, height);
	*/
}


void set_first(void)
{	// Move to first set
	//if (!playing)
	{
		pshow->currset = pshow->firstset;
		pshow->prevset = 0;
		setnum=0;
		//set_step=0;
	}
}


void set_last(void)
{
	// Goto the last set
	//if (!playing)
	{
		while (pshow->currset->next != NULL)
		{
			if (pshow->prevset == NULL)
				pshow->prevset = pshow->currset;
			else
				pshow->prevset = pshow->prevset->next;
			pshow->currset = pshow->currset->next;
			setnum++;
		}
		pshow->step = 0;

		//setnum=set_tot-1;
		//set_step=0;
	}
}

void set_next(void)
{	// Move to the next set
	//if (!playing)
	{
		if (pshow->currset->next != NULL)
		{
			pshow->prevset = pshow->currset;
			pshow->currset = pshow->currset->next;
			setnum++;
		}
	}
}


void set_next_count(void)
{
	struct set_proto *nextset;
	//if (!playing)
	{	// shouldn't use this when playing
		//printf("set_next setnum=%i\nset_tot=%i\nset_step=%i\n", setnum, set_tot, set_step);
		if (!isLastSet())
		{
			pshow->step++;
			nextset = pshow->currset;
			nextset = nextset->next;
			if (pshow->step >= nextset->counts)
			{
				// to the next set
				pshow->step = 0;
				pshow->prevset = pshow->currset;
				pshow->currset = nextset;
				setnum++;
			}
		}
		/*
		if (setnum+1<set_tot)
			set_step++;
		else
			setnum=set_tot-1;
		if (set_step >= counts[setnum+1] && setnum+1<set_tot)
		{
			set_step=0;
			setnum++;
		}
		*/
	}
}



void set_prev(void)
{
	struct set_proto *last;

	//if (!playing)	// shouldn't use this when playing
	{
		// if in the middle of set,
		// go to beginning of set
		if (pshow->step)
			pshow->step = 0;
		else if (pshow->prevset != NULL)
		{
			// not already at first set, move backwards
			pshow->currset = pshow->prevset;
			last = pshow->firstset;
			if (pshow->currset == last)
			{
				// first set
				pshow->prevset = 0;
				setnum = 0;
			}
			else
			{
				// find previous set
				while (last->next != pshow->currset && last != NULL)
				{
					last = last->next;
				}
				if (last != NULL)
				{
					pshow->prevset = last;
					setnum--;
				}
				else
				{
					pshow->prevset = 0;
					setnum = 0;
				}
			}
		}
		/*
		if (!set_step)
			setnum--;
		else	// go back to start of set, instead of set-1
			set_step=0;
		if (setnum < 0)
			setnum = 0;
		*/
	}
}


