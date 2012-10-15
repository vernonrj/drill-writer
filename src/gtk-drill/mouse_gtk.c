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
	select_t *select = pstate.select;
	select_t *form_select = select_create();
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
				select_add_multiple_forms(form_select, select);
				// move a performer
				mouse_clickx = x - mouse_clickx;
				mouse_clicky = y - mouse_clicky;
				//select = select_find_form_with_endpoint(pstate.select, fldstate.mouse_clickx, fldstate.mouse_clicky);
				form = form_find_selected_with_endpoint(form_select, fldstate.mouse_clickx, fldstate.mouse_clicky);
				if (form)
					select_remove_form(form_select, form->parent->index);
				if ((event->state & ~GDK_SHIFT_MASK)== GDK_BUTTON_PRESS_MASK)
				{
					//form = select ? select->form : NULL;
					//form = select ? select_get_form(select) : NULL;
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
							if ((form2 = form_find_selected_with_endpoint_hole(form_select, x, y)) != NULL)
							{
								select_remove_form(form_select, form2->parent->index);
								//form2 = select->form;
								//form2 = select_get_form(select);
								index = form_find_index_with_coords(form, x, y);
								form_add_index_to_hole_with_coords(form2, index, x, y);
							}
							//if (select)
							if ((form = form_find_selected_with_endpoint(form_select, fldstate.mouse_clickx, fldstate.mouse_clicky)) != NULL)
							{
								//select = select_find_form_with_endpoint(select->next, fldstate.mouse_clickx, fldstate.mouse_clicky);
								//select = select_find_form_with_endpoint(select_get_next(select), fldstate.mouse_clickx, fldstate.mouse_clicky);
								select_remove_form(form_select, form->parent->index);
							}
							//form = select ? select->form : NULL;
							//form = select ? select_get_form(select) : NULL;
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
				form = form_container_get_form_child(pshow->topforms, select_get_form(pstate.select));
				form_set_endpoint(form, 0, 0, x, y);
				form_set_endpoint(form, 0, 0, fldstate.mouse_clickx, fldstate.mouse_clicky);
				form_add_to_current_set(form);
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
	select_destroy(form_select);
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
				select = field_get_in_area(mouse_clickx, mouse_clicky);
				if (event->state == GDK_CONTROL_MASK)
				{
					// ctrl-click
					select_toggle_multiple(pstate.select, select);
				}
				else if ((event->state & ~GDK_SHIFT_MASK)== 0)
				{
					// regular click
					if (select_empty(select))
					{
						//select_dots_discard();
						select_clear(pstate.select);
						mouse_discarded = 1;
						break;
					}
					index = select_get_form(select);
					if (!select_form_empty(select) && !select_check_form(pstate.select, index))
					{
						// select form with ability to scale form
						select_clear(pstate.select);
						mouse_discarded = 1;
						select_add_form(pstate.select, select_get_form(select));
						select_update_center(pstate.select);
					}
					else if (select_form_empty(select) && isSelected(select_get_dot(select)) != 1) 
					{
						// dot is not selected
						select_clear(pstate.select);
						mouse_discarded = 1;
						if ((index = select_get_dot(select))!= -1)
						{
							select_add_dot(pstate.select, index);
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
				index = perf_add();
				coords_set_coord(pshow, index, coordx, coordy);
				dr_canvas_refresh(drill);
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
	select_t *new_select = select_create();
	select_t *select_added = select_create();
	select_t *select_omitted = select_create();

	if (!fldstate.mouse_selection)
		fldstate.mouse_selection = select_create();
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
		new_select = field_select_in_rectangle(new_select, fldstate.mouse_clickx, fldstate.mouse_clicky,
				coordx, coordy, false);
		// then find what's been added, what's gone
		select_add_multiple(select_added, new_select);
		select_add_multiple(select_omitted, fldstate.mouse_selection);
		select_remove_multiple(select_added, fldstate.mouse_selection);
		select_remove_multiple(select_omitted, new_select);

		// Store new set
		select_clear(fldstate.mouse_selection);
		select_add_multiple(fldstate.mouse_selection, new_select);
		select_update_center(fldstate.mouse_selection);

		// add new dots
		if (event->state == (GDK_BUTTON_PRESS_MASK | GDK_CONTROL_MASK))
		{
			//select_push_all(&pstate.select, &select_added, true);
			//select_add_multiple(&pstate.select, &select_omitted, true);
			select_toggle_multiple(pstate.select, select_added);
			select_toggle_multiple(pstate.select, select_omitted);
		}
		else if (event->state == GDK_BUTTON_PRESS_MASK)
		{
			// normal or shift-clicked
			//select_push_all(&pstate.select, &select_added, false);
			//pstate.select = select_drop_multiple(pstate.select, select_omitted);
			select_add_multiple(pstate.select, select_added);
			// drop ommitted dots
			select_remove_multiple(pstate.select, select_omitted);
		}
	}
	else
		fldstate.mouse_selection = NULL;
	fldstate.mousex = coordx;
	fldstate.mousey = coordy;
	// store mouse event
	excode = fieldrel_convert_xy_to_relation(&coordx, &coordy, &buffer);
	select_added = select_destroy(select_added);
	select_omitted = select_destroy(select_omitted);
	new_select = select_destroy(new_select);
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

