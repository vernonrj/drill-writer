#include "drillwriter-gtk.h"

// Side-to-Side relations
void coords_change_ss_entry_gtk(GtkWidget *widget)
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
	ssStep = fieldrel_get_side_to_side(&cx, &cy);
	yardrel = fieldrel_check_is_inside_yard(&cx, &cy, &fieldside);
	buffer = gtk_entry_get_text(GTK_ENTRY(widget));
	new_cx = atof(buffer);
	if (ssStep != new_cx)
	{
		// changed
		if ((yardrel == -1 && fieldside == 1)
				|| (yardrel == 1 && fieldside == 2))
		{
			// move right
			coords_movexy(ssStep-new_cx, 0);
		}
		else if ((yardrel == 1 && fieldside == 1)
				|| (yardrel == -1 && fieldside == 2))
		{
			// move left
			coords_movexy(new_cx-ssStep, 0);
		}	
		dr_canvas_refresh(drill);
	}

	return;
}

void coords_toggle_ssYdRel_gtk(GtkWidget *widget)
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
	ssStep = fieldrel_get_side_to_side(&cx, &cy);
	ssStep = roundf(4*ssStep)/4;
	yardrel = fieldrel_check_is_inside_yard(&cx, &cy, &fieldside);
	if (ssStep == 4)
	{
		// don't do anything
	}
	else if ((yardrel == -1 && fieldside == 1) 
			|| (yardrel == 1 && fieldside == 2))
	{
		// move right
		coords_movexy(2*ssStep, 0);
		dr_canvas_refresh(drill);
	}
	else if ((yardrel == 1 && fieldside == 1) 
			|| (yardrel == -1 && fieldside == 2))
	{
		// move left
		coords_movexy(-2*ssStep, 0);
		dr_canvas_refresh(drill);
	}
	return;
}

void coords_toggle_ssSide_gtk(GtkWidget *widget)
{
	// change side from side 1 to side 2
	// 	or side 2 to side 1
	double cx;
	cx = pstate.center->x;

	coords_movexy(2*(80-cx), 0);
	dr_canvas_refresh(drill);

	return;
}

void coords_toggle_ssYdLine_gtk(GtkWidget *widget)
{
	return;
}

// Front-to-back relations
void coords_change_fb_entry_gtk(GtkWidget *widget)
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

	fbStep = fieldrel_get_front_to_back(&cx, &cy, &fb_hashrel,
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
			coords_movexy(0, inOut * (new_cy-fbStep));
			dr_canvas_refresh(drill);
		}
		else if (!strcmp(fb_frontback, "back"))
		{
			// back
			coords_movexy(0, -inOut*(new_cy-fbStep));
			dr_canvas_refresh(drill);
		}
	}
	g_free (fb_hashrel);
	g_free (fb_frontback);
	g_free (fb_hashside);

	return;
}

void coords_toggle_fbHashRel_gtk(GtkWidget *widget)
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
	fbStep = fieldrel_get_front_to_back(&cx, &cy, &fb_hashrel, 
			&fb_frontback, &fb_hashside);
	if (!strcmp(fb_frontback, "front"))
	{
		// front hash/side
		if (!strcmp(fb_hashrel, "inside"))
		{
			// inside front, add
			coords_movexy(0, 2*fbStep);
			dr_canvas_refresh(drill);
		}
		else if (!strcmp(fb_hashrel, "outside"))
		{
			// outside front, subtract
			coords_movexy(0, -2*fbStep);
			dr_canvas_refresh(drill);
		}
	}
	else if (!strcmp(fb_frontback, "back"))
	{
		// back hash/side
		if (!strcmp(fb_hashrel, "inside"))
		{
			// inside back, subtract
			coords_movexy(0, -2*fbStep);
			dr_canvas_refresh(drill);
		}
		else if (!strcmp(fb_hashrel, "outside"))
		{
			// outside back, add
			coords_movexy(0, 2*fbStep);
			dr_canvas_refresh(drill);
		}
	}

	g_free (fb_hashrel);
	g_free (fb_frontback);
	g_free (fb_hashside);

	return;
}

void coords_toggle_fbFrontBack_gtk(GtkWidget *widget)
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
	fbStep = fieldrel_get_front_to_back(&cx, &cy, &fb_hashrel, 
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
		coords_movexy(0, fb * (shstep - 2*fbStep));
		dr_canvas_refresh(drill);
	}
	else if (!strcmp(fb_hashrel, "outside"))
	{
		// outside front hash/side
		coords_movexy(0, fb * (shstep + 2*fbStep));
		dr_canvas_refresh(drill);
	}
	g_free (fb_hashrel);
	g_free (fb_frontback);
	g_free (fb_hashside);
	return;
}

void coords_toggle_HashSide_gtk(GtkWidget *widget)
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
	fieldrel_get_front_to_back(&cx, &cy, &fb_hashrel, 
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
		coords_movexy(0, fb * 32);
		dr_canvas_refresh(drill);
	}
	else if (!strcmp(fb_hashside, "sideline"))
	{
		// front/back sideline
		coords_movexy(0, -fb*32);
		dr_canvas_refresh(drill);
	}

	g_free (fb_hashrel);
	g_free (fb_frontback);
	g_free (fb_hashside);

	return;
}

void coords_expand_form_gtk(GtkWidget *widget)
{
	// expand the form by 1 step
	coords_box_scale_form_from_center(1);
	dr_canvas_refresh(drill);
	return;
}

void coords_contract_form_gtk(GtkWidget *widget)
{
	// contract the form by 1 step
	coords_box_scale_form_from_center(-1);
	dr_canvas_refresh(drill);
	return;
}

void coords_rot_cw_gtk(GtkWidget *widget)
{
	// rotate form clockwise
	coords_rot_selected_around_center(M_PI/16);
	dr_canvas_refresh(drill);
	return;
}

void coords_rot_countercw_gtk(GtkWidget *widget)
{
	// rotate form clockwise
	coords_rot_selected_around_center(-M_PI/16);
	dr_canvas_refresh(drill);
	return;
}

void coords_rot_cw_small_gtk(GtkWidget *widget)
{
	// rotate form clockwise
	coords_rot_selected_around_center(M_PI/64);
	dr_canvas_refresh(drill);
	return;
}

void coords_rot_countercw_small_gtk(GtkWidget *widget)
{
	// rotate form clockwise
	coords_rot_selected_around_center(-M_PI/64);
	dr_canvas_refresh(drill);
	return;
}

void coords_dot_align_to_grid_gtk(GtkWidget *widget)
{
	// align dots to 8:5 grid (called from gtk)
	coords_align_dots_to_grid();
	dr_canvas_refresh(drill);
	return;
}

void coords_move_selected_up(GtkWidget *widget)
{
	// Move a dot backfield if not stepped
	if (pstate.curr_step == 0)
	{
		coords_movexy(0, -1);
		dr_canvas_refresh(drill);
	}
}

void coords_move_selected_down(GtkWidget *widget)
{
	// Move a dot frontfield
	if (pstate.curr_step == 0)
	{
		coords_movexy(0, 1);
		dr_canvas_refresh(drill);
	}
}

void coords_move_selected_left(GtkWidget *widget)
{
	// Move a dot toward left goal line
	if (pstate.curr_step == 0)
	{
		coords_movexy(-1, 0);
		dr_canvas_refresh(drill);
	}
}

void coords_move_selected_right(GtkWidget *widget)
{
	// Move a dot toward right goal line
	if (pstate.curr_step == 0)
	{
		coords_movexy(1, 0);
		dr_canvas_refresh(drill);
	}
}

void coords_move_selected_up_small(GtkWidget *widget)
{
	// Move a dot toward back sideline (0.25)
	if (pstate.curr_step == 0)
	{
		coords_movexy(0, -0.25);
		dr_canvas_refresh(drill);
	}
}

void coords_move_selected_down_small(GtkWidget *widget)
{
	// Move a dot toward front sideline (0.25)
	if (pstate.curr_step == 0)
	{
		coords_movexy(0, 0.25);
		dr_canvas_refresh(drill);
	}
}
	
void coords_move_selected_left_small(GtkWidget *widget)
{
	// Move a dot toward left goal line (0.25)
	if (pstate.curr_step == 0)
	{
		coords_movexy(-0.25, 0);
		dr_canvas_refresh(drill);
	}
}

void coords_move_selected_right_small(GtkWidget *widget)
{
	// Move a dot toward right goal line (0.25)
	if (pstate.curr_step == 0)
	{
		coords_movexy(0.25, 0);
		dr_canvas_refresh(drill);
	}
}

