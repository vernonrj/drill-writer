#include "drillwriter-gtk.h"
#include "../forms/dr_forms.h"
#include "../selection/selectcon.h"
//#include "dr-sidebar-groups.h"



static void dr_group_cell_class_init(DrGroupCellClass *klass);
static void dr_group_cell_init(DrGroupCell *groupcell);

struct _DrGroupCellPrivate {
	gpointer garbage;
	GtkWidget *entry_name;
	//GtkWidget *button_add;
	GtkWidget *button_del;
	GtkWidget *button_name;
	GtkWidget *context_menu;
	GtkWidget *expander;
	GtkWidget *expander_box;
	group_t *group;
	form_child_t *form;
	int type;
	bool this_set;
	gulong button_del_id;
	gulong button_add_id;
	DrGroupCell *next;
};

G_DEFINE_TYPE (DrGroupCell, dr_group_cell, GTK_TYPE_HBOX)

#define DR_GROUP_CELL_GET_PRIVATE(obj) \
		(G_TYPE_INSTANCE_GET_PRIVATE ((obj), DR_GROUP_CELL_TYPE, DrGroupCellPrivate))

static void dr_group_cell_class_init(DrGroupCellClass *class)
{
	GObjectClass *g_object_class = G_OBJECT_CLASS(class);
	g_type_class_add_private(g_object_class, sizeof(DrGroupCellPrivate));
}
static void dr_group_cell_add_form(GtkWidget *widget, gpointer *data)
{
	DrGroupCell *groupcell = (DrGroupCell*)data;
	g_return_if_fail(IS_GROUP_CELL(groupcell));
	form_child_t *form = NULL;
	form_parent_t *parent_form = NULL;
	form = form_build_line(parent_form, groupcell->priv->group->selects);
	form->name = (char*)malloc(10*sizeof(char));
	snprintf(form->name, 10, "New Form");
	//form_add_to_current_set(form);
	select_clear(pstate.select);
	pshow->topforms = form_parent_add_form(pshow->topforms, form, pstate.setnum);
	select_add_form(pstate.select, form->parent->index);
	select_head(pstate.select);
	mouse_currentMode = ADDFORM;
	return;
}


static void group_cell_select_group(GtkWidget *widget, gpointer *data)
{
	DrGroupCell *groupcell = (DrGroupCell*)data;
	g_return_if_fail(IS_GROUP_CELL(groupcell));
	if (dr_group_cell_get_container_type((GtkWidget*)data) == GROUP_CELL_TYPE_GROUP)
		select_group_gtk(widget, groupcell->priv->group);
	else
		select_form_gtk(widget, groupcell->priv->form);
	return;
}


static void select_add_to_group_cell(GtkWidget *widget, gpointer *data)
{
	DrGroupCell *groupcell = (DrGroupCell*)data;
	g_return_if_fail(IS_GROUP_CELL(groupcell));
	select_add_multiple(groupcell->priv->group->selects, pstate.select);
	return;
}

static void select_remove_from_group_cell(GtkWidget *widget, gpointer *data)
{
	DrGroupCell *groupcell = (DrGroupCell*)data;
	g_return_if_fail(IS_GROUP_CELL(groupcell));
	groupcell->priv->group = group_remove_selects(groupcell->priv->group, pstate.select);
}



static void dr_group_cell_remove_cell(GtkWidget *widget, gpointer *data)
{
	DrGroupCell *groupcell = (DrGroupCell*)data;
	g_return_if_fail(IS_GROUP_CELL(groupcell));
	if (groupcell->priv->form)
	{
		// form
		pshow->sets->currset->forms = form_remove_from(groupcell->priv->form, pshow->sets->currset->forms);
		form_parent_remove_set(pshow->topforms, form_parent_find_with_form(pshow->topforms, groupcell->priv->form), pstate.setnum);
	}
	else if (groupcell->priv->group && groupcell->priv->group->local)
	{
		// local group
		pshow->sets->currset->groups = group_remove_from(groupcell->priv->group, pshow->sets->currset->groups);
	}
	else if (groupcell->priv->group)
	{
		// global group
		pshow->topgroups = group_remove_from(groupcell->priv->group, pshow->topgroups);
	}
	groupcell->priv->group = NULL;
	groupcell->priv->form = NULL;
	dr_sidebar_update((DrSidebar*)sidebar);
	dr_canvas_refresh(drill);
}





gint group_cell_clicked(GtkWidget *widget, GdkEventButton *event, gpointer *data)
{
	DrGroupCell *groupcell = (DrGroupCell*)data;
	g_return_val_if_fail(IS_GROUP_CELL(groupcell), -1);

	if (event->button == 3)
	{
		gtk_menu_popup(GTK_MENU(groupcell->priv->context_menu), NULL, NULL, NULL, NULL, event->button, event->time);
		return TRUE;
	}
	return FALSE;
}

gint group_cell_toggle_group_scope(GtkWidget *widget, gpointer *data)
{
	DrGroupCell *groupcell = (DrGroupCell*)data;
	g_return_val_if_fail(IS_GROUP_CELL(groupcell), -1);
	if (groupcell->priv->group->local)
	{
		// switch local group to global
		pshow->sets->currset->groups = group_pop_from(groupcell->priv->group, pshow->sets->currset->groups);
		group_add_global(groupcell->priv->group);
		groupcell->priv->group->local = false;
	}
	else if (!groupcell->priv->form)
	{
		// switch global group to local
		pshow->topgroups = group_pop_from(groupcell->priv->group, pshow->topgroups);
		group_add_to_set(groupcell->priv->group);
		groupcell->priv->group->local = true;
	}
	groupcell->priv->group = NULL;
	dr_sidebar_update((DrSidebar*)sidebar);
	return TRUE;
}


gint group_cell_toggle_to_entry(GtkWidget *widget, gpointer *data)
{
	DrGroupCell *groupcell = (DrGroupCell*)data;
	g_return_val_if_fail(IS_GROUP_CELL(groupcell), -1);
	gtk_widget_hide(groupcell->priv->button_name);
	gtk_widget_set_sensitive(groupcell->priv->button_name, FALSE);
	gtk_widget_show(groupcell->priv->entry_name);
	//text_length = gtk_entry_get_text_length(GTK_ENTRY(groupcell->priv->entry_name))-1;
	//if (text_length > 0)
	{
		gtk_editable_select_region(GTK_EDITABLE(groupcell->priv->entry_name),
			0, -1);
	}
	gtk_editable_set_position(GTK_EDITABLE(groupcell->priv->entry_name), 0);

	gtk_widget_grab_focus(groupcell->priv->entry_name);
	return TRUE;
}

gint group_cell_set_name(GtkWidget *widget, gpointer *data)
{
	const gchar *new_name;
	char *name;
	int size;
	DrGroupCell *groupcell = (DrGroupCell*)data;
	g_return_val_if_fail(IS_GROUP_CELL(groupcell), -1);
	new_name = gtk_entry_get_text(GTK_ENTRY(widget));
	gtk_button_set_label(GTK_BUTTON(groupcell->priv->button_name), new_name);
	gtk_widget_hide(widget);
	gtk_widget_show(groupcell->priv->button_name);
	gtk_widget_set_sensitive(groupcell->priv->button_name, TRUE);
	size = sizeof(char)*(strlen(new_name)+1);
	name = (char*)malloc(size);
	strncpy(name, new_name, size);
	if (dr_group_cell_get_container_type((GtkWidget*)data) == GROUP_CELL_TYPE_GROUP)
	{
		free(groupcell->priv->group->name);
		groupcell->priv->group->name = name;
	}
	else if (dr_group_cell_get_container_type((GtkWidget*)data) == GROUP_CELL_TYPE_FORM)
	{
		free(groupcell->priv->form->name);
		groupcell->priv->form->name = name;
	}
	return 0;
}

GtkWidget *mymenu_append(GtkWidget *menu, char *name)
{
	GtkWidget *menuitem;
	menuitem = gtk_menu_item_new_with_label(name);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);
	gtk_widget_show(menuitem);
	return menuitem;
}



		



static void dr_group_cell_init(DrGroupCell *groupcell)
{
	g_return_if_fail(IS_GROUP_CELL(groupcell));
	//GtkWidget *entry_groupname;
	GtkWidget *button;
	GtkWidget *image;
	//GtkEntryBuffer *entry_buffer;
	GtkWidget *entry;
	GtkWidget *menu;
	GtkWidget *menuitem;
	gulong button_del_id;
	gulong button_add_id;
	//GtkWidget *expander;
	//GtkWidget *expander_box;
	//const gchar *init_chars;

	//char groupname_buf[20];

	groupcell->priv = DR_GROUP_CELL_GET_PRIVATE(groupcell);
	groupcell->priv->next = NULL;
	groupcell->priv->entry_name = NULL;


	button = gtk_button_new();
	image = gtk_image_new_from_stock(GTK_STOCK_REMOVE, GTK_ICON_SIZE_MENU);
	button_del_id = g_signal_connect(button, "clicked", G_CALLBACK(dr_group_cell_remove_cell), groupcell);
	button_add_id = g_signal_connect(button, "clicked", G_CALLBACK(dr_group_cell_transplant_cell), groupcell);
	g_signal_handler_block(button, button_add_id);
	groupcell->priv->button_add_id = button_add_id;
	groupcell->priv->button_del_id = button_del_id;
	gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);
	gtk_button_set_image(GTK_BUTTON(button), image);
	gtk_box_pack_end(GTK_BOX(groupcell), button, FALSE, FALSE, 0);
	gtk_widget_show(button);
	gtk_widget_show(image);
	groupcell->priv->button_del = button;

	groupcell->priv->this_set = true;


	button = gtk_button_new_with_label("New Group");
	gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);
	g_signal_connect(button, "clicked", G_CALLBACK(group_cell_select_group), groupcell);
	g_signal_connect(button, "button_press_event", G_CALLBACK(group_cell_clicked), groupcell);
	gtk_box_pack_start(GTK_BOX(groupcell), button, TRUE, TRUE, 0);
	gtk_widget_show(button);
	groupcell->priv->button_name = button;


	entry = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entry), gtk_button_get_label(GTK_BUTTON(button)));
	gtk_entry_set_alignment(GTK_ENTRY (entry), 1);
	gtk_entry_set_width_chars(GTK_ENTRY (entry), 4);
	gtk_box_pack_start (GTK_BOX (groupcell), entry, TRUE, TRUE, 5);
	g_signal_connect(entry, "activate", G_CALLBACK(group_cell_set_name), groupcell);
	groupcell->priv->entry_name = entry;



	menu = gtk_menu_new();
	menuitem = mymenu_append(menu, "Edit Name");
	g_signal_connect(menuitem, "activate", G_CALLBACK(group_cell_toggle_to_entry), groupcell);
	menuitem = mymenu_append(menu, "Toggle Global");
	g_signal_connect(menuitem, "activate", G_CALLBACK(group_cell_toggle_group_scope), groupcell);
	menuitem = mymenu_append(menu, "Add Dots");
	g_signal_connect(menuitem, "activate", G_CALLBACK(select_add_to_group_cell), groupcell);
	menuitem = mymenu_append(menu, "Remove Dots");
	g_signal_connect(menuitem, "activate", G_CALLBACK(select_remove_from_group_cell), groupcell);
	menuitem = mymenu_append(menu, "Add Form");
	g_signal_connect(menuitem, "activate", G_CALLBACK(dr_group_cell_add_form), groupcell);
	gtk_widget_show(menu);
	groupcell->priv->context_menu = menu;
	return;
}

GtkWidget *dr_group_cell_new(void)
{
	return GTK_WIDGET(g_object_new(dr_group_cell_get_type(), NULL));
}


int dr_group_cell_get_container_type(GtkWidget *widget)
{
	DrGroupCell *groupcell = (DrGroupCell*)widget;
	g_return_val_if_fail(IS_GROUP_CELL(groupcell), -1);

	return groupcell->priv->type;
}

void dr_group_cell_set_container_type(GtkWidget *widget, int type)
{
	DrGroupCell *groupcell = (DrGroupCell*)widget;
	g_return_if_fail(IS_GROUP_CELL(groupcell));

	groupcell->priv->type = type;
	return;
}


group_t *dr_group_cell_get_group(GtkWidget *grpcell)
{
	DrGroupCell *groupcell = (DrGroupCell*)grpcell;
	if (!IS_GROUP_CELL(groupcell))
		return NULL;
	return groupcell->priv->group;
}


void dr_group_cell_set_group(GtkWidget *groupcell_widget, group_t *group)
{
	//select_t *select;
	//group_t *newgroup;
	DrGroupCell *groupcell = (DrGroupCell*)groupcell_widget;
	g_return_if_fail(IS_GROUP_CELL(groupcell));
	groupcell->priv->group = group;
	gtk_button_set_label(GTK_BUTTON(groupcell->priv->button_name), group->name);
	dr_group_cell_set_container_type(groupcell_widget, GROUP_CELL_TYPE_GROUP);
	return;
}


form_child_t *dr_group_cell_get_form(GtkWidget *widget)
{
	DrGroupCell *groupcell = (DrGroupCell*)widget;
	g_return_val_if_fail(IS_GROUP_CELL(groupcell), NULL);
	return groupcell->priv->form;
}


void dr_group_cell_set_form(GtkWidget *groupcell_widget, form_child_t *form)
{
	DrGroupCell *groupcell = (DrGroupCell*)groupcell_widget;
	g_return_if_fail(IS_GROUP_CELL(groupcell));
	groupcell->priv->form = form;
	gtk_button_set_label(GTK_BUTTON(groupcell->priv->button_name), form->name);
	dr_group_cell_set_container_type(groupcell_widget, GROUP_CELL_TYPE_FORM);
	return;
}

GtkWidget *dr_group_cell_get_next(GtkWidget *groupcell_widget)
{
	DrGroupCell *groupcell = (DrGroupCell*)groupcell_widget;
	if (!IS_GROUP_CELL(groupcell))
		return NULL;
	return (GtkWidget*)groupcell->priv->next;
}

bool dr_group_cell_has_next(GtkWidget *groupcell_widget)
{
	DrGroupCell *groupcell = (DrGroupCell*)groupcell_widget;
	if (!IS_GROUP_CELL(groupcell))
		return false;
	return (groupcell->priv->next != NULL);
}

GtkWidget *dr_group_cell_add(GtkWidget *groupcell_widget, group_t *group)
{
	DrGroupCell *groupcell = (DrGroupCell*)groupcell_widget;
	GtkWidget *last;
	last = dr_group_cell_new();
	dr_group_cell_set_group(last, group);
	if (!groupcell)
		return last;
	if (!IS_GROUP_CELL(groupcell))
		return NULL;
	((DrGroupCell*)last)->priv->next = groupcell;
	return last;
}

GtkWidget *dr_group_cell_append(GtkWidget *widget, group_t *group, form_child_t *form)
{
	DrGroupCell *groupcell = (DrGroupCell*)widget;
	GtkWidget *last;
	GtkWidget *curr;
	last = dr_group_cell_new();
	if (group)
		dr_group_cell_set_group(last, group);
	if (form)
		dr_group_cell_set_form(last, form);
	if (!groupcell)
		return last;
	if (!IS_GROUP_CELL(groupcell))
		return NULL;
	curr = widget;
	while (groupcell->priv->next)
		groupcell = groupcell->priv->next;
	groupcell->priv->next = (DrGroupCell*)last;
	return curr;
}


GtkWidget *dr_group_cell_add_before(GtkWidget *head_w, GtkWidget *widget, group_t *group, form_child_t *form)
{
	DrGroupCell *groupcell = (DrGroupCell*)widget;
	DrGroupCell *head = (DrGroupCell*)head_w;
	DrGroupCell *curr;
	GtkWidget *last;
	if (!groupcell)
	{
		last = dr_group_cell_new();
		if (group)
			dr_group_cell_set_group(last, group);
		if (form)
			dr_group_cell_set_form(last, form);
		return last;
	}
	if (!IS_GROUP_CELL(groupcell))
		return NULL;
	if (!head || head == groupcell)
	{
		last = dr_group_cell_new();
		if (group)
			dr_group_cell_set_group(last, group);
		if (form)
			dr_group_cell_set_form(last, form);
		((DrGroupCell*)last)->priv->next = head;
		return GTK_WIDGET(last);
	}
	curr = head;
	while(curr && curr->priv->next != groupcell)
		curr = curr->priv->next;
	if (!curr)
	{
		printf("error! not found!\n");
		exit(-1);
	}
	else
	{
		last = dr_group_cell_new();
		if (group)
			dr_group_cell_set_group(last, group);
		if (form)
			dr_group_cell_set_form(last, form);
		((DrGroupCell*)last)->priv->next = groupcell;
		curr->priv->next = (DrGroupCell*)last;
	}
	return last;
}


void dr_group_cell_delete_from(GtkWidget *widget)
{
	DrGroupCell *groupcell = (DrGroupCell*)widget;
	//DrGroupCell *curr = (DrGroupCell*)container;
	//DrGroupCell *gnext;
	if (!groupcell)// || !container)
		return;
	g_return_if_fail(IS_GROUP_CELL(groupcell));
	/*
	if (!IS_GROUP_CELL(curr))
		return NULL;
		*/
	/*
	if (container != widget)
	{
		while (curr->priv->next != groupcell)
			curr = curr->priv->next;
	}
	else
		curr = NULL;
		*/
	//gtk_widget_hide(groupcell->priv->button_add);

	/*
	if (curr != NULL)
		curr->priv->next = groupcell->priv->next;
	gnext = groupcell->priv->next;
	*/

	/*
	if (false)
	{
		// for now, always push onto stack
		groupcell->priv->group = NULL;
		groupcell->priv->form = NULL;
		spool = (stack_pool_t*)malloc(sizeof(stack_pool_t));
		spool->data = widget;
		g_trash_stack_push(&stack_pool, spool);
	}
	else
	*/
	{
		//gtk_widget_hide(groupcell->priv->button_del);
		gtk_widget_destroy(groupcell->priv->button_del);

		//gtk_widget_hide(groupcell->priv->button_name);
		gtk_widget_destroy(groupcell->priv->button_name);


		//gtk_widget_hide((GtkWidget*)groupcell);
		gtk_widget_destroy((GtkWidget*)groupcell);
	}
	return;
}


void dr_group_cell_set_is_this_set(GtkWidget *widget, bool is_this_set)
{
	GtkWidget *image;
	DrGroupCell *groupcell = (DrGroupCell*)widget;
	g_return_if_fail(IS_GROUP_CELL(groupcell));
	if (is_this_set == groupcell->priv->this_set)
		return;
	groupcell->priv->this_set = is_this_set;
	if (is_this_set == FALSE)
	{
		image = gtk_image_new_from_stock(GTK_STOCK_ADD, GTK_ICON_SIZE_MENU);
		g_signal_handler_block(groupcell->priv->button_del, groupcell->priv->button_del_id);
		g_signal_handler_unblock(groupcell->priv->button_del, groupcell->priv->button_add_id);
		gtk_button_set_image(GTK_BUTTON(groupcell->priv->button_del), image);
	}
	else
	{
		image = gtk_image_new_from_stock(GTK_STOCK_REMOVE, GTK_ICON_SIZE_MENU);
		g_signal_handler_block(groupcell->priv->button_del, groupcell->priv->button_add_id);
		g_signal_handler_unblock(groupcell->priv->button_del, groupcell->priv->button_del_id);
		gtk_button_set_image(GTK_BUTTON(groupcell->priv->button_del), image);
	}
	return;
}


bool dr_group_cell_get_is_this_set(GtkWidget *widget)
{
	DrGroupCell *groupcell = (DrGroupCell*)widget;
	g_return_val_if_fail(IS_GROUP_CELL(groupcell), false);
	return groupcell->priv->this_set;
}

void dr_group_cell_transplant_cell(GtkWidget *widget, gpointer *data)
{
	form_child_t *form;
	int excode;
	DrGroupCell *groupcell = (DrGroupCell*)data;
	form_parent_t *fparent;
	g_return_if_fail(IS_GROUP_CELL(groupcell));
	//form = form_copy(groupcell->priv->form);
	fparent = form_parent_find_with_form(pshow->topforms, groupcell->priv->form);
	excode = form_parent_add_set(pshow->topforms, fparent, pstate.setnum);
	if (excode == 1)
		mouse_currentMode = ADDFORM;
	form = form_parent_find_form_at_index(fparent, pstate.setnum);
	form_add_to_current_set(form);
	dr_sidebar_update((DrSidebar*)sidebar);
	dr_canvas_refresh(drill);
	return;
}


bool dr_group_cell_check_form_nonlocal(GtkWidget *widget)
{
	form_parent_t *fparent;
	DrGroupCell *groupcell = (DrGroupCell*)widget;
	g_return_val_if_fail(IS_GROUP_CELL(groupcell), false);
	if (!groupcell->priv->form)
		return false;
	fparent = form_parent_find_with_form(pshow->topforms, groupcell->priv->form);
	if (!fparent)
		return false;
	return form_parent_mapped_at_set(fparent, pstate.setnum);
}

