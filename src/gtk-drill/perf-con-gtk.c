#include "drillwriter-gtk.h"
#include "dr-sidebar.h"
#include "../dr_forms.h"
#include "../dr_select.h"
void view_stepsize_gtk(GtkWidget *widget)
{
	// view the stepsize of selected dots
	double stepsize;
	stepsize = perf_average_stepsize_selected(pshow);
	printf("stepsize = %.2f:5\n", stepsize);
	return;
}

/*
void goto_perf (GtkWidget *widget)
{
	const gchar *entry_buffer;
	int perf_buffer;
	if (!pstate.playing)
	{
		entry_buffer = gtk_entry_get_text (GTK_ENTRY (entry_perf));
		perf_buffer = atoi(entry_buffer);
		if (perf_buffer<pshow->perfnum)
			perf_cur=perf_buffer;
		dr_canvas_refresh(drill);
	}
}
*/

int add_perf_gtk(GtkWidget *widget)
{
	int excode = 0;
	mouse_currentMode = ADDPERF;
	//gtk_button_set_label(GTK_BUTTON(menuButton), "Add");
	//excode = add_perf();
	//dr_canvas_refresh(drill);
	if (excode != -1)
		return 0;
	return excode;
}


void revert_perf_gtk(GtkWidget *widget)
{
	// revert selected performers
	if (!pstate.playing)
	{
		perf_revert_selected(pshow);
		//dr_canvas_refresh(drill);
		dr_canvas_refresh(drill);
	}
	return;
}

void delete_perf_gtk(GtkWidget *widget)
{
	// Delete selected performers
	if (!pstate.playing)
	{
		perf_delete_selected();
		//dr_canvas_refresh(drill);
		dr_canvas_refresh(drill);
	}
}



/*
void xperf_change (GtkWidget *widget)
{
	const gchar *entry_buffer;
	double perf_buffer;
	struct coord_proto *coords = pshow->sets->currset->coords[perf_cur];
	if (!pstate.playing)
	{
		entry_buffer = gtk_entry_get_text (GTK_ENTRY (entry_perf_x));
		perf_buffer = atof(entry_buffer);
		if (perf_buffer>0 && perf_buffer<=160)
		{
			coords_set_coord(pshow, perf_cur, perf_buffer, coords->y);
			//perf[setnum][perf_cur][0] = perf_buffer;
		}
		dr_canvas_refresh(drill);
	}
}

void yperf_change (GtkWidget *widget)
{
	const gchar *entry_buffer;
	double perf_buffer;
	struct coord_proto *coords = pshow->sets->currset->coords[perf_cur];
	if (!pstate.playing)
	{
		entry_buffer = gtk_entry_get_text (GTK_ENTRY (entry_perf_y));
		perf_buffer = atoi(entry_buffer);
		if (perf_buffer>0 && perf_buffer<=85)
		{
			coords_set_coord(pshow, perf_cur, coords->x, perf_buffer);
			//perf[setnum][perf_cur][1] = perf_buffer;
		}
		dr_canvas_refresh(drill);
	}
}
*/


/*
void next_perf(GtkWidget *widget)
{
	// Go to next sequential dot
	if (perf_cur < pshow->perfnum-1)
	{
		perf_cur++;
		dr_canvas_refresh(drill);
		//dr_canvas_refresh(drill);
	}
}
*/

/*
void prev_perf(GtkWidget *widget)
{
	// Go to previous sequential dot
	if (perf_cur > 0)
	{
		perf_cur--;
		dr_canvas_refresh(drill);
		//dr_canvas_refresh(drill);
	}
}
*/

int select_all_gtk (GtkWidget *widget)
{
	select_all_dots();
	dr_canvas_refresh(drill);
	//dr_canvas_refresh(drill);
	return 0;
}


int select_form_gtk(GtkWidget *widget, form_child_t *form)
{
	//select_add_form(pstate.select, form->parent->index);
	select_add_form(pstate.select, form_child_get_index(form));
	dr_canvas_refresh(drill);
	return 0;
}


int select_group_gtk(GtkWidget *widget, group_t *group)
{
	//pstate.select = select_add_group(pstate.select, group);
	select_t *select = group_retrieve_dots(group);
	select_add_multiple_dots(pstate.select, select);
	//free(select);
	select_destroy(select);
	dr_canvas_refresh(drill);
	return 0;
}


int add_group_gtk (GtkWidget *widget)
{
	// add selection to group
	//int excode = add_group();
	group_t *last = NULL;
	group_t *curr;
	last = group_add_selects(last, pstate.select);
	//pshow->topgroups = group_add_selects(pshow->topgroups, pstate.select);
	curr = pshow->topgroups;
	/*
	while (curr && curr->next)
		curr = curr->next;
	if (!curr)
		pshow->topgroups = last;
	else
		curr->next = last;
		*/
	if (!curr)
		pshow->topgroups = last;
	else
		group_add_to_groups(last, curr);
	dr_sidebar_update((DrSidebar*)sidebar);
	return 0;
}


int select_mode_gtk (GtkWidget *widget)
{
	// go back to normal selectmode
	mouse_currentMode = SELECTONE;
	//gtk_button_set_label(GTK_BUTTON(menuButton), "Select");
	//dr_canvas_refresh(drill);
	dr_canvas_refresh(drill);
	return 0;
}



int isSelected(int dot)
{
	// check to see if a dot is in selected dots
	struct select_proto *select;
	select_t *form_select = NULL;
	//group_t *group = NULL;
	int index;

	select = pstate.select;
	if (select_check_dot(select, dot))
		return 1;
	select_head(select);
	while ((index = select_get_form_advance(select)) != -1)
	{
		form_select = form_get_contained_dots(form_container_get_form_child(pshow->topforms, index));
		if (select_check_dot(form_select, dot))
			return 1;
	}
	
	/*
	while (select != NULL)
	{
		if (!form_select && select_has_form(select))
			form_select = form_get_contained_dots(select_get_form(select));
		if (form_select)
		{
			//index_selected = form_select->index;
			index_selected = select_get_dot(form_select);
		}
		else
		{
			//index_selected = select->index;
			index_selected = select_get_dot(select);
		}

		if (index == -1)
		{
			isin = -1;
			break;
		}
		else if (index == index_selected)
		{
			isin = 1;
			break;
		}
		if (form_select)
		{
			//form_select = form_select->next;
			form_select = select_get_next(form_select);
		}
		if (!form_select)
		{
			//select = select->next;
			select = select_get_next(select);
		}
	}
	*/
	return 0;
}





form_child_t *dr_check_form_endpoints(GtkWidget *widget, GdkEventButton *event)
{
	double coordx, coordy;
	form_child_t *form = pshow->sets->currset->forms;
	coordx = event->x;
	coordy = event->y;
	pixel_to_field(&coordx, &coordy);
	form = check_endpoints(coordx, coordy, form);
	return form;
}


form_child_t *dr_check_form(GtkWidget *widget, GdkEventButton *event)
{
	double coordx, coordy;
	form_coord_attr_t form_attr;
	form_child_t *form = pshow->sets->currset->forms;

	coordx = event->x;
	coordy = event->y;
	pixel_to_field(&coordx, &coordy);
	while (form)
	{
		if (form_contains_coords(form, &form_attr, coordx, coordy))
			return form;
		//form = form->next;
		form = form_child_get_next(form);
	}
	return NULL;
}



form_child_t *check_endpoints(double coordx, double coordy, form_child_t *form)
{
	int dot_alloc;
	int formindex;
	form_coord_attr_t form_attr;
	while (form)
	{
		//if (form_endpoint_contains_coords(form, coordx, coordy))
		if (form_contains_coords(form, &form_attr, coordx, coordy))
		{
			formindex = form_attr.formindex;
			dot_alloc = form_attr.dot_alloc;
			if (formindex == 0 || formindex == (dot_alloc - 1))
				return form;
		}
		//form = form->next;
		form = form_child_get_next(form);
	}
	return form;
}





