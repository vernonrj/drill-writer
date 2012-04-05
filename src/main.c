// Drill Writer Prototype Version 0.0.0.6
// main.c
#include <stdlib.h>

#include "drill.h"		// header file, with most global variables
//#include "media.c"		// playback functions
//#include "set-controls.c"	// Various functions concerning the use of sets
//#include "perf-con.c"		// Various functions for performers/dots
//#include "file-ops.c"		// File operations
//#include "count-con.c"		// count structure

extern int expose_flag;		// might not be required
//extern static GtkActionEntry entries;
//extern static guint n_entries;

//GtkWidget *entry_perf;
//GtkWidget *entry_perf_x;
//GtkWidget *entry_perf_y;



int movexy(float xoff, float yoff)
{
	// move selected dots by xoff and yoff
	float x, y;
	struct coord_proto **coords = pshow->currset->coords;
	struct select_proto *selects = pshow->select;
	while(selects != NULL)
	{
		retr_coord(coords[selects->index], &x, &y);
		x = x + xoff;
		y = y + yoff;
		set_coord(coords[selects->index], x, y);
		selects = selects->next;
	}
	// move center of selection
	x = pshow->center->x;
	y = pshow->center->y;
	x = x + xoff;
	y = y + yoff;
	pshow->center->x = x;
	pshow->center->y = y;
	return 0;
}


void dot_align_to_grid(GtkWidget *widget)
{
	// align dots to 8:5 grid (called from gtk)
	align_dots();
	gtk_widget_queue_draw_area(window, 0, 0, width, height);
	return;
}


int align_dots(void)
{
	// align selected dots to 8:5 grid
	struct coord_proto **coords = pshow->currset->coords;
	struct select_proto *select = pshow->select;
	float x, y;
	while (select != NULL)
	{
		retr_coord(coords[select->index], &x, &y);
		x = roundf(x);
		y = roundf(y);
		set_coord(coords[select->index], x, y);
		select = select->next;
	}
	// move center of selection
	update_sel_center();
	return 0;
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


int movexy_grid(float xoff, float yoff)
{
	// move selected dots by xoff and yoff on 1-step intervals
	float x, y;
	struct coord_proto **coords = pshow->currset->coords;
	struct select_proto *selects = pshow->select;
	while(selects != NULL)
	{
		retr_coord(coords[selects->index], &x, &y);
		x = floorf(x + xoff);
		y = floorf(y + yoff);
		set_coord(coords[selects->index], x, y);
		selects = selects->next;
	}
	// move center of selection
	x = pshow->center->x;
	y = pshow->center->y;
	x = x + xoff;
	y = y + yoff;
	pshow->center->x = x;
	pshow->center->y = y;
	return 0;
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

void next_perf(GtkWidget *widget)
{
	// Go to next sequential dot
	if (perf_cur < pshow->perfnum-1)
	{
		perf_cur++;
		gtk_widget_queue_draw_area(window, 0, 0, width, height);
	}
}

void prev_perf(GtkWidget *widget)
{
	// Go to previous sequential dot
	if (perf_cur > 0)
	{
		perf_cur--;
		gtk_widget_queue_draw_area(window, 0, 0, width, height);
	}
}

static void not_implemented ()
{	// If I don't have something yet, callbacks go here
	g_print("This isn't done yet...\n");
}

void force_redraw(GtkWidget *widget)
{	// Refresh the field
	do_field=1;
	gtk_widget_queue_draw_area(window, 0, 0, width, height);
}

static void quit_action ()
{	// Quit (will have more here later)
	gtk_main_quit();
}

int isInsideYard(float *x, float *y, int *field_side)
{
	// check to see if a dot is inside or outside
	// a yardline
	float ssrel;
	float coordx, coordy;
	int relation;
	coordx = *x;
	coordy = *y;
	// get side-to-side
	ssrel = coordx / 8;
	ssrel = 8*(ssrel - (int)ssrel);
	if (ssrel > 4)
	{
		// to the right of yardline
		if (coordx > 80)
		{
			// inside side 2
			*field_side = 2;
			return 1;
		}
		else
		{
			// outside side 1
			*field_side = 1;
			return -1;
		}
	}
	else if (ssrel < 4)
	{
		// to the left of yardline
		if (coordx > 80)
		{
			// outside side 2
			*field_side = 2;
			return -1;
		}
		else
		{
			// inside side 1
			*field_side = 1;
			return 1;
		}
	}
	else if (ssrel == 4)
	{
		if (coordx > 80)
		{
			// split side 2 yardline
			*field_side = 2;
			return 1;
		}
		else
		{
			// on side 1 yardline
			*field_side = 1;
			return 1;
		}
	}
	// on the yardline
	return 0;
}

int getYardline(float *x, float *y)
{
	// get yardline number
	int yardline;
	float coordx, coordy;
	float inCheck;
	coordx = *x;
	coordy = *y;
	yardline = (coordx+4)/8;
	yardline = abs(yardline - 10);
	yardline = 5 * abs(10 - yardline);
	inCheck = (coordx+4)/8;
	if (coordx < 80 && inCheck == floorf(inCheck))
		yardline = yardline - 5;
	return yardline;
}


float getSidetoSide(float *x, float *y)
{
	// get the side-to-side relation
	float ssrel;
	float coordx, coordy;

	coordx = *x;
	coordy = *y;
	ssrel = coordx / 8;
	ssrel = 8*(ssrel - (int)ssrel);
	ssrel = (int)(ssrel*4);
	ssrel = ssrel / 4;
	if (ssrel > 4)
		ssrel = 8 - ssrel;
	return ssrel;
}

float getFronttoBack(float *x, float *y, gchar **inorout_r, gchar **frontback_r, gchar **hashorside_r)
{
	// check to see if a dot is inside or outside
	// a hash/sideline, and whether it's on the front or back
	int back_side = 0;
	int back_hash = 32;
	int front_hash = 53;
	int front_side = 85;
	gchar *inorout;
	gchar *frontback;
	gchar *hashorside;
	float fbrel;
	// back hash to back sideline
	int bhbs = (back_hash - back_side) / 2 + back_side;
	// front hash to back hash
	int bhfh = (front_hash - back_hash) / 2 + back_hash;	
	// front sideline to front hash
	int fhfs = (front_side - front_hash) / 2 + front_hash;
	float coordx = *x;
	float coordy = *y;
	// Get front-to-back
	if (coordy == back_side)
	{
		// coord is on back sideline
		fbrel = 0;
		inorout = g_strdup_printf("on");
		frontback = g_strdup_printf("back");
		hashorside = g_strdup_printf("sideline");
	}
	else if (coordy < bhbs)
	{
		// coord is close to back sideline
		fbrel = coordy;
		inorout = g_strdup_printf("inside");
		frontback = g_strdup_printf("back");
		hashorside = g_strdup_printf("sideline");
	}
	else if (coordy < back_hash)
	{
		// coord is behind back hash
		fbrel = back_hash - coordy;
		inorout = g_strdup_printf("outside");
		frontback = g_strdup_printf("back");
		hashorside = g_strdup_printf("hash");
	}
	else if (coordy == back_hash)
	{
		// coord is on back hash
		fbrel = 0;
		inorout = g_strdup_printf("on");
		frontback = g_strdup_printf("back");
		hashorside = g_strdup_printf("hash");
	}
	else if (coordy < bhfh)
	{
		// coord is close, but in front of, back hash
		fbrel = coordy - back_hash;
		inorout = g_strdup_printf("inside");
		frontback = g_strdup_printf("back");
		hashorside = g_strdup_printf("hash");
	}
	else if (coordy < front_hash)
	{
		// coord is close to, but behind, front hash
		fbrel = front_hash - coordy;
		inorout = g_strdup_printf("inside");
		frontback = g_strdup_printf("front");
		hashorside = g_strdup_printf("hash");
	}
	else if (coordy == front_hash)
	{
		// coord is on front hash
		fbrel = 0;
		inorout = g_strdup_printf("on");
		frontback = g_strdup_printf("front");
		hashorside = g_strdup_printf("hash");
	}
	else if (coordy < fhfs)
	{
		// coord is close to, but in front of, front hash
		fbrel = coordy - front_hash;
		inorout = g_strdup_printf("outside");
		frontback = g_strdup_printf("front");
		hashorside = g_strdup_printf("hash");
	}
	else if (coordy < front_side)
	{
		// coord is close to, but behind, front sideline
		fbrel = front_side - coordy;
		inorout = g_strdup_printf("inside");
		frontback = g_strdup_printf("front");
		hashorside = g_strdup_printf("sideline");
	}
	else
	{
		// coord is on front sideline
		fbrel = 0;
		inorout = g_strdup_printf("on");
		frontback = g_strdup_printf("front");
		hashorside = g_strdup_printf("sideline");
	}
	*inorout_r = inorout;
	*frontback_r = frontback;
	*hashorside_r = hashorside;
	return fbrel;
}


void xy_to_relation(float *x, float *y, gchar **buffer_r)
{
	// convert event to side-side and front-back relation
	int yardline;
	int relation;
	int sidetoside;

	float ssrel;
	float fbrel;
	gchar *buffer;
	gchar *sideside_relation;
	gchar *side;
	gchar *hash;
	gchar *frontback_relation;
	float coordx, coordy;
	gchar *inorout;
	gchar *frontback;
	gchar *hashorside;
	// relation splits


	coordx = *x;
	coordy = *y;
	// Get side-to-side
	relation = (int)coordx % 8;
	relation = isInsideYard(x, y, &sidetoside);
	ssrel = getSidetoSide(x, y);
	if (relation == 1)
	{
		// inside
		if (sidetoside == 2)
			sideside_relation = g_strdup_printf("inside side 2");
		else
			sideside_relation = g_strdup_printf("inside side 1");
	}
	else if (relation == -1)
	{
		// outside
		if (sidetoside == 2)
			sideside_relation = g_strdup_printf("outside side 2");
		else
			sideside_relation = g_strdup_printf("outside side 1");
	}
	else
	{
		// on yardline
		if (sidetoside == 2)
			sideside_relation = g_strdup_printf("on side 2");
		else
			sideside_relation = g_strdup_printf("on side 1");
	}
	//ssrel = labs(8 - ssrel);
	// Get yardline relation
	yardline = getYardline(x, y);

	fbrel = getFronttoBack(x, y, &inorout, &frontback, &hashorside);
	frontback_relation = g_strdup_printf("%.2f %s %s %s", fbrel, inorout, frontback, hashorside);
	g_free(inorout);
	g_free(frontback);
	g_free(hashorside);

	buffer = g_strdup_printf("(%.2f, %.2f): %.2f %s %i %s", coordx, coordy, ssrel, sideside_relation, yardline, frontback_relation);
	g_free(sideside_relation);
	g_free(frontback_relation);

	*buffer_r = buffer;
	return;
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

gboolean clicked(GtkWidget *widget, GdkEventButton *event)
{
	// Handle click event on canvas
	
	//printf("ping\n");
	// loop vars
	int i, j;
	// number of performers
	int perfnum;
	// location of click
	float coordx, coordy;
	// dot of performer being checked
	float workx, worky;
	//double closex, closey;
	// Length from click location to nearest dot
	// Click must be closer than 3 steps
	double dist_threshold = 9;
	double distance;
	// store type of click
	guint state;
	int found_dot = 0;


	//printf("Event = %i\n", event->button);
	//g_print("Event = %i\n", event->state);
	//g_print("%g, %g\n", (event->x-xo2)/step, (event->y-yo2)/step);
	state = event->state;
	if (event->button == 1)
	{
		if (state == 0)
		{
			// normal click; discard other clicks
			select_discard();
		}
		coordx = event->x;
		coordy = event->y;
		//printf("x = %g, y = %g\n", coordx, coordy);
		// Adjust for various canvas offsets
		coordx = (coordx-xo2)/step;
		//coordy = (coordy-yo2-25)/step;
		coordy = (coordy-yo2)/step;

		//printf("button 1 pressed at %g %g %g\n", coordx, coordy, yo2);
		perfnum = pshow->perfnum;
		//for (i=0; i<perfnum; i++)
		perf_cur = 0;
		for (i=0; i<perfnum; i++)
		{
			// TODO: Maybe use a BST later
			retr_midset(pshow->currset, i, &workx, &worky);
			workx = workx - coordx;
			worky = worky - coordy;
			distance = pow(workx, 2) + pow(worky, 2);
			if (distance < dist_threshold && pshow->perfs[i]->valid == 1)
			{
				// Found a closer dot
				perf_cur = i;
				dist_threshold = distance;
				found_dot = 1;
			}
		}
		if (found_dot == 1)
			select_add(perf_cur);
		gtk_widget_queue_draw_area(window, 0, 0, width, height);
	}
	return TRUE;
}

void calc_stepsize (GtkWidget *widget)
{
	/*
	double dy, dx;
	double stepsize;
	dx = pow(perf[setnum][perf_cur][0] - perf[setnum-1][perf_cur][0], 2);
	dy = pow(perf[setnum][perf_cur][1] - perf[setnum-1][perf_cur][1], 2);
	stepsize = 8*counts[setnum];
	stepsize = stepsize/sqrt(dy + dx);
	printf("stepsize = %g:5\n", stepsize);
	*/
}

int startTk(int argc, char *argv[])
{
	// Start Toolkit of choice
	// gtk
	gtk_init(&argc, &argv);
	return 0;
}

int update_entries(void)
{
	int tempo;
	char set_buf[5];
	char count_buf[5];
	char tempo_buf[5];
	char perf_buf[5];
	char perf_buf_x[5];
	char perf_buf_y[5];
	// side-to-side entries/buttons
	char ss_buf[20];
	char ss_ydrel[10];
	char ss_siderel[8];
	char ss_ydline[3];
	// front-to-back entries/buttons
	float ssStep;
	float fbStep;
	int yardRel;
	int fieldSide;
	float cx, cy;
	char fb_buf[20];
	char fb_hashrel[10];
	char fb_frontback[8];
	char fb_hashside[10];
	// Update all the entries
	cx = pshow->center->x;
	cy = pshow->center->y;
	ssStep = getSidetoSide(&cx, &cy);
	sprintf(ss_buf, "%.2f", ssStep);
	yardRel = isInsideYard(&cx, &cy, &fieldSide);
	if (yardRel == -1)
	{
		// outside
		sprintf(ss_ydrel, "outside");
	}
	else if (yardRel == 1)
	{
		// inside
		sprintf(ss_ydrel, "inside");
	}
	else
	{
		// on
		sprintf(ss_ydrel, "on");
	}
	sprintf(ss_siderel, "side %i", fieldSide);
	sprintf(ss_ydline, "%i", getYardline(&cx, &cy));
	gtk_entry_set_text(GTK_ENTRY (sidesideBtns.ssStepEntry), ss_buf);
	gtk_button_set_label(GTK_BUTTON (sidesideBtns.ssYdRelButton), ss_ydrel);
	gtk_button_set_label(GTK_BUTTON (sidesideBtns.ssSide), ss_siderel);
	gtk_button_set_label(GTK_BUTTON (sidesideBtns.ssYdLine), ss_ydline);
	
	// Convert all numbers into strings first
	if (!pshow->currset->name[0])
		sprintf(set_buf, "%i", setnum);
	else
		strcpy(set_buf, pshow->currset->name);
	update_tempo();
	tempo = pshow->currtempo->tempo;
	sprintf(tempo_buf, "%i", tempo);
	//printf("tempo = %i\n", tempo);
	if (pshow->step)
	{
		// take the counts from the next set
		sprintf(count_buf, "%i", pshow->currset->next->counts);
	}
	else
	{
		// Take the counts from this set
		sprintf(count_buf, "%i", pshow->currset->counts);
	}
	// side-to-side relations
	sprintf(perf_buf, "%i", perf_cur);
	//retr_coord(pshow->currset->coords[perf_cur], &x, &y);
	sprintf(perf_buf_x, "%g", pshow->center->x);
	sprintf(perf_buf_y, "%g", pshow->center->y);
	// Now Update entries with new data
	gtk_entry_set_text(GTK_ENTRY (entry_sets), set_buf);
	gtk_entry_set_text(GTK_ENTRY (entry_tempo), tempo_buf);
	gtk_entry_set_text(GTK_ENTRY (entry_counts), count_buf);
	gtk_entry_set_text(GTK_ENTRY (entry_perf), perf_buf);
	//gtk_entry_set_text(GTK_ENTRY (entry_perf_x), perf_buf_x);
	//gtk_entry_set_text(GTK_ENTRY (entry_perf_y), perf_buf_y);
	return 0;
}

int buildIfacegtk(void)
{
	GtkActionGroup *action_group;	// menus
	GtkUIManager *menu_manager;	// menus
	GError *error;	
	GtkWidget *menubar;		// for menus
	GtkWidget *toolbar;
	// Container Widgets
	GtkWidget *box0;		// second-level packing box (for canvas)
	GtkWidget *box1;
	GtkWidget *setbox;		// Set attributes go in this box
	GtkWidget *perfbox;		// Dot attributes go in this box
	GtkWidget *box3;
	GtkWidget *menu_box;		// First-level packing box (for menus)
	GtkWidget *media_box;		// For media buttons
	// buttons, checks, etc
	GtkWidget *button;		// buttons
	GtkWidget *check;
	GtkWidget *label;
	GtkWidget *separator;
	GtkWidget *image;
	gchar *sbinfo;

	// Field relation buttons
	//struct gtk_ssRel sidesideBtns;
	//struct gtk_fbRel frbkBtns;

	gint tmp_pos;

	int tempo;
	char set_buf[5];
	char count_buf[5];
	char tempo_buf[5];
	char perf_buf[5];
	char perf_buf_x[5];
	char perf_buf_y[5];
	char ss_buf[20];
	char fb_buf[20];

	first_time = 1;


	// Build the menu
	// TODO: Gtk errors have something to do with menu
	static GtkActionEntry entries[] =
	{
	{ "FileMenuAction", NULL, "_File" },	// name, stock id, label
	{ "ToolsMenuAction", NULL, "_Tools"},
	{ "HelpMenuAction", NULL, "_Help"},
	//{ "RunMenuAction", NULL, "_Run" },
	{ "SetMenuAction", NULL, "_Set" },
	//{ "AddMenuAction", NULL, "_Add"},
	{ "EditMenuAction", NULL, "_Edit"},
	{ "ViewMenuAction", NULL, "_View"},
	//{ "MoveMenuAction", NULL, "_Move"},
	{ "DotMenuAction", NULL, "_Dot"},
		
	// File Menu
	{ "NewAction", GTK_STOCK_NEW,
		"_New", "<control>N",
		"Create a new file",
		G_CALLBACK (not_implemented) },
	{ "OpenAction", GTK_STOCK_OPEN,
		"_Open", "<control>O",
		"Open an existing file",
		G_CALLBACK (wrap_load_dep) },
	{ "SaveAction", GTK_STOCK_SAVE,
		"_Save", "<control>S",
		"Save file",
		G_CALLBACK (save_file) },
	{ "SaveAsAction", NULL,
		"_Save As", NULL,
		"Save to new file",
		G_CALLBACK (not_implemented) },
	{ "ExportPDFAction", NULL,
		"_Export to PDF", NULL,
		"Export show to PDF file",
		G_CALLBACK (not_implemented) },
	{ "QuitAction", GTK_STOCK_QUIT,		// name, stock id
		"_Quit", "<control>Q",		// label, accelerator
		"Exit Program",			// tooltip
		G_CALLBACK (gtk_main_quit) },
	// Edit Menu
	{ "SelectAllAction", NULL,
		"_Select All", "<control>A",
		"Select All Performers",
		G_CALLBACK (select_all_gtk) },
	{ "AddPerfAction", NULL,
		"_Add Performer", NULL,
		"Add a performer to the show",
		G_CALLBACK (add_perf) },
	{ "DelPerfAction", NULL,
		"_Delete Performer", NULL,
		"Delete a performer from the show",
		G_CALLBACK (delete_perf) },
	{ "AddSetAction", NULL,
		"_Add Set", NULL,
		"Add a set to the show",
		G_CALLBACK (add_set_gtk) },
	{ "DelSetAction", NULL,
		"_Delete Set", NULL,
		"Delete a set from the show",
		G_CALLBACK (delete_set_gtk) },
	// View Menu
	{ "ZoomInAction", NULL,
		"_Zoom In", "<control>equal",
		"Zoom the field in",
		G_CALLBACK(zoom_in) },
	{ "ZoomOutAction", NULL,
		"_Zoom Out", "<control>minus",
		"Zoom the field out",
		G_CALLBACK(zoom_out) },
	{ "ZoomStandardAction", NULL,
		"_Zoom Standard", "<control>0",
		"Zoom the field to 100%",
		G_CALLBACK(zoom_standard) },
	// Dot Menu
	{ "AlignGridAction", NULL,
		"_Align to Grid", "<control>G",
		"Align selected dots to 8:5 grid",
		G_CALLBACK(dot_align_to_grid) },
	{ "MoveUpAction", NULL,
		"_Move Up", "<control>Up",
		"Move performer up 1 step",
		G_CALLBACK (move_up) },
	{ "MoveDownAction", NULL,
		"_Move Down", "<control>Down",
		"Move performer down 1 step",
		G_CALLBACK (move_down) },
	{ "MoveLeftAction", NULL,
		"_Move Left", "<control>Left",
		"Move performer Left 1 step",
		G_CALLBACK (move_left) },
	{ "MoveRightAction", NULL,
		"_Move Right", "<control>Right",
		"Move performer right 1 step",
		G_CALLBACK (move_right) },
	{ "MoveUpSmallAction", NULL,
		"_Move Up (small)", "<control><shift>Up",
		"Move performer up 0.25 steps",
		G_CALLBACK (move_up_small) },
	{ "MoveDownSmallAction", NULL,
		"_Move Down (small)", "<control><shift>Down",
		"Move performer down 0.25 steps",
		G_CALLBACK (move_down_small) },
	{ "MoveLeftSmallAction", NULL,
		"_Move Left (small)", "<control><shift>Left",
		"Move performer Left 0.25 steps",
		G_CALLBACK (move_left_small) },
	{ "MoveRightSmallAction", NULL,
		"_Move Right (small)", "<control><shift>Right",
		"Move performer right 0.25 steps",
		G_CALLBACK (move_right_small) },
	{ "NextPerfAction", NULL,
		"_Next Performer", "bracketright",
		"Highlight next performer",
		G_CALLBACK (next_perf) },
	{ "PrevPerfAction", NULL,
		"_Previous Performer", "bracketleft",
		"Highlight previous performer",
		G_CALLBACK (prev_perf) },
	{ "ExpandPerfAction", NULL,
		"_Expand Form", "<control>bracketright",
		"Expand the form around weighted center",
		G_CALLBACK (expand_form) },
	{ "ContractPerfAction", NULL,
		"_Contract Form", "<control>bracketleft",
		"Contract the form around weighted center",
		G_CALLBACK (contract_form) },
	{ "RotCWPerfAction", NULL,
		"_Rotate CW", "<control>R",
		"Rotate the form clockwise around weighted center",
		G_CALLBACK(rot_cw) },
	{ "RotCCWPerfAction", NULL,
		"_Rotate CCW", "<control>E",
		"Rotate the form counterclockwise around weighted center",
		G_CALLBACK(rot_countercw) },
	{ "RotCWSPerfAction", NULL,
		"_Rotate CW (small)", "<control><shift>R",
		"Rotate the form a little clockwise around weighted center",
		G_CALLBACK(rot_cw_small) },
	{ "RotCCWSPerfAction", NULL,
		"_Rotate CCW (small)", "<control><shift>E",
		"Rotate the form a little counterclockwise around weighted center",
		G_CALLBACK(rot_countercw_small) },
	// Set Menu
	{ "SetSetNameAction", NULL,
		"_Set setname", NULL,
		"Set current page name",
		G_CALLBACK (set_set_name_gtk) },
	{ "FirstSetAction", NULL,
		"_First Set", "Home",
		"Go to first set",
		G_CALLBACK (set_first_gtk) },
	{ "NextSetAction", NULL,
		"_Next Set", "Next",
		"Go to next set",
		G_CALLBACK(set_next_gtk) },
	{ "StepNextSetAction", NULL,
		"_Next Count", "<control>Next",
		"Go to next count",
		G_CALLBACK (set_next_count_gtk) },
	{ "StepPrevSetAction", NULL,
		"_Prev Count", "<control>Prior",
		"Go to previous count",
		G_CALLBACK (set_prev_count_gtk) },
	{ "PrevSetAction", NULL,
		"_Previous Set", "Prior",
		"Go to previous set",
		G_CALLBACK (set_prev_gtk) },
	{ "LastSetAction", NULL,
		"_Last Set", "End",
		"Go to last set",
		G_CALLBACK (set_last_gtk) },
	{ "PlayHereAction", NULL,
		"_Play", "<control>Return",
		"Play from current set",
		G_CALLBACK (queue_show) },
	{ "PlayFromStartAction", NULL,
		"_Play from Start", "<control><shift>Return",
		"Play from first set",
		G_CALLBACK (play_show_from_start) },
	{ "StopPlayAction", NULL,
		"_Stop", NULL,
		"Stop playback",
		G_CALLBACK (stop_show) },
	{ "ForceRedrawAction", NULL,
		"_Force Redraw", "F5",
		"Refresh the canvas",
		G_CALLBACK (force_redraw) },
	{ "StepsizeAction", NULL,
		"_Calculate Step-size", NULL,
		"Find the step size of selected dot",
		G_CALLBACK (calc_stepsize) },
	{ "SnapshotAction", NULL,
		"_Snapshot", "<shift>S",
		"Save current page to *.png",
		G_CALLBACK (not_implemented) },
	{ "AboutAction", NULL, 
		"_About", NULL,
		"About this Program",
		G_CALLBACK (not_implemented) }
	};
	static guint n_entries = G_N_ELEMENTS (entries);

	// timer setup
	timer = g_timer_new();
	g_timer_stop(timer);


	// Window Actions
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_add_events (window, GDK_BUTTON_PRESS_MASK);
	gtk_window_set_title(GTK_WINDOW(window), "Drill-Writer Pre-Alpha");
	gtk_window_set_default_size(GTK_WINDOW(window), 1000, 700);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	
	// Might need these in the future
	//gtk_widget_set_app_paintable(window, TRUE);
	//gtk_widget_set_double_buffered(window, FALSE);


	// Window Signals
	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

	// Menu Stuff
	menu_box = gtk_vbox_new (FALSE, 0);
	action_group = gtk_action_group_new ("TestActions");
	menu_manager = gtk_ui_manager_new ();

	// Hook up window to menu_box
	gtk_container_add (GTK_CONTAINER (window), menu_box);
	gtk_action_group_add_actions (action_group, entries, n_entries, NULL);
	gtk_ui_manager_insert_action_group (menu_manager, action_group, 0);

	// Read menu UI from XML file
	error = NULL;
	gtk_ui_manager_add_ui_from_file (menu_manager, "menu-ui.xml", &error);
	if (error)
	{
		g_message ("building menus failed: %s", error->message);
		g_error_free (error);
	}

	// Get the menubar and toolbar and put them in vertical packing box
	menubar = gtk_ui_manager_get_widget (menu_manager, "/MainMenu");
	gtk_box_pack_start (GTK_BOX (menu_box), menubar, FALSE, FALSE, 0);

	// Make sure the accelerators work
	gtk_window_add_accel_group (GTK_WINDOW (window),
			gtk_ui_manager_get_accel_group (menu_manager));

	// Start packing canvas
	box0 = gtk_vbox_new (FALSE, 0);
	gtk_box_pack_start(GTK_BOX (menu_box), box0, TRUE, TRUE, 0);

	// Set attributes (set, counts, tempo, etc)
	setbox = gtk_hbox_new (FALSE, 0);	
	gtk_box_pack_start(GTK_BOX (box0), setbox, FALSE, FALSE, 0);

	// create field canvas scroll container
	scrolled_window = gtk_scrolled_window_new (NULL, NULL);
	gtk_container_set_border_width(GTK_CONTAINER(scrolled_window), 10);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
			GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
	gtk_box_pack_start(GTK_BOX(box0), scrolled_window, TRUE, TRUE, 0);
	gtk_widget_show(scrolled_window);

	// get and pack canvas
	drill = gtk_drill_new();
	gtk_widget_set_size_request(drill, 801, 426);
	gtk_scrolled_window_add_with_viewport(
			GTK_SCROLLED_WINDOW(scrolled_window), drill);
	//gtk_box_pack_start(GTK_BOX (box0), drill, TRUE, TRUE, 0);
	//g_signal_connect(window, "button-press-event", G_CALLBACK(clicked), NULL);
	// draw the field the first time
	do_field=1;

	perfbox = gtk_hbox_new (FALSE, 0);	// Dot attributes
	gtk_box_pack_start(GTK_BOX (box0), perfbox, FALSE, FALSE, 0);


	media_box = gtk_vbox_new (FALSE, 0);
	gtk_box_pack_start(GTK_BOX (box0), media_box, FALSE, FALSE, 0);

	// Media Controls (First, prev, next, last)
	box1 = gtk_hbox_new (FALSE, 0);	
	gtk_box_pack_start(GTK_BOX (media_box), box1, FALSE, FALSE, 0);

	label = gtk_label_new ("Set:");
	gtk_box_pack_start (GTK_BOX (setbox), label, FALSE, TRUE, 0);

	// need to change these to spin buttons
	sprintf(set_buf, "%i", setnum);
	entry_sets = gtk_entry_new ();
	gtk_entry_set_max_length (GTK_ENTRY (entry_sets), 50);
	g_signal_connect(entry_sets, "activate", G_CALLBACK (goto_set), entry_sets);
	gtk_entry_set_text (GTK_ENTRY (entry_sets), set_buf);
	tmp_pos = GTK_ENTRY (entry_sets)->text_length;
	gtk_editable_select_region (GTK_EDITABLE (entry_sets),
			0, GTK_ENTRY (entry_sets)->text_length);
	gtk_entry_set_alignment (GTK_ENTRY (entry_sets), 1);
	gtk_entry_set_width_chars(GTK_ENTRY (entry_sets), 4);
	gtk_box_pack_start (GTK_BOX (setbox), entry_sets, FALSE, TRUE, 0);
	gtk_widget_show(entry_sets);

	//separator = gtk_vseparator_new ();
	//gtk_box_pack_start (GTK_BOX (setbox), separator, FALSE, TRUE, 0);

	label = gtk_label_new ("Counts:");
	gtk_box_pack_start (GTK_BOX (setbox), label, FALSE, TRUE, 0);

	// need to change these to spin buttons
	sprintf(count_buf, "%i", pshow->currset->counts);
	entry_counts = gtk_entry_new ();
	gtk_entry_set_max_length (GTK_ENTRY (entry_counts), 50);
	g_signal_connect(entry_counts, "activate", G_CALLBACK (change_counts), entry_counts);
	gtk_entry_set_text (GTK_ENTRY (entry_counts), count_buf);
	tmp_pos = GTK_ENTRY (entry_counts)->text_length;
	//gtk_editable_insert_text (GTK_EDITABLE (entry_counts), " world", -1, &tmp_pos);
	gtk_editable_select_region (GTK_EDITABLE (entry_counts),
			0, GTK_ENTRY (entry_counts)->text_length);
	gtk_entry_set_alignment (GTK_ENTRY (entry_counts), 1);
	gtk_entry_set_width_chars(GTK_ENTRY (entry_counts), 4);
	gtk_box_pack_start (GTK_BOX (setbox), entry_counts, FALSE, TRUE, 0);
	gtk_widget_show(entry_counts);

	// Tempo box (embedded in set attribute box)
	label = gtk_label_new ("Tempo");
	gtk_box_pack_start(GTK_BOX(setbox), label, FALSE, TRUE, 0);

	// Tempo entry box
	tempo = pshow->currtempo->tempo;
	sprintf(tempo_buf, "%i", tempo);
	entry_tempo = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(entry_tempo), 50);
	g_signal_connect(entry_tempo, "activate", G_CALLBACK(change_tempo_gtk), entry_tempo);
	gtk_entry_set_text(GTK_ENTRY(entry_tempo), tempo_buf);
	tmp_pos = GTK_ENTRY(entry_tempo)->text_length;
	gtk_editable_select_region(GTK_EDITABLE(entry_tempo),
			0, GTK_ENTRY(entry_tempo)->text_length);
	gtk_entry_set_alignment(GTK_ENTRY(entry_tempo), 1);
	gtk_entry_set_width_chars(GTK_ENTRY(entry_tempo), 4);
	gtk_box_pack_start(GTK_BOX(setbox), entry_tempo, FALSE, TRUE, 0);


	// Side-to-Side
	// Field Step entry
	sprintf(ss_buf, "%i", 0);
	sidesideBtns.ssStepEntry = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(sidesideBtns.ssStepEntry), 50);
	g_signal_connect(sidesideBtns.ssStepEntry, "activate",
		       	G_CALLBACK(change_ss_entry), sidesideBtns.ssStepEntry);
	gtk_entry_set_text(GTK_ENTRY(sidesideBtns.ssStepEntry), ss_buf);
	tmp_pos = GTK_ENTRY(sidesideBtns.ssStepEntry)->text_length;
	gtk_editable_select_region(GTK_EDITABLE(sidesideBtns.ssStepEntry),
			0, GTK_ENTRY(sidesideBtns.ssStepEntry)->text_length);
	gtk_entry_set_alignment(GTK_ENTRY(sidesideBtns.ssStepEntry), 1);
	gtk_entry_set_width_chars(GTK_ENTRY(sidesideBtns.ssStepEntry), 4);
	gtk_box_pack_start(GTK_BOX(perfbox), sidesideBtns.ssStepEntry, FALSE, TRUE, 0);

	// yardline relation
	sidesideBtns.ssYdRelButton = gtk_button_new_with_label ("inside");
	g_signal_connect(sidesideBtns.ssYdRelButton,
		       	"clicked", G_CALLBACK(toggle_ssYdRel), NULL);
	gtk_box_pack_start(GTK_BOX (perfbox), 
			sidesideBtns.ssYdRelButton, FALSE, FALSE, 0);
	gtk_widget_show(sidesideBtns.ssYdRelButton);

	// side of the field
	sidesideBtns.ssSide = gtk_button_new_with_label ("Side 1");
	g_signal_connect(sidesideBtns.ssSide,
		       	"clicked", G_CALLBACK(toggle_ssSide), NULL);
	gtk_box_pack_start(GTK_BOX (perfbox), 
			sidesideBtns.ssSide, FALSE, FALSE, 0);
	gtk_widget_show(sidesideBtns.ssSide);

	// yardline
	sidesideBtns.ssYdLine = gtk_button_new_with_label ("50");
	g_signal_connect(sidesideBtns.ssYdLine,
		       	"clicked", G_CALLBACK(toggle_ssYdLine), NULL);
	gtk_box_pack_start(GTK_BOX (perfbox), 
			sidesideBtns.ssYdLine, FALSE, FALSE, 0);
	gtk_widget_show(sidesideBtns.ssYdLine);


	// Front-To-Back
	// Field Step entry
	sprintf(fb_buf, "%i", 0);
	frbkBtns.fbStepEntry = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(frbkBtns.fbStepEntry), 50);
	g_signal_connect(frbkBtns.fbStepEntry, "activate",
		       	G_CALLBACK(change_fb_entry), frbkBtns.fbStepEntry);
	gtk_entry_set_text(GTK_ENTRY(frbkBtns.fbStepEntry), fb_buf);
	tmp_pos = GTK_ENTRY(frbkBtns.fbStepEntry)->text_length;
	gtk_editable_select_region(GTK_EDITABLE(frbkBtns.fbStepEntry),
			0, GTK_ENTRY(frbkBtns.fbStepEntry)->text_length);
	gtk_entry_set_alignment(GTK_ENTRY(frbkBtns.fbStepEntry), 1);
	gtk_entry_set_width_chars(GTK_ENTRY(frbkBtns.fbStepEntry), 4);
	gtk_box_pack_start(GTK_BOX(perfbox), frbkBtns.fbStepEntry, FALSE, TRUE, 0);

	// Hash relation button
	frbkBtns.fbHashRelButton = gtk_button_new_with_label ("inside");
	g_signal_connect(frbkBtns.fbHashRelButton,
		       	"clicked", G_CALLBACK(toggle_fbHashRel), NULL);
	gtk_box_pack_start(GTK_BOX (perfbox), 
			frbkBtns.fbHashRelButton, FALSE, FALSE, 0);
	gtk_widget_show(frbkBtns.fbHashRelButton);

	// Front/Back toggle button
	frbkBtns.fbToggleButton = gtk_button_new_with_label ("Front");
	g_signal_connect(frbkBtns.fbToggleButton,
		       	"clicked", G_CALLBACK(toggle_fbFrontBack), NULL);
	gtk_box_pack_start(GTK_BOX (perfbox), 
			frbkBtns.fbToggleButton, FALSE, FALSE, 0);
	gtk_widget_show(frbkBtns.fbToggleButton);

	// Hash/Sideline toggle button
	frbkBtns.HashSideButton = gtk_button_new_with_label ("Sideline");
	g_signal_connect(frbkBtns.HashSideButton,
		       	"clicked", G_CALLBACK(toggle_HashSide), NULL);
	gtk_box_pack_start(GTK_BOX (perfbox), 
			frbkBtns.HashSideButton, FALSE, FALSE, 0);
	gtk_widget_show(frbkBtns.HashSideButton);


	/*
	label = gtk_label_new ("Performer:");
	gtk_box_pack_start (GTK_BOX (perfbox), label, FALSE, TRUE, 0);
	*/

	sprintf(perf_buf, "%i", perf_cur);
	entry_perf = gtk_entry_new ();
	gtk_entry_set_max_length (GTK_ENTRY (entry_perf), 5);
	g_signal_connect (entry_perf, "activate", G_CALLBACK (goto_perf), entry_perf);
	gtk_entry_set_text (GTK_ENTRY (entry_perf), perf_buf);
	//tmp_pos = GTK_ENTRY (entry_counts)->text_length;
	gtk_entry_set_alignment(GTK_ENTRY (entry_perf), 1);
	gtk_entry_set_width_chars(GTK_ENTRY (entry_perf), 4);
	gtk_box_pack_start (GTK_BOX (perfbox), entry_perf, FALSE, TRUE, 0);
	/*


	label = gtk_label_new ("X:");
	gtk_box_pack_start (GTK_BOX (perfbox), label, FALSE, TRUE, 0);

	//sprintf(perf_buf_x, "%g", pshow->currset->coords[perf_cur]->x);
	sprintf(perf_buf_x, "%g", pshow->center->x);
	entry_perf_x = gtk_entry_new ();
	gtk_entry_set_max_length (GTK_ENTRY (entry_perf_x), 5);
	g_signal_connect (entry_perf_x, "activate", G_CALLBACK (xperf_change), entry_perf_x);
	gtk_entry_set_text (GTK_ENTRY (entry_perf_x), perf_buf_x);
	gtk_entry_set_alignment(GTK_ENTRY (entry_perf_x), 1);
	gtk_entry_set_width_chars(GTK_ENTRY (entry_perf_x), 4);
	gtk_box_pack_start (GTK_BOX (perfbox), entry_perf_x, FALSE, TRUE, 0);

	label = gtk_label_new ("Y:");
	gtk_box_pack_start (GTK_BOX (perfbox), label, FALSE, TRUE, 0);

	//sprintf(perf_buf_y, "%g", pshow->currset->coords[perf_cur]->y);
	sprintf(perf_buf_y, "%g", pshow->center->y);
	entry_perf_y = gtk_entry_new ();
	gtk_entry_set_max_length (GTK_ENTRY (entry_perf_y), 5);
	g_signal_connect (entry_perf_y, "activate", G_CALLBACK (yperf_change), entry_perf_y);
	gtk_entry_set_text (GTK_ENTRY (entry_perf_y), perf_buf_y);
	gtk_entry_set_alignment(GTK_ENTRY (entry_perf_y), 1);
	gtk_entry_set_width_chars(GTK_ENTRY (entry_perf_y), 4);
	gtk_box_pack_start (GTK_BOX (perfbox), entry_perf_y, FALSE, TRUE, 0);
	*/

	// Play
	button = gtk_button_new_with_label ("Play");
	g_signal_connect(button, "clicked", G_CALLBACK(queue_show), NULL);
	image = gtk_image_new_from_stock (GTK_STOCK_MEDIA_PLAY, GTK_ICON_SIZE_MENU);
	gtk_button_set_image (GTK_BUTTON (button), image);
	gtk_box_pack_start(GTK_BOX (box1), button, FALSE, FALSE, 0);
	gtk_widget_show(button);
	gtk_widget_show(image);

	// Stop
	button = gtk_button_new_with_label ("Stop");
	g_signal_connect(button, "clicked", G_CALLBACK(stop_show), NULL);
	image = gtk_image_new_from_stock (GTK_STOCK_MEDIA_STOP, GTK_ICON_SIZE_MENU);
	gtk_button_set_image(GTK_BUTTON (button), image);
	gtk_box_pack_start(GTK_BOX (box1), button, FALSE, FALSE, 0);
	gtk_widget_show(button);
	gtk_widget_show(image);

	// Play from Start
	button = gtk_button_new_with_label ("Play Start");
	g_signal_connect(button, "clicked", G_CALLBACK(play_show_from_start), NULL);
	image = gtk_image_new_from_stock (GTK_STOCK_MEDIA_PREVIOUS, GTK_ICON_SIZE_MENU);
	gtk_button_set_image(GTK_BUTTON (button), image);
	gtk_box_pack_start(GTK_BOX (box1), button, FALSE, FALSE, 0);
	gtk_widget_show(button);
	gtk_widget_show(image);


	// First Set
	button = gtk_button_new_with_label ("First");
	g_signal_connect(button, "clicked", G_CALLBACK(set_first_gtk), NULL);
	image = gtk_image_new_from_stock (GTK_STOCK_GOTO_FIRST, GTK_ICON_SIZE_MENU);
	gtk_button_set_image(GTK_BUTTON (button), image);
	gtk_box_pack_start(GTK_BOX (box1), button, FALSE, TRUE, 0);
	gtk_widget_show(button);
	gtk_widget_show(image);

	// Previous Set
	button = gtk_button_new_with_label ("Prev"); 
	g_signal_connect(button, "clicked", G_CALLBACK(set_prev_gtk), NULL);
	image = gtk_image_new_from_stock (GTK_STOCK_MEDIA_REWIND, GTK_ICON_SIZE_MENU);
	gtk_button_set_image(GTK_BUTTON (button), image);
	gtk_box_pack_start(GTK_BOX (box1), button, TRUE, TRUE, 0);
	gtk_widget_show(button);
	gtk_widget_show(image);

	// Step
	button = gtk_button_new_with_label ("Step");
	g_signal_connect(button, "clicked", G_CALLBACK(set_next_count_gtk), NULL);
	gtk_box_pack_start(GTK_BOX (box1), button, FALSE, FALSE, 0);
	gtk_widget_show(button);

	// Next Set
	button = gtk_button_new_with_label ("Next");
	g_signal_connect(button, "clicked", G_CALLBACK(set_next_gtk), NULL);//, window);
	image = gtk_image_new_from_stock (GTK_STOCK_MEDIA_FORWARD, GTK_ICON_SIZE_MENU);
	gtk_button_set_image(GTK_BUTTON (button), image);
	gtk_box_pack_start(GTK_BOX (box1), button, TRUE, TRUE, 0);
	gtk_widget_show(button);
	gtk_widget_show(image);

	// Last Set
	button = gtk_button_new_with_label ("Last");
	image = gtk_image_new_from_stock (GTK_STOCK_GOTO_LAST, GTK_ICON_SIZE_MENU);
	gtk_button_set_image(GTK_BUTTON (button), image);
	g_signal_connect(button, "clicked", G_CALLBACK(set_last_gtk), NULL);
	gtk_box_pack_start(GTK_BOX (box1), button, FALSE, TRUE, 0);
	gtk_widget_show(button);
	gtk_widget_show(image);

	// status bar
	statusbar = gtk_statusbar_new();
	gtk_box_pack_start(GTK_BOX (box0), statusbar, FALSE, FALSE, 0);
	context_id = gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar), "statusbar");
	sbinfo = g_strdup_printf("status bar");
	gtk_statusbar_push(GTK_STATUSBAR(statusbar), GPOINTER_TO_INT(context_id), sbinfo);
	g_free(sbinfo);
	gtk_widget_show(statusbar);
	context_id = 0;

	// Show everything
	gtk_widget_show(drill);
	gtk_widget_show(box0);
	gtk_widget_show(box1);
	gtk_widget_show(setbox);
	gtk_widget_show(perfbox);
	//gtk_widget_show(box3);
	gtk_widget_show(media_box);
	gtk_widget_show(menu_box);
	gtk_widget_show_all (window);

	return 0;
}

int main (int argc, char *argv[])
{
	// specific set
	struct set_proto *currset;
	struct set_proto *prevset;
	// specific performer
	struct perf_proto *currperf;
	// coords
	struct coord_proto *coords; 
	struct coord_proto *prevcr;
	int excode;
	int i, j;		// loop vars
	float x, y;

	// Hardcode tempo in
	int tempo = 120;
	// set show as uninitialized
	pshow = 0;
	//dot_construct(&pshow);

	// Make a show with 5 performers
	pshow = 0;
	excode = show_construct(&pshow, 21);
	if (excode == -1)
	{
		printf("Allocation error\n");
		return -1;
	}
	currset = pshow->firstset;
	pshow->perfnum = 15;
	currset->counts = 0;
	set_coord_valid(currset->coords, 0, 32, 53);
	set_coord_valid(currset->coords, 1, 36, 53);
	set_coord_valid(currset->coords, 2, 40, 53);
	set_coord_valid(currset->coords, 3, 34, 49);
	set_coord_valid(currset->coords, 4, 38, 49);
	set_coord_valid(currset->coords, 5, 36, 45);

	set_coord_valid(currset->coords, 6, 32, 70);
	set_coord_valid(currset->coords, 7, 36, 70);
	set_coord_valid(currset->coords, 8, 40, 70);
	set_coord_valid(currset->coords, 9, 34, 66);
	set_coord_valid(currset->coords, 10, 38, 66);
	set_coord_valid(currset->coords, 11, 36, 62);
	newset_create(currset);
	prevset = currset;
	currset = currset->next;
	currset->counts = 8;
	for (i=0; i<12; i++)
	{
		prevcr = prevset->coords[i];
		set_coord(currset->coords[i], prevcr->x+4, prevcr->y);
	}
	newset_create(currset);
	prevset = currset;
	currset = currset->next;
	currset->counts = 8;
	for (i=0; i<12; i++)
	{
		prevcr = prevset->coords[i];
		set_coord(currset->coords[i], prevcr->x, prevcr->y-10);
	}

	// Start at first set
	setnum = 0;
	pshow->currset = pshow->firstset;
	pshow->prevset = 0;

	perf_cur = 0;

	// Start up gtk
	startTk(argc, argv);
	// Create gtk interface
	buildIfacegtk();
	//printf("%s\n", perf_buf_x);


	// Run Main Loop
	gtk_main();

	return 0;
}
	
