#include "d_gtk.h"

// Side-to-Side relations
void change_ss_entry(GtkWidget *widget)
{
	return;
}

void toggle_ssYdRel(GtkWidget *widget)
{
	// change relationship from 	outside->inside
	// 				inside->outside
	const gchar *buffer;
	float cx, cy;
	float ssStep;
	int fieldside;
	int yardrel;
	buffer = gtk_button_get_label(GTK_BUTTON(widget));
	cx = pshow->center->x;
	cy = pshow->center->y;
	ssStep = getSidetoSide(&cx, &cy);
	ssStep = roundf(4*ssStep)/4;
	yardrel = isInsideYard(&cx, &cy, &fieldside);
	if (ssStep == 4)
	{
		// don't do anything
	}
	else if ((yardrel == -1 && fieldside == 1) || (yardrel == 1 && fieldside == 2))
	{
		// move 4 steps right
		movexy(2*ssStep, 0);
		gtk_widget_queue_draw_area(window, 0, 0, width, height);
	}
	else if ((yardrel == 1 && fieldside == 1) || (yardrel == -1 && fieldside == 2))
	{
		// move 4 steps left
		movexy(-2*ssStep, 0);
		gtk_widget_queue_draw_area(window, 0, 0, width, height);
	}
	return;
}

void toggle_ssSide(GtkWidget *widget)
{
	return;
}

void toggle_ssYdLine(GtkWidget *widget)
{
	return;
}

// Front-to-back relations
void change_fb_entry(GtkWidget *widget)
{
	return;
}

void toggle_fbHashRel(GtkWidget *widget)
{
	return;
}

void toggle_fbFrontBack(GtkWidget *widget)
{
	return;
}

void toggle_HashSide(GtkWidget *widget)
{
	return;
}

void expand_form(GtkWidget *widget)
{
	// expand the form by 1 step
	scale_form(1);
	gtk_widget_queue_draw_area(window, 0, 0, width, height);
	return;
}

void contract_form(GtkWidget *widget)
{
	// contract the form by 1 step
	scale_form(-1);
	gtk_widget_queue_draw_area(window, 0, 0, width, height);
	return;
}

void rot_cw(GtkWidget *widget)
{
	// rotate form clockwise
	rot_form(M_PI/16);
	gtk_widget_queue_draw_area(window, 0, 0, width, height);
	return;
}

void rot_countercw(GtkWidget *widget)
{
	// rotate form clockwise
	rot_form(-M_PI/16);
	gtk_widget_queue_draw_area(window, 0, 0, width, height);
	return;
}

void rot_cw_small(GtkWidget *widget)
{
	// rotate form clockwise
	rot_form(M_PI/64);
	gtk_widget_queue_draw_area(window, 0, 0, width, height);
	return;
}

void rot_countercw_small(GtkWidget *widget)
{
	// rotate form clockwise
	rot_form(-M_PI/64);
	gtk_widget_queue_draw_area(window, 0, 0, width, height);
	return;
}

void dot_align_to_grid(GtkWidget *widget)
{
	// align dots to 8:5 grid (called from gtk)
	align_dots();
	gtk_widget_queue_draw_area(window, 0, 0, width, height);
	return;
}

void move_up(GtkWidget *widget)
{
	// Move a dot backfield if not stepped
	if (pshow->step == 0)
	{
		movexy(0, -1);
		gtk_widget_queue_draw_area(window, 0, 0, width, height);
	}
}

void move_down(GtkWidget *widget)
{
	// Move a dot frontfield
	if (pshow->step == 0)
	{
		movexy(0, 1);
		gtk_widget_queue_draw_area(window, 0, 0, width, height);
	}
}

void move_left(GtkWidget *widget)
{
	// Move a dot toward left goal line
	if (pshow->step == 0)
	{
		movexy(-1, 0);
		gtk_widget_queue_draw_area(window, 0, 0, width, height);
	}
}

void move_right(GtkWidget *widget)
{
	// Move a dot toward right goal line
	if (pshow->step == 0)
	{
		movexy(1, 0);
		gtk_widget_queue_draw_area(window, 0, 0, width, height);
	}
}

void move_up_small(GtkWidget *widget)
{
	// Move a dot toward back sideline (0.25)
	if (pshow->step == 0)
	{
		movexy(0, -0.25);
		gtk_widget_queue_draw_area(window, 0, 0, width, height);
	}
}

void move_down_small(GtkWidget *widget)
{
	// Move a dot toward front sideline (0.25)
	if (pshow->step == 0)
	{
		movexy(0, 0.25);
		gtk_widget_queue_draw_area(window, 0, 0, width, height);
	}
}
	
void move_left_small(GtkWidget *widget)
{
	// Move a dot toward left goal line (0.25)
	if (pshow->step == 0)
	{
		movexy(-0.25, 0);
		gtk_widget_queue_draw_area(window, 0, 0, width, height);
	}
}

void move_right_small(GtkWidget *widget)
{
	// Move a dot toward right goal line (0.25)
	if (pshow->step == 0)
	{
		movexy(0.25, 0);
		gtk_widget_queue_draw_area(window, 0, 0, width, height);
	}
}

gboolean xy_movement(GtkWidget *widget, GdkEventMotion *event)
{
	// caught a mouse movement event
	float coordx, coordy;
	gchar *buffer;
	coordx = event->x;
	coordy = event->y;
	coordx = (coordx-xo2)/step;
	coordy = (coordy-yo2)/step;
	xy_to_relation(&coordx, &coordy, &buffer);
	gtk_statusbar_pop(GTK_STATUSBAR(statusbar), GPOINTER_TO_INT(context_id));
	gtk_statusbar_push(GTK_STATUSBAR(statusbar), GPOINTER_TO_INT(context_id), buffer);
	g_free(buffer);

	return TRUE;
}
