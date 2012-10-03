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
		coords_retrieve_midset(pstate.setnum, i, &workx, &worky);
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
	int index;
	form_child_t *form2;
	select_t *select;
	//double x1, y1;
	form_child_t *form;
	form = pshow->sets->currset->forms;
	x = event->x;
	y = event->y;
	pixel_to_field(&x, &y);
	if (event->button == 1)
	{
		switch(mouse_currentMode)
		{
			case SELECTONE:
				// move a performer
				mouse_clickx = x - mouse_clickx;
				mouse_clicky = y - mouse_clicky;
				//select = select_find_form_with_endpoint(pstate.select, fldstate.mouse_clickx, fldstate.mouse_clicky);
				select = form_find_selected_with_endpoint(pstate.select, fldstate.mouse_clickx, fldstate.mouse_clicky);
				if ((event->state & ~GDK_SHIFT_MASK)== GDK_BUTTON_PRESS_MASK)
				{
					//form = select ? select->form : NULL;
					form = select ? select_get_form(select) : NULL;
					// regular click
					// move dots
					if(!mouse_discarded && form_endpoint_contains_coords(form, fldstate.mouse_clickx, fldstate.mouse_clicky))
					{
						while (form && form_endpoint_contains_coords(form, fldstate.mouse_clickx, fldstate.mouse_clicky))
						{
							if ((event->state & GDK_SHIFT_MASK) == GDK_SHIFT_MASK)
								form_move_endpoint_grid(form, fldstate.mouse_clickx, fldstate.mouse_clicky, x, y);
							else
								form_move_endpoint(form, fldstate.mouse_clickx, fldstate.mouse_clicky, x, y);
							//if ((select = select_find_form_with_endpoint_hole(select->next, x, y)) != NULL)
							//if ((select = select_find_form_with_endpoint_hole(select_get_next(select), x, y)) != NULL)
							if ((select = form_find_selected_with_endpoint_hole(select_get_next(select), x, y)) != NULL)
							{
								//form2 = select->form;
								form2 = select_get_form(select);
								index = form_find_index_with_coords(form, x, y);
								form_add_index_to_hole_with_coords(form2, index, x, y);
							}
							if (select)
							{
								//select = select_find_form_with_endpoint(select->next, fldstate.mouse_clickx, fldstate.mouse_clicky);
								//select = select_find_form_with_endpoint(select_get_next(select), fldstate.mouse_clickx, fldstate.mouse_clicky);
								select = form_find_selected_with_endpoint(select_get_next(select), fldstate.mouse_clickx, fldstate.mouse_clicky);
							}
							else
								select = NULL;
							//form = select ? select->form : NULL;
							form = select ? select_get_form(select) : NULL;
						}
						select_update_center(pstate.select);
						dr_canvas_refresh(drill);
					}
					else if ((mouse_clickx != 0 || mouse_clicky != 0) && !mouse_discarded)
					{
						if ((event->state & GDK_SHIFT_MASK) == GDK_SHIFT_MASK)
							coords_movexy_grid(mouse_clickx, mouse_clicky);
						else
							coords_movexy(mouse_clickx, mouse_clicky);
						select_update_center(pstate.select);
						dr_canvas_refresh(drill);
					}
				}
				break;
			case ADDFORM:
				// add a form
				//form = pstate.select->form;
				form = select_get_form(pstate.select);
				form_set_endpoint(form, 0, 0, x, y);
				form_set_endpoint(form, 0, 0, fldstate.mouse_clickx, fldstate.mouse_clicky);
				form_add_to_set(form);
				form_update_line(form);
				mouse_currentMode = SELECTONE;
				select_update_center(pstate.select);
				dr_canvas_refresh(drill);
				break;
				
		}
	}
	else if (event->button == 3)
	{
		// right-click
		form = form_find_with_coords(pshow->sets->currset->forms, x, y);
		index = form_find_index_with_coords(form, x, y);
		printf("%i\n", index);
		if (index != -1)
			form_unmanage_dot(form, index);
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
	select_t *select = NULL;
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
				//select = select_get_in_area(mouse_clickx, mouse_clicky);
				select = field_get_in_area(mouse_clickx, mouse_clicky);
				if (!select)
				{
					select_dots_discard();
					mouse_discarded = 1;
					break;
				}
				if (event->state == GDK_CONTROL_MASK)
				{
					// ctrl-click
					/*
					if (!select->form)
						select_dots_add_index(select->index);
						*/
					if (!select_get_form(select))
						select_dots_add_index(select_get_index(select));
				}
				else if ((event->state & ~GDK_SHIFT_MASK)== 0)
				{
					// regular click
					//if (select->form && !form_is_selected(select->form, pstate.select))
					if (select_get_form(select) && !form_is_selected(select_get_form(select), pstate.select))
					{
						// select form with ability to scale form
						select_dots_discard();
						mouse_discarded = 1;
						//pstate.select = select_add_form(pstate.select, select->form, false);
						pstate.select = select_add_form(pstate.select, select_get_form(select), false);
						select_update_center(pstate.select);
					}
					//else if (!select->form && isSelected(select->index) != 1) 
					else if (!select_get_form(select) && isSelected(select_get_index(select)) != 1) 
					{
						// dot is not selected
						//index = select->index;
						index = select_get_index(select);
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
				}
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
			case ADDFORM:
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
	if ((event->state & GDK_SHIFT_MASK) == GDK_SHIFT_MASK)
	{
		coordx = round(coordx);
		coordy = round(coordy);
	}
	if (fldstate.mouse_clicked == 0x1 && mouse_currentMode == SELECTONE)
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
		if (event->state == (GDK_BUTTON_PRESS_MASK | GDK_CONTROL_MASK))
		{
			select_push_all(&pstate.select, &select_added, true);
			select_add_multiple(&pstate.select, &select_omitted, true);
		}
		else if (event->state == GDK_BUTTON_PRESS_MASK)
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

