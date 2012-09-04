#include "drillwriter-gtk.h"
#include "dr-drill-class.h"

void zoom_amnt(double invalue, bool from_mouse)
{
	// for now, just zoom relative
	double value;
	double offsetx1, offsety1;
	double offsetx2, offsety2;
	double page_sizex, page_sizey;
	double mousex, mousey;
	double hsc_psize, vsc_psize;
	// block scroll signals for scrollbars
	// while adjusting canvas scaling, scroll amount
	g_signal_handler_block(hscroll, hscroll_id);
	g_signal_handler_block(vscroll, vscroll_id);
	if (!invalue || (fldstate.zoom_amnt * invalue) < 1)
	{
		// set zoom to 100%
		fldstate.zoom_amnt = 1;
		gtk_adjustment_set_page_size(hscroll, fldstate.width);
		gtk_adjustment_set_page_size(vscroll, fldstate.height);
		fldstate.fieldx = 0;
		fldstate.fieldy = 0;
		hsc_psize = gtk_adjustment_get_page_size(hscroll);
		vsc_psize = gtk_adjustment_get_page_size(vscroll);
		gtk_adjustment_configure(hscroll, 0.0, 0.0, fldstate.width, hsc_psize / 10, hsc_psize / 10 * 9, hsc_psize);
		gtk_adjustment_configure(vscroll, 0.0, 0.0, fldstate.height, vsc_psize / 10, vsc_psize / 10 * 9, vsc_psize);
		canvas_move(drill, 0, 0);
		g_signal_handler_unblock(hscroll, hscroll_id);
		g_signal_handler_unblock(vscroll, vscroll_id);
		return;
	}
	else
	{
		// zoom and scale the canvas
		value = invalue;
		// get the old page size
		offsetx1 = gtk_adjustment_get_upper(hscroll) / fldstate.zoom_amnt;
		offsety1 = gtk_adjustment_get_upper(vscroll) / fldstate.zoom_amnt;
		// new zoom factor
		fldstate.zoom_amnt *= value;
		// new page size
		page_sizex = gtk_adjustment_get_upper(hscroll) / fldstate.zoom_amnt;
		page_sizey = gtk_adjustment_get_upper(vscroll) / fldstate.zoom_amnt;
		// change scrollbars
		gtk_adjustment_configure(hscroll, fldstate.fieldx, 0.0, fldstate.width, page_sizex / 10, page_sizex / 10 * 9, page_sizex);
		gtk_adjustment_configure(vscroll, fldstate.fieldy, 0.0, fldstate.height, page_sizey / 10, page_sizey / 10 * 9, page_sizey);
		// calculate base offset after zoom
		offsetx2 = (offsetx1 - page_sizex) / 2;
		offsety2 = (offsety1 - page_sizey) / 2;
		if (from_mouse)
		{
			// get mouse position
			// use it in canvas offset
			mousex = fldstate.mousex;
			mousey = fldstate.mousey;
			// translate mouse position to pixel values
			field_to_pixel(&mousex, &mousey);
			// calculate a mouse offset factor
			mousex = 2*(mousex - fldstate.fieldx) / offsetx1;
			mousey = 2*(mousey - fldstate.fieldy) / offsety1;
		}
		else
		{
			// don't use mouse for zoom offset
			mousex = 1;
			mousey = 1;
		}
		// offset the canvas after zooming based on mouse position
		canvas_move(drill, offsetx2*mousex, offsety2*mousey);
		// release scrollbars
		g_signal_handler_unblock(hscroll, hscroll_id);
		g_signal_handler_unblock(vscroll, vscroll_id);
		return;
	}
	return;
}



void zoom_fit(GtkWidget *widget)
{
	zoom_amnt(0.0, false);
	return;
}


void zoom_in(GtkWidget *widget, bool from_mouse)
{
	// zoom canvas in
	zoom_amnt(1.1, from_mouse);
}



void zoom_out(GtkWidget *widget, bool from_mouse)
{
	// zoom canvas out
	zoom_amnt(0.9, from_mouse);
}



void zoom_standard(GtkWidget *widget)
{
	// zoom canvas to 100%
	zoom_amnt(0, false);
}

