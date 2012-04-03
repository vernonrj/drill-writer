// Functions like playing, stopping, etc go here (automated set movement)
#include "drill.h"
gboolean play_show (GtkWidget *widget)
{	// Play the show
	gulong dumb_API;
	struct set_proto *nextset;
	int local_tempo = pshow->currtempo->tempo;
	time_elapsed = g_timer_elapsed(timer, &dumb_API);
	if (playing == 1 && time_elapsed >= (double)60/(double)local_tempo && !expose_flag)
	{	
		//set_step++;
		pshow->step++;
		do_field=0;	// don't need to redraw field
		nextset = pshow->currset->next;
		if (pshow->step >= nextset->counts)
		{
			// next set
			set_next();
			if (pshow->currset->next == NULL)
			{
				// last set
				playing = 0;
			}
		}
		//g_print("width=%i\theight=%i\n", width, height);
		expose_flag =1;
		gtk_widget_queue_draw_area(window, 0, 0, width, height);
		//g_print("Play_show %i %i %i\n", setnum, set_step, playing);
		if (playing)
		{
			g_timer_start(timer);
			return TRUE;
		}
		else
			return FALSE;
	}
	else if (playing == 0)
		return FALSE;
}


void stop_show (GtkWidget *widget)
{
	playing=0;
	gtk_widget_queue_draw_area(window, 0, 0, width, height);
}


void queue_show (GtkWidget *widget)//, GtkWidget *window)
{
	if (!playing)
	{
		(void)g_timeout_add(50, (GSourceFunc)play_show, window);
		do_field=0;	// don't need to redraw field
		gtk_widget_queue_draw_area(window, 0, 0, width, height);
		if (pshow->currset->next != NULL)
			playing = 1;
		g_timer_start(timer);	// start up the timer
	}
	else
		stop_show(widget);

}

void play_show_from_start (GtkWidget *widget)
{
	if (!playing)
	{
		pshow->currset = pshow->firstset;
		pshow->prevset = 0;
		pshow->step = 0;
		setnum=0;
		(void)g_timeout_add(50, (GSourceFunc)play_show, window);
		playing=1;
		gtk_widget_queue_draw_area(window, 0, 0, width, height);
		g_timer_start(timer);
	}
}
