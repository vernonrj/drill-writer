#include "drillwriter-gtk.h"



int mouse_click_find_close_dot(GtkWidget *widget, GdkEventButton *event)
{
	// find a dot close to mouseclick
	double dist_threshold = 9;
	double distance;
	double workx, worky;
	double coordx, coordy;
	int perfnum;
	int i;
	int found_dot = 0;

	coordx = event->x;
	coordy = event->y;
	pixel_to_field(&coordx, &coordy);

	//printf("button 1 pressed at %g %g %g\n", coordx, coordy, yo2);
	perfnum = pshow->perfnum;
	//for (i=0; i<perfnum; i++)
	perf_cur = 0;
	for (i=0; i<perfnum; i++)
	{
		coords_retrieve_midset(pshow->sets->currset, i, &workx, &worky);
		workx = workx - coordx;
		worky = worky - coordy;
		distance = pow(workx, 2) + pow(worky, 2);
		/*
		if (i == 0)
		{
			printf("(x,y) at %.2f, %.2f (valid = %i)\n", workx, worky, pshow->perfs[i]->valid);
		}
		*/
		if (distance < dist_threshold && pshow->perfs[i]->valid)
		{
			// Found a closer dot
			perf_cur = i;
			dist_threshold = distance;
			found_dot = 1;
		}

	}
	if (found_dot == 1)
	{
		return perf_cur;
		/*
		i = inSelected(perf_cur);
		if (state != 4) 
		{
			if (!i)
				select_discard();
		}
		if (state != 0 || !i)
			select_add(perf_cur);
		*/
	}
	else
		return -1;
	return 0;
}



gboolean mouse_unclicked(GtkWidget *widget, GdkEventButton *event)
{
	// handle un-click events on canvas
	double x, y;
	//double x1, y1;
	int index;
	group_t *group;
	if (event->button == 1)
	{
		switch(mouse_currentMode)
		{
			case SELECTONE:
				// move a performer
				x = event->x;
				y = event->y;
				pixel_to_field(&x, &y);
				mouse_clickx = x - mouse_clickx;
				mouse_clicky = y - mouse_clicky;
				group = dr_check_form_selected(widget, event);
				//printf("event->state == %i\n", event->state);
				if (event->state == GDK_BUTTON_PRESS_MASK)
				{
					// regular click
					//index = mouse_click_find_close_dot(widget, event);
					if ((mouse_clickx != 0 || mouse_clicky != 0) && !mouse_discarded)
					{
						coords_movexy(mouse_clickx, mouse_clicky);
						mouse_discarded = 0;
						//dr_canvas_refresh(drill);
						dr_canvas_refresh(drill);
					}
					/*
					else if (mouse_discarded && !isSelected(index))
					{
						if (index != -1)
							select_dots_add_index(index);
					}
					*/
				}
				/*
				else if (event->state == (GDK_BUTTON_PRESS_MASK | GDK_CONTROL_MASK))
				{
					// ctrl-click
					index = mouse_click_find_close_dot(widget, event);
					if (group)
						pstate.select = select_add_group(pstate.select, group, true);
					else if (index != -1)
						select_dots_add_index(index);
				}
				*/


				break;
		}
	}
	fldstate.mouse_clicked = 0;
	dr_canvas_refresh(drill);
	return TRUE;
}



gboolean mouse_clicked(GtkWidget *widget, GdkEventButton *event)
{
	// Handle click event on canvas
	
	int index;
	double coordx, coordy;
	group_t *group;
	// Length from click location to nearest dot


	mouse_discarded = 0;
	if (event->button == 8)
	{
		// back button
		if (event->state == GDK_SHIFT_MASK)
			set_prev_count();
		else if (event->state == GDK_CONTROL_MASK)
			set_first();
		else
			set_prev();
		fldstate.mouse_clicked = 0x0;
	}
	else if (event->button == 9)
	{
		// forward button
		if (event->state == GDK_SHIFT_MASK)
			set_next_count();
		else if (event->state == GDK_CONTROL_MASK)
			set_last();
		else
			set_next();
		fldstate.mouse_clicked = 0x0;
	}
	else if (event->button == 1)
	{
		fldstate.mouse_clicked = 0x1;
		mouse_clickx = event->x;
		mouse_clicky = event->y;
		pixel_to_field(&mouse_clickx, &mouse_clicky);
		fldstate.mouse_clickx = mouse_clickx;
		fldstate.mouse_clicky = mouse_clicky;
		switch(mouse_currentMode)
		{
			case SELECTONE:
				// select 1 performer
				group = dr_check_form_selected(widget, event);
				index = mouse_click_find_close_dot(widget, event);
				if (event->state == GDK_CONTROL_MASK)// && index != -1)
				{
					// ctrl-click
					if (!group && index != -1)
						select_dots_add_index(index);
				}
				else if (event->state == GDK_SHIFT_MASK)// && index != -1)
				{
					// shift-click
					if (!group && !isSelected(index) && index != -1)
						select_dots_add_index(index);
				}
				else
				{
					// regular click
					if (!group && !isSelected(index))
					{
						// dot is not selected
						select_dots_discard();
						mouse_discarded = 1;
						if (index != -1)
						{
							select_dots_add_index(index);
						}
					}
					else
					{
						// hold click; dots being moved
						fldstate.mouse_clicked |= 0x2;
					}
					/*
					else if (group && !group_is_selected(group, pstate.select))
					{
						if (isSelected(index))
							fldstate.mouse_clicked |= 0x2;
						else
							mouse_discarded = 1;
						select_dots_discard();
						pstate.select = select_add_group(pstate.select, group, false);
					}
					*/
				}
				//printf("event = %i\n", event->state);
				//dr_canvas_refresh(drill);
				dr_canvas_refresh(drill);
				break;
			case SELECTDRAG:
				// Select (by dragging) performers
				//dr_canvas_refresh(drill);
				dr_canvas_refresh(drill);
				break;
			case ADDPERF:
				// Add performers by clicking on canvas
				coordx = event->x;
				coordy = event->y;
				pixel_to_field(&coordx, &coordy);
				/*
				// Adjust for various canvas offsets
				coordx = round((coordx-fldstate.xo2)/fldstate.canv_step);
				//coordy = (coordy-yo2-25)/step;
				coordy = round((coordy-fldstate.yo2)/fldstate.canv_step);
				*/
				index = perf_add();
				coords_set_coord(pshow, index, coordx, coordy);
				//dr_canvas_refresh(drill);
				dr_canvas_refresh(drill);
				//mouse_currentMode = SELECTONE;
				break;
			case MVPERF:
				// Move performers by clicking on canvas?
				//dr_canvas_refresh(drill);
				dr_canvas_refresh(drill);
				break;
		}
	}
	else
		fldstate.mouse_clicked = 0;
	return TRUE;
}



gboolean mouse_xy_movement(GtkWidget *widget, GdkEventMotion *event)
{
	// caught a mouse movement event
	double coordx, coordy;
	gchar *buffer;
	int excode;
	select_t *new_select;
	new_select = NULL;
	select_t *select_added;
	select_added = NULL;
	select_t *select_omitted = NULL;
	//select_t *last;

	coordx = event->x;
	coordy = event->y;
	pixel_to_field(&coordx, &coordy);
	if (fldstate.mouse_clicked == 0x1)
	{
		// click drag
		// get what's currently in selection rectangle
		new_select = select_add_in_rectangle(new_select, fldstate.mouse_clickx, fldstate.mouse_clicky,
				coordx, coordy, false);
		// then find what's been added, what's gone
		select_added = select_drop_multiple(new_select, fldstate.mouse_selection);
		select_omitted = select_drop_multiple(fldstate.mouse_selection, new_select);

		// Store new set
		fldstate.mouse_selection = select_discard(fldstate.mouse_selection);
		select_push_all(&fldstate.mouse_selection, &new_select, false);
		select_update_center(fldstate.mouse_selection);

		// add new dots
		if (event->state == 256 + GDK_CONTROL_MASK)
		{
			select_push_all(&pstate.select, &select_added, true);
			select_add_multiple(&pstate.select, &select_omitted, true);
		}
		else if (event->state == 256 || event->state == (256 + GDK_SHIFT_MASK))
		{
			// normal or shift-clicked
			select_push_all(&pstate.select, &select_added, false);
			// drop ommitted dots
			pstate.select = select_drop_multiple(pstate.select, select_omitted);
		}
	}
	else
		fldstate.mouse_selection = NULL;
	fldstate.mousex = coordx;
	fldstate.mousey = coordy;
	// store mouse event
	excode = fieldrel_convert_xy_to_relation(&coordx, &coordy, &buffer);
	if (excode == -1)
		return FALSE;


	gtk_statusbar_pop(GTK_STATUSBAR(statusbar), GPOINTER_TO_INT(context_id));
	gtk_statusbar_push(GTK_STATUSBAR(statusbar), GPOINTER_TO_INT(context_id), buffer);
	g_free(buffer);
	dr_canvas_refresh(drill);

	return TRUE;
}


gboolean mouse_handle_scroll_event(GtkWidget *widget, GdkEventScroll *event)
{
	// handle zoom events
	// propagate everything except control modifier
	if (event->direction == GDK_SCROLL_UP)
	{
		if (event->state == 0)
		{
			// no modifiers
			// move up
			//canvas_move(widget, 0.0, -1*vscroll->step_increment);
			canvas_move(widget, 0.0, -1*gtk_adjustment_get_step_increment(vscroll));
		}
		else if (event->state == GDK_SHIFT_MASK)
		{
			// shift modifier
			// move left
			canvas_move(widget, -1*gtk_adjustment_get_step_increment(hscroll), 0.0);
		}
		else if (event->state == GDK_CONTROL_MASK)
		{
			// ctrl modifier
			// zoom in
			zoom_in(widget, true);
		}
	}
	else if (event->direction == GDK_SCROLL_DOWN)
	{
		if (event->state == 0)
		{
			// move down
			canvas_move(widget, 0.0, gtk_adjustment_get_step_increment(vscroll));
		}
		else if (event->state == GDK_SHIFT_MASK)
		{
			// shift modifier
			// move right
			canvas_move(widget, gtk_adjustment_get_step_increment(hscroll), 0.0);
		}
		else if (event->state == GDK_CONTROL_MASK)
		{
			// ctrl modifier
			// zoom out
			zoom_out(widget, true);
		}
	}
	else if (event->direction == GDK_SCROLL_LEFT)
	{
		canvas_move(widget, -1*gtk_adjustment_get_step_increment(hscroll), 0.0);
	}
	else if (event->direction == GDK_SCROLL_RIGHT)
	{
		canvas_move(widget, gtk_adjustment_get_step_increment(hscroll), 0.0);
	}
	return TRUE;
}

