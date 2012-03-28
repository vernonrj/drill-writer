// Functions like playing, stopping, etc go here (automated set movement)
#include "drill.h"
gboolean play_show (GtkWidget *widget)
{	// Play the show
	gulong dumb_API;
	time_elapsed = g_timer_elapsed(timer, &dumb_API);
	//g_print("play_show time=%g %i\n", time_elapsed, playing);
	if (playing==1 && time_elapsed >= (double)60/(double)tempo && !expose_flag)
	{	// animate (currently at 120 bpm)
		//set_step++;
		pshow->step++;
		do_field=0;	// don't need to redraw field
		g_print("width=%i\theight=%i\n", width, height);
		expose_flag =1;
		gtk_widget_queue_draw_area(window, 0, 0, width, height);
		g_print("Play_show %i %i %i\n", setnum, set_step, playing);
		if (playing)
			g_timer_start(timer);
	}
	return TRUE;

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
		do_field=0;	// don't need to redraw field
		gtk_widget_queue_draw_area(window, 0, 0, width, height);
		if (pshow->currset->next != NULL)
			playing = 1;
		/*
		if (setnum+1<set_tot)
			playing=1;	// start playback
		*/
		g_print("queue_show playing=%i\n", playing);
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
		setnum=0;
		gtk_widget_queue_draw_area(window, 0, 0, width, height);
		playing=1;
		g_timer_start(timer);
	}
}
