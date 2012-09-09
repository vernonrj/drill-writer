#include "drillwriter-gtk.h"
//#include "dr-sidebar-groups.h"



static void dr_group_cell_class_init(DrGroupCellClass *klass);
static void dr_group_cell_init(DrGroupCell *groupcell);

struct _DrGroupCellPrivate {
	GtkWidget *entry_name;
	//GtkWidget *button_add;
	GtkWidget *button_del;
	GtkWidget *button_name;
	GtkWidget *context_menu;
	GtkWidget *expander;
	GtkWidget *expander_box;
	group_t *group;
	form_t *form;
	int type;
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
	form_t *form = NULL;
	form = form_build_line(form, groupcell->priv->group->selects);
	//form_add_to_set(form);
	select_dots_discard();
	pstate.select = select_add_form(pstate.select, form, false);
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
	select_add_multiple(&groupcell->priv->group->selects, &pstate.select, false);
	return;
}


static void dr_group_cell_remove_cell(GtkWidget *widget, gpointer *data)
{
	DrGroupCell *groupcell = (DrGroupCell*)data;
	g_return_if_fail(IS_GROUP_CELL(groupcell));
	if (groupcell->priv->form)
	{
		// form
		pshow->sets->currset->forms = form_remove_from(groupcell->priv->form, pshow->sets->currset->forms);
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
	GtkWidget *expander;
	GtkWidget *expander_box;
	//const gchar *init_chars;

	//char groupname_buf[20];

	groupcell->priv = DR_GROUP_CELL_GET_PRIVATE(groupcell);
	groupcell->priv->next = NULL;
	groupcell->priv->entry_name = NULL;


	button = gtk_button_new();
	image = gtk_image_new_from_stock(GTK_STOCK_REMOVE, GTK_ICON_SIZE_MENU);
	gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);
	gtk_button_set_image(GTK_BUTTON(button), image);
	gtk_widget_show(image);
	gtk_widget_show(button);
	gtk_box_pack_end(GTK_BOX(groupcell), button, FALSE, FALSE, 0);
	g_signal_connect(button, "clicked", G_CALLBACK(dr_group_cell_remove_cell), groupcell);
	groupcell->priv->button_del = button;


	/*
	expander = gtk_expander_new("");
	gtk_box_pack_start(GTK_BOX(groupcell), expander, TRUE, TRUE, 0);
	groupcell->priv->expander = expander;
	gtk_expander_set_label_widget(GTK_EXPANDER(expander), button);
	gtk_widget_show(expander);

	button = gtk_button_new_with_label("test");
	gtk_container_add(GTK_CONTAINER(expander), button);
	gtk_widget_show(button);
	*/


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


form_t *dr_group_cell_get_form(GtkWidget *widget)
{
	DrGroupCell *groupcell = (DrGroupCell*)widget;
	g_return_val_if_fail(IS_GROUP_CELL(groupcell), NULL);
	return groupcell->priv->form;
}


void dr_group_cell_set_form(GtkWidget *groupcell_widget, form_t *form)
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

GtkWidget *dr_group_cell_append(GtkWidget *widget, group_t *group, form_t *form)
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


GtkWidget *dr_group_cell_delete_from(GtkWidget *widget, GtkWidget *container)
{
	DrGroupCell *groupcell = (DrGroupCell*)widget;
	DrGroupCell *curr = (DrGroupCell*)container;
	//gtk_widget_hide(widget);
	if (!groupcell || !container)
		return NULL;
	if (!IS_GROUP_CELL(groupcell))
		return NULL;
	if (!IS_GROUP_CELL(curr))
		return NULL;
	if (container != widget)
	{
		while (curr->priv->next != groupcell)
			curr = curr->priv->next;
	}
	else
		curr = NULL;
	//gtk_widget_hide(groupcell->priv->button_add);
	//g_free(groupcell->priv->button_add);

	gtk_widget_hide(groupcell->priv->button_del);
	//g_free(groupcell->priv->button_del);

	gtk_widget_hide(groupcell->priv->button_name);
	//g_free(groupcell->priv->button_name);


	gtk_widget_hide((GtkWidget*)groupcell);
	if (curr != NULL)
		curr->priv->next = groupcell->priv->next;
	return (GtkWidget*)groupcell->priv->next;
}

