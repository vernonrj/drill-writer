#include "d_gtk.h"

// Side-to-Side relations
void change_ss_entry(GtkWidget *widget)
{
	// change side-to-side
	double cx = pstate.center->x;
	double cy = pstate.center->y;
	int yardrel;
	int fieldside;
	double new_cx;
	double ssStep;
	const gchar *buffer;
	//char *newtext;
	ssStep = getSidetoSide(&cx, &cy);
	yardrel = isInsideYard(&cx, &cy, &fieldside);
	buffer = gtk_entry_get_text(GTK_ENTRY(widget));
	new_cx = atof(buffer);
	if (ssStep != new_cx)
	{
		// changed
		if ((yardrel == -1 && fieldside == 1)
				|| (yardrel == 1 && fieldside == 2))
		{
			// move right
			movexy(ssStep-new_cx, 0);
		}
		else if ((yardrel == 1 && fieldside == 1)
				|| (yardrel == -1 && fieldside == 2))
		{
			// move left
			movexy(new_cx-ssStep, 0);
		}	
		gtk_widget_queue_draw_area(window, 0, 0, 
				fldstate.width, fldstate.height);
	}

	return;
}

void toggle_ssYdRel(GtkWidget *widget)
{
	// change relationship from 	outside->inside
	// 				inside->outside
	//const gchar *buffer;
	double cx, cy;
	double ssStep;
	int fieldside;
	int yardrel;
	//buffer = gtk_button_get_label(GTK_BUTTON(widget));
	cx = pstate.center->x;
	cy = pstate.center->y;
	ssStep = getSidetoSide(&cx, &cy);
	ssStep = roundf(4*ssStep)/4;
	yardrel = isInsideYard(&cx, &cy, &fieldside);
	if (ssStep == 4)
	{
		// don't do anything
	}
	else if ((yardrel == -1 && fieldside == 1) 
			|| (yardrel == 1 && fieldside == 2))
	{
		// move right
		movexy(2*ssStep, 0);
		gtk_widget_queue_draw_area(window, 0, 0, 
				fldstate.width, fldstate.height);
	}
	else if ((yardrel == 1 && fieldside == 1) 
			|| (yardrel == -1 && fieldside == 2))
	{
		// move left
		movexy(-2*ssStep, 0);
		gtk_widget_queue_draw_area(window, 0, 0, 
				fldstate.width, fldstate.height);
	}
	return;
}

void toggle_ssSide(GtkWidget *widget)
{
	// change side from side 1 to side 2
	// 	or side 2 to side 1
	double cx;
	cx = pstate.center->x;

	movexy(2*(80-cx), 0);
	gtk_widget_queue_draw_area(window, 0, 0, fldstate.width, fldstate.height);

	return;
}

void toggle_ssYdLine(GtkWidget *widget)
{
	return;
}

// Front-to-back relations
void change_fb_entry(GtkWidget *widget)
{
	// change front-to-back
	double cx = pstate.center->x;
	double cy = pstate.center->y;
	//int yardrel;
	//int fieldside;
	double new_cy;
	double fbStep;
	const gchar *buffer;
	//char *newtext;
	int inOut;
	gchar *fb_hashrel;
	gchar *fb_frontback;
	gchar *fb_hashside;

	fbStep = getFronttoBack(&cx, &cy, &fb_hashrel,
			&fb_frontback, &fb_hashside);
	buffer = gtk_entry_get_text(GTK_ENTRY(widget));
	new_cy = atof(buffer);
	if (fbStep != new_cy)
	{
		// changed
		if (!strcmp(fb_hashrel, "inside"))
		{
			// inside hash/sideline
			inOut = -1;
		}
		else if (!strcmp(fb_hashrel, "outside"))
		{
			// outside hash/sideline
			inOut = 1;
		}
		else
			inOut = 0;
		// front/back
		if (!strcmp(fb_frontback, "front"))
		{
			// front
			movexy(0, inOut * (new_cy-fbStep));
			gtk_widget_queue_draw_area(window, 0, 0, fldstate.width, fldstate.height);
		}
		else if (!strcmp(fb_frontback, "back"))
		{
			// back
			movexy(0, -inOut*(new_cy-fbStep));
			gtk_widget_queue_draw_area(window, 0, 0, fldstate.width, fldstate.height);
		}
	}
	g_free (fb_hashrel);
	g_free (fb_frontback);
	g_free (fb_hashside);

	return;
}

void toggle_fbHashRel(GtkWidget *widget)
{
	// toggle relation from ins/out
	// 			out/ins
	//const gchar *buffer;
	double fbStep;
	double cx, cy;
	gchar *fb_hashrel;
	gchar *fb_frontback;
	gchar *fb_hashside;
	cx = pstate.center->x;
	cy = pstate.center->y;
	fbStep = getFronttoBack(&cx, &cy, &fb_hashrel, 
			&fb_frontback, &fb_hashside);
	if (!strcmp(fb_frontback, "front"))
	{
		// front hash/side
		if (!strcmp(fb_hashrel, "inside"))
		{
			// inside front, add
			movexy(0, 2*fbStep);
			gtk_widget_queue_draw_area(window, 0, 0, fldstate.width, fldstate.height);
		}
		else if (!strcmp(fb_hashrel, "outside"))
		{
			// outside front, subtract
			movexy(0, -2*fbStep);
			gtk_widget_queue_draw_area(window, 0, 0, fldstate.width, fldstate.height);
		}
	}
	else if (!strcmp(fb_frontback, "back"))
	{
		// back hash/side
		if (!strcmp(fb_hashrel, "inside"))
		{
			// inside back, subtract
			movexy(0, -2*fbStep);
			gtk_widget_queue_draw_area(window, 0, 0, fldstate.width, fldstate.height);
		}
		else if (!strcmp(fb_hashrel, "outside"))
		{
			// outside back, add
			movexy(0, 2*fbStep);
			gtk_widget_queue_draw_area(window, 0, 0, fldstate.width, fldstate.height);
		}
	}

	g_free (fb_hashrel);
	g_free (fb_frontback);
	g_free (fb_hashside);

	return;
}

void toggle_fbFrontBack(GtkWidget *widget)
{
	// toggle relation from front/back
	//const gchar *buffer;
	//buffer = gtk_button_get_label(GTK_BUTTON(widget));
	double fbStep;
	double shstep;
	//double shstart;
	double cx, cy;
	int fb;
	gchar *fb_hashrel;
	gchar *fb_frontback;
	gchar *fb_hashside;
	cx = pstate.center->x;
	cy = pstate.center->y;
	fbStep = getFronttoBack(&cx, &cy, &fb_hashrel, 
			&fb_frontback, &fb_hashside);
	if (!strcmp(fb_hashside, "hash"))
	{
		// hash info
		//shstart = 32;
		shstep = 85-64;
	}
	else
	{
		// sideline info
		shstep = 85;
		//shstart = 0;
	}
	if (!strcmp(fb_frontback, "front"))
	{
		fb = -1;
	}
	else if (!strcmp(fb_frontback, "back"))
	{
		fb = 1;
	}
	else
		fb = 0;

	// front hash/side
	if (!strcmp(fb_hashrel, "inside"))
	{
		// inside front hash/side
		movexy(0, fb * (shstep - 2*fbStep));
		gtk_widget_queue_draw_area(window, 0, 0, fldstate.width, fldstate.height);
	}
	else if (!strcmp(fb_hashrel, "outside"))
	{
		// outside front hash/side
		movexy(0, fb * (shstep + 2*fbStep));
		gtk_widget_queue_draw_area(window, 0, 0, fldstate.width, fldstate.height);
	}
	g_free (fb_hashrel);
	g_free (fb_frontback);
	g_free (fb_hashside);
	return;
}

void toggle_HashSide(GtkWidget *widget)
{
	// toggle hash/sideline
	//const gchar *buffer;
	//buffer = gtk_button_get_label(GTK_BUTTON(widget));
	//double fbStep;
	double cx, cy;
	gchar *fb_hashrel;
	gchar *fb_frontback;
	gchar *fb_hashside;
	cx = pstate.center->x;
	cy = pstate.center->y;
	getFronttoBack(&cx, &cy, &fb_hashrel, 
			&fb_frontback, &fb_hashside);
	int fb;
	if (!strcmp(fb_frontback, "front"))
		fb = 1;
	else if (!strcmp(fb_frontback, "back"))
		fb = -1;
	else
		fb = 0;
	if (!strcmp(fb_hashside, "hash"))
	{
		// front/back hash
		movexy(0, fb * 32);
		gtk_widget_queue_draw_area(window, 0, 0, fldstate.width, fldstate.height);
	}
	else if (!strcmp(fb_hashside, "sideline"))
	{
		// front/back sideline
		movexy(0, -fb*32);
		gtk_widget_queue_draw_area(window, 0, 0, fldstate.width, fldstate.height);
	}

	g_free (fb_hashrel);
	g_free (fb_frontback);
	g_free (fb_hashside);

	return;
}

void expand_form(GtkWidget *widget)
{
	// expand the form by 1 step
	box_scale_form(1);
	gtk_widget_queue_draw_area(window, 0, 0, fldstate.width, fldstate.height);
	return;
}

void contract_form(GtkWidget *widget)
{
	// contract the form by 1 step
	box_scale_form(-1);
	gtk_widget_queue_draw_area(window, 0, 0, fldstate.width, fldstate.height);
	return;
}

void rot_cw(GtkWidget *widget)
{
	// rotate form clockwise
	rot_form(M_PI/16);
	gtk_widget_queue_draw_area(window, 0, 0, fldstate.width, fldstate.height);
	return;
}

void rot_countercw(GtkWidget *widget)
{
	// rotate form clockwise
	rot_form(-M_PI/16);
	gtk_widget_queue_draw_area(window, 0, 0, fldstate.width, fldstate.height);
	return;
}

void rot_cw_small(GtkWidget *widget)
{
	// rotate form clockwise
	rot_form(M_PI/64);
	gtk_widget_queue_draw_area(window, 0, 0, fldstate.width, fldstate.height);
	return;
}

void rot_countercw_small(GtkWidget *widget)
{
	// rotate form clockwise
	rot_form(-M_PI/64);
	gtk_widget_queue_draw_area(window, 0, 0, fldstate.width, fldstate.height);
	return;
}

void dot_align_to_grid(GtkWidget *widget)
{
	// align dots to 8:5 grid (called from gtk)
	align_dots();
	gtk_widget_queue_draw_area(window, 0, 0, fldstate.width, fldstate.height);
	return;
}

void move_up(GtkWidget *widget)
{
	// Move a dot backfield if not stepped
	if (pstate.curr_step == 0)
	{
		movexy(0, -1);
		gtk_widget_queue_draw_area(window, 0, 0, fldstate.width, fldstate.height);
	}
}

void move_down(GtkWidget *widget)
{
	// Move a dot frontfield
	if (pstate.curr_step == 0)
	{
		movexy(0, 1);
		gtk_widget_queue_draw_area(window, 0, 0, fldstate.width, fldstate.height);
	}
}

void move_left(GtkWidget *widget)
{
	// Move a dot toward left goal line
	if (pstate.curr_step == 0)
	{
		movexy(-1, 0);
		gtk_widget_queue_draw_area(window, 0, 0, fldstate.width, fldstate.height);
	}
}

void move_right(GtkWidget *widget)
{
	// Move a dot toward right goal line
	if (pstate.curr_step == 0)
	{
		movexy(1, 0);
		gtk_widget_queue_draw_area(window, 0, 0, fldstate.width, fldstate.height);
	}
}

void move_up_small(GtkWidget *widget)
{
	// Move a dot toward back sideline (0.25)
	if (pstate.curr_step == 0)
	{
		movexy(0, -0.25);
		gtk_widget_queue_draw_area(window, 0, 0, fldstate.width, fldstate.height);
	}
}

void move_down_small(GtkWidget *widget)
{
	// Move a dot toward front sideline (0.25)
	if (pstate.curr_step == 0)
	{
		movexy(0, 0.25);
		gtk_widget_queue_draw_area(window, 0, 0, fldstate.width, fldstate.height);
	}
}
	
void move_left_small(GtkWidget *widget)
{
	// Move a dot toward left goal line (0.25)
	if (pstate.curr_step == 0)
	{
		movexy(-0.25, 0);
		gtk_widget_queue_draw_area(window, 0, 0, fldstate.width, fldstate.height);
	}
}

void move_right_small(GtkWidget *widget)
{
	// Move a dot toward right goal line (0.25)
	if (pstate.curr_step == 0)
	{
		movexy(0.25, 0);
		gtk_widget_queue_draw_area(window, 0, 0, fldstate.width, fldstate.height);
	}
}

gboolean xy_movement(GtkWidget *widget, GdkEventMotion *event)
{
	// caught a mouse movement event
	double coordx, coordy;
	gchar *buffer;
	double xo2, yo2;
	double step;
	int excode;
	xo2 = fldstate.xo2;
	yo2 = fldstate.yo2;
	step = fldstate.canv_step;

	coordx = event->x;
	coordy = event->y;
	pixel_to_field(&coordx, &coordy);
	//coordx = (coordx-xo2)/step;
	//coordy = (coordy-yo2)/step;
	excode = xy_to_relation(&coordx, &coordy, &buffer);
	if (excode == -1)
		return FALSE;
	gtk_statusbar_pop(GTK_STATUSBAR(statusbar), GPOINTER_TO_INT(context_id));
	gtk_statusbar_push(GTK_STATUSBAR(statusbar), GPOINTER_TO_INT(context_id), buffer);
	g_free(buffer);

	return TRUE;
}
